#include "helios_alarms.h"
#include "helios_storage.h"
#include "helios_sounds.h"
#include "chronos_core.h"
#include <rtthread.h>
#include <rthw.h>
#include <string.h>

#define LOG_TAG "helios.alarms"
#include "log.h"

static chronos_alarm_t alarms[CHRONOS_ALARM_SIZE];
static uint32_t fired_day[CHRONOS_ALARM_SIZE];
static chronos_alarm_t pending[CHRONOS_ALARM_SIZE];
static uint32_t pending_mask;
static uint32_t ringing_mask, ringing_button_sequence;
static rt_tick_t last_beep, last_save;
static bool dirty, save_attempted;
static bool second_beep_pending;

#define ALARM_SECOND_BEEP_MS 250
#define ALARM_PAIR_PERIOD_MS 1000

static bool integer(const cJSON *row, const char *key, int maximum, uint32_t *out)
{
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(row, key);
    if (!cJSON_IsNumber(v) || v->valuedouble < 0 || v->valuedouble > maximum ||
        v->valuedouble != v->valueint)
        return false;
    *out = (uint32_t)v->valueint;
    return true;
}

static bool restore(const cJSON *doc)
{
    const cJSON *items = cJSON_GetObjectItemCaseSensitive(doc, "items");
    if (!cJSON_IsArray(items) || cJSON_GetArraySize(items) != CHRONOS_ALARM_SIZE)
        return false;
    chronos_alarm_t loaded[CHRONOS_ALARM_SIZE] = {0};
    uint32_t days[CHRONOS_ALARM_SIZE] = {0};
    for (unsigned i = 0; i < CHRONOS_ALARM_SIZE; ++i) {
        const cJSON *row = cJSON_GetArrayItem(items, i);
        const cJSON *enabled = cJSON_GetObjectItemCaseSensitive(row, "enabled");
        uint32_t hour, minute, repeat;
        if (!integer(row, "hour", 23, &hour) || !integer(row, "minute", 59, &minute) ||
            !integer(row, "repeat", 255, &repeat) || !cJSON_IsBool(enabled) ||
            !integer(row, "fired_day", 20991231, &days[i]))
            return false;
        loaded[i] = (chronos_alarm_t){hour, minute, repeat, cJSON_IsTrue(enabled)};
    }
    memcpy(alarms, loaded, sizeof(alarms));
    memcpy(fired_day, days, sizeof(fired_day));
    return true;
}

static void save(rt_tick_t now)
{
    if (!dirty || (save_attempted && (rt_tick_t)(now - last_save) < rt_tick_from_millisecond(1000)))
        return;
    save_attempted = true;
    last_save = now;
    cJSON *doc = helios_storage_document();
    if (!doc) return;
    cJSON *items = cJSON_AddArrayToObject(doc, "items");
    if (!items) goto done;
    for (unsigned i = 0; i < CHRONOS_ALARM_SIZE; ++i) {
        cJSON *row = cJSON_CreateObject();
        if (!row) goto done;
        if (!cJSON_AddItemToArray(items, row)) { cJSON_Delete(row); goto done; }
        if (!cJSON_AddNumberToObject(row, "hour", alarms[i].hour) ||
            !cJSON_AddNumberToObject(row, "minute", alarms[i].minute) ||
            !cJSON_AddNumberToObject(row, "repeat", alarms[i].repeat) ||
            !cJSON_AddBoolToObject(row, "enabled", alarms[i].enabled) ||
            !cJSON_AddNumberToObject(row, "fired_day", fired_day[i])) goto done;
    }
    if (helios_storage_save(HELIOS_STORE_ALARMS, doc)) dirty = false;
done:
    cJSON_Delete(doc);
}

void helios_alarms_init(void)
{
    memset(alarms, 0, sizeof(alarms));
    memset(fired_day, 0, sizeof(fired_day));
    ringing_mask = 0;
    dirty = save_attempted = false;
    bool exists;
    cJSON *doc = helios_storage_load(HELIOS_STORE_ALARMS, &exists);
    if (doc && !restore(doc)) {
        helios_storage_preserve_invalid(HELIOS_STORE_ALARMS);
        LOG_W("invalid alarm document preserved");
    }
    cJSON_Delete(doc);
    rt_base_t level = rt_hw_interrupt_disable();
    memcpy(chronos_core_get_state()->alarms, alarms, sizeof(alarms));
    rt_hw_interrupt_enable(level);
}

