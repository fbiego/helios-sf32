#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/helios_alarms.c"

static char *stored;
static bool blocked, fail_save, sounding;
static unsigned beeps, stops, save_calls;
static rt_tick_t ticks;
rt_tick_t rt_tick_get(void) { return ticks; }
rt_tick_t rt_tick_from_millisecond(int32_t ms) { return ms; }
rt_base_t rt_hw_interrupt_disable(void) { return 0; }
void rt_hw_interrupt_enable(rt_base_t value) { (void)value; }
void helios_sounds_alarm(bool ringing)
{ sounding = ringing; if (ringing) ++beeps; else ++stops; }
cJSON *helios_storage_document(void)
{ cJSON *doc = cJSON_CreateObject(); if (doc && !cJSON_AddNumberToObject(doc, "version", 1)) { cJSON_Delete(doc); return NULL; } return doc; }
cJSON *helios_storage_load(helios_store_id_t id, bool *exists)
{ assert(id == HELIOS_STORE_ALARMS); *exists = stored != NULL; return stored ? cJSON_Parse(stored) : NULL; }
void helios_storage_preserve_invalid(helios_store_id_t id)
{ assert(id == HELIOS_STORE_ALARMS); blocked = true; }
bool helios_storage_save(helios_store_id_t id, const cJSON *doc)
{
    assert(id == HELIOS_STORE_ALARMS); ++save_calls;
    if (blocked || fail_save) return false;
    char *value = cJSON_PrintUnformatted(doc); if (!value) return false;
    free(stored); stored = value; return true;
}
static void received(chronos_config_t config, uint32_t a, uint32_t b)
{ assert(config == CHRONOS_CONFIG_ALARM); helios_alarms_request(a, b); }
static void phone(unsigned slot, bool enabled, unsigned hour, unsigned minute, unsigned repeat)
{
    uint8_t payload[] = {slot, enabled, hour, minute, repeat};
    uint8_t packet[32]; size_t len;
    assert(chronos_core_build_packet(0xAB, CHRONOS_PROTOCOL_STANDARD, 0x73, 0x80,
        payload, sizeof(payload), packet, sizeof(packet), &len) == CHRONOS_STATUS_OK);
    assert(chronos_core_receive_ble_packet(packet, len) == CHRONOS_STATUS_OK);
}
static struct tm calendar(unsigned day, unsigned hour, unsigned minute)
{
    struct tm value = {0};
    value.tm_year = 126; value.tm_mon = 10; value.tm_mday = 9 + day;
    value.tm_hour = hour; value.tm_min = minute; value.tm_isdst = -1;
    assert(mktime(&value) != (time_t)-1); return value; /* Nov 9, 2026 is Monday. */
}
static bool poll(unsigned day, unsigned hour, unsigned minute, uint32_t button)
{ struct tm value = calendar(day, hour, minute); return helios_alarms_poll(&value, button); }
static void clear(void)
{
    free(stored); stored = NULL; blocked = fail_save = sounding = false;
    beeps = stops = save_calls = 0; pending_mask = 0; ticks += 1000;
    helios_alarms_init();
}
static void *no_memory(size_t size) { (void)size; return NULL; }

int main(void)
{
    setenv("TZ", "UTC", 1); tzset();
    chronos_core_init(); chronos_core_set_config_cb(received);
    clear(); phone(0, false, 7, 0, 1); poll(0, 7, 0, 100);
    assert(!sounding && stored); /* Disabled slots still persist. */
    phone(0, true, 7, 0, 1); poll(0, 6, 59, 100); assert(!sounding);
    poll(0, 7, 0, 100); assert(sounding && beeps == 1 && fired_day[0] == 20261109);
    ticks += 500; poll(0, 7, 0, 100); assert(beeps == 2);
    ticks += 500; poll(0, 7, 0, 100); assert(beeps == 2);
    assert(poll(0, 7, 0, 101)); assert(!sounding && stops == 1);
    poll(0, 7, 0, 101); assert(beeps == 2);
    /* A duplicate phone sync and restart cannot retrigger this occurrence. */
    phone(0, true, 7, 0, 1); poll(0, 7, 0, 101); assert(beeps == 2);
    helios_alarms_init(); poll(0, 7, 0, 101); assert(beeps == 2);
    assert(chronos_core_get_alarm(0)->enabled);
    poll(6, 7, 0, 101); assert(beeps == 2); /* Not Sunday. */
    poll(7, 7, 0, 101); assert(beeps == 3 && sounding);
    phone(0, false, 7, 0, 1); poll(7, 7, 0, 101); assert(!sounding && stops == 2);

    /* Two short beeps per cycle; dismissal cancels the pending second beep. */
    clear(); phone(0, true, 7, 0, 0x7f); poll(0, 7, 0, 0); assert(beeps == 1);
    ticks += 249; poll(0, 7, 0, 0); assert(beeps == 1);
    ticks += 1; poll(0, 7, 0, 0); assert(beeps == 2);
    ticks += 999; poll(0, 7, 0, 0); assert(beeps == 2);
    ticks += 1; poll(0, 7, 0, 0); assert(beeps == 3);
    ticks += 100; assert(poll(0, 7, 0, 1)); assert(!sounding);
    ticks += 150; poll(0, 7, 0, 1); assert(beeps == 3);
    clear(); ticks = UINT32_MAX - 100;
    phone(0, true, 7, 0, 0x7f); poll(0, 7, 0, 0);
    ticks += 250; poll(0, 7, 0, 0); assert(beeps == 2);

    /* Every weekday bit, plus Chronos daily masks and no selected days. */
    for (unsigned bit = 0; bit < 7; ++bit) {
        clear(); phone(0, true, 7, 0, 1u << bit);
        poll((bit + 1) % 7, 7, 0, 0); assert(!sounding);
        poll(bit, 7, 0, 0); assert(sounding);
    }
    const uint8_t daily[] = {0x7f, 0x80};
    for (unsigned i = 0; i < 2; ++i) {
        clear(); phone(0, true, 0, 0, daily[i]);
        poll(6, 0, 0, 0); assert(sounding); /* Sunday midnight. */
    }
    clear(); phone(0, true, 7, 0, 0); poll(0, 7, 0, 0); assert(!sounding);

    /* All eight slots survive restart and simultaneous alarms share a tone. */
    clear();
    for (unsigned i = 0; i < CHRONOS_ALARM_SIZE; ++i) phone(i, true, 7, 0, 0x7f);
    poll(0, 6, 59, 0); helios_alarms_init();
    poll(0, 7, 0, 0); assert(ringing_mask == 255 && beeps == 1);
    ticks += 3600000; poll(0, 8, 0, 0); assert(sounding && beeps == 2);
    assert(poll(0, 8, 0, 1)); assert(!sounding);
    helios_alarms_init(); poll(0, 7, 0, 1); assert(!sounding);

    /* Invalid phone data and bad RTC values do not create a ringing alarm. */
    clear(); phone(0, true, 25, 0, 0x7f); poll(0, 7, 0, 0); assert(!sounding && !stored);
    helios_alarms_request(8, 0x07007f01); poll(0, 7, 0, 0); assert(!stored);
    phone(0, true, 7, 0, 0x7f);
    struct tm bad = calendar(0, 7, 0); bad.tm_year = 70;
    helios_alarms_poll(&bad, 0); assert(!sounding);
    helios_alarms_poll(NULL, 0); assert(!sounding);

    /* Failed saves/OOM stay dirty and retry, without busy allocation loops. */
    clear(); fail_save = true; phone(0, true, 7, 0, 1); poll(0, 6, 59, 0);
    assert(dirty && save_calls == 1 && !stored);
    poll(0, 6, 59, 0); assert(save_calls == 1);
    fail_save = false; ticks += 1000; poll(0, 6, 59, 0); assert(!dirty && stored);
    cJSON_Hooks hooks = {no_memory, free}; cJSON_InitHooks(&hooks);
    phone(1, true, 8, 0, 1); poll(0, 6, 59, 0); assert(dirty);
    cJSON_InitHooks(NULL); ticks += 1000; poll(0, 6, 59, 0); assert(!dirty);

    /* Corrupt schema is preserved; no partial restoration of other slots. */
    cJSON *doc = cJSON_Parse(stored);
    cJSON *row = cJSON_GetArrayItem(cJSON_GetObjectItem(doc, "items"), 1);
    cJSON_SetNumberValue(cJSON_GetObjectItem(row, "hour"), 99);
    free(stored); stored = cJSON_PrintUnformatted(doc); cJSON_Delete(doc);
    helios_alarms_init(); assert(blocked && !alarms[0].enabled && !alarms[1].enabled);
    free(stored);
    puts("Alarms: real phone packets, 8-slot persistence/restart, all repeat days, midnight, dismissal/no retrigger, disable, invalid data, OOM and save retries passed");
    return 0;
}