void helios_alarms_request(uint32_t slot, uint32_t packed)
{
    if (slot >= CHRONOS_ALARM_SIZE) return;
    chronos_alarm_t alarm = {packed >> 24, (packed >> 16) & 0xff,
                             (packed >> 8) & 0xff, (packed & 0xff) != 0};
    if (alarm.hour > 23 || alarm.minute > 59) {
        LOG_W("invalid phone alarm ignored slot=%lu", (unsigned long)slot);
        return;
    }
    rt_base_t level = rt_hw_interrupt_disable();
    pending[slot] = alarm;
    pending_mask |= 1u << slot;
    rt_hw_interrupt_enable(level);
}

bool helios_alarms_poll(const struct tm *calendar, uint32_t button_sequence)
{
    bool was_ringing = ringing_mask != 0;
    chronos_alarm_t updates[CHRONOS_ALARM_SIZE];
    rt_base_t level = rt_hw_interrupt_disable();
    uint32_t mask = pending_mask;
    memcpy(updates, pending, sizeof(updates));
    pending_mask = 0;
    rt_hw_interrupt_enable(level);
    for (unsigned i = 0; i < CHRONOS_ALARM_SIZE; ++i) {
        if (!(mask & (1u << i))) continue;
        chronos_alarm_t *old = &alarms[i], *value = &updates[i];
        if (old->hour != value->hour || old->minute != value->minute ||
            old->repeat != value->repeat || old->enabled != value->enabled) {
            *old = *value;
            fired_day[i] = 0;
            ringing_mask &= ~(1u << i);
            dirty = true;
            save_attempted = false;
        }
        level = rt_hw_interrupt_disable();
        chronos_core_get_state()->alarms[i] = *old;
        rt_hw_interrupt_enable(level);
    }

    bool dismissed = ringing_mask && button_sequence != ringing_button_sequence;
    if (dismissed) ringing_mask = 0;
    uint32_t before = ringing_mask;
    if (calendar && calendar->tm_year >= 120 && calendar->tm_year <= 199 &&
        calendar->tm_wday >= 0 && calendar->tm_wday <= 6 &&
        calendar->tm_mon >= 0 && calendar->tm_mon <= 11 &&
        calendar->tm_mday >= 1 && calendar->tm_mday <= 31 && !dismissed) {
        uint32_t day = (calendar->tm_year + 1900) * 10000u +
                       (calendar->tm_mon + 1) * 100u + calendar->tm_mday;
        uint8_t weekday = 1u << (calendar->tm_wday == 0 ? 6 : calendar->tm_wday - 1);
        for (unsigned i = 0; i < CHRONOS_ALARM_SIZE; ++i) {
            const chronos_alarm_t *alarm = &alarms[i];
            if (!alarm->enabled || alarm->hour != calendar->tm_hour ||
                alarm->minute != calendar->tm_min || fired_day[i] == day ||
                !(alarm->repeat == 0x80 || (alarm->repeat & weekday))) continue;
            fired_day[i] = day;
            dirty = true;
            save_attempted = false;
            ringing_mask |= 1u << i;
            LOG_I("ringing slot=%u", i);
        }
    }
    rt_tick_t now = rt_tick_get();
    if (ringing_mask) {
        if (!before) ringing_button_sequence = button_sequence;
        rt_tick_t elapsed = (rt_tick_t)(now - last_beep);
        if (!before || elapsed >= rt_tick_from_millisecond(ALARM_PAIR_PERIOD_MS)) {
            helios_sounds_alarm(true);
            last_beep = now;
            second_beep_pending = true;
        } else if (second_beep_pending && elapsed >= rt_tick_from_millisecond(ALARM_SECOND_BEEP_MS)) {
            helios_sounds_alarm(true);
            second_beep_pending = false;
        }
    } else if (was_ringing)
        helios_sounds_alarm(false);
    save(now);
    return dismissed;
}
