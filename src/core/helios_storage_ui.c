#include "helios_storage_ui.h"
#include "helios_storage.h"
#include "helios_prefs.h"
#include "helios_user.h"
#include "chronos_core.h"
#include "helios_ui/helios_ui.h"
#include "helios_ui/custom/apps/notifications/notifications.h"
#include "helios_ui/custom/apps/contacts/contacts.h"
#include "helios_ui/custom/apps/weather/weather.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#define LOG_TAG "helios.storage.ui"
#include "log.h"

static bool dirty[HELIOS_STORE_COUNT];
static bool restoring;
static int sos_contact = -1;
static char qr[CHRONOS_QR_SIZE][CHRONOS_TEXT_LARGE_SIZE];
/* Stable serialized IDs, never persist image or LVGL object addresses. */
static const void **const icons[] = {
    &icon_nt_chat, &icon_nt_messenger, &icon_nt_whatsapp, &icon_nt_twitter,
    &icon_nt_mail, &icon_nt_qq, &icon_nt_skype, &icon_nt_line, &icon_nt_weibo,
    &icon_nt_kakao, &icon_nt_viber, &icon_nt_vk, &icon_nt_wechat, &icon_nt_paypal,
    &icon_nt_telegram, &icon_nt_instagram, &icon_nt_calendar, &icon_nt_hangouts,
    &icon_nt_download, &icon_nt_facebook, &icon_nt_snapchat, &icon_nt_tiktok,
    &icon_nt_dingtalk, &icon_mpesa_watch_32
};
static int icon_id(const void *icon)
{
    for (unsigned i = 0; i < sizeof(icons) / sizeof(icons[0]); i++) if (*icons[i] == icon) return i;
    return 0;
}
static const void *icon_ptr(int id)
{ return id >= 0 && id < (int)(sizeof(icons) / sizeof(icons[0])) ? *icons[id] : *icons[0]; }
static const cJSON *item(const cJSON *doc, const char *key)
{ return cJSON_GetObjectItemCaseSensitive(doc, key); }
static const char *str(const cJSON *doc, const char *key)
{ const cJSON *v = item(doc, key); return cJSON_IsString(v) ? v->valuestring : ""; }
static int num(const cJSON *doc, const char *key)
{ const cJSON *v = item(doc, key); return cJSON_IsNumber(v) ? v->valueint : 0; }
static bool string_fits(const cJSON *doc, const char *key, size_t capacity)
{ const cJSON *v = item(doc, key); return cJSON_IsString(v) && strlen(v->valuestring) < capacity; }

static void notifications_changed(helios_notifications_event_t event, const helios_notification_t *n, void *arg)
{ (void)event; (void)n; (void)arg; if (!restoring) dirty[HELIOS_STORE_NOTIFICATIONS] = true; }
static void contacts_changed(helios_contacts_event_t event, const helios_contact_t *c, void *arg)
{
    (void)c; (void)arg;
    if (!restoring) {
        if (event == HELIOS_CONTACTS_EVENT_CLEARED) sos_contact = -1;
        dirty[HELIOS_STORE_CONTACTS] = true;
    }
}
static void weather_changed(helios_weather_event_t event, void *arg)
{ (void)event; (void)arg; if (!restoring) dirty[HELIOS_STORE_WEATHER] = true; }

#define N(o,k,v) do { if (!cJSON_AddNumberToObject(o,k,v)) goto failed; } while (0)
#define S(o,k,v) do { if (!cJSON_AddStringToObject(o,k,v)) goto failed; } while (0)
#define B(o,k,v) do { if (!cJSON_AddBoolToObject(o,k,v)) goto failed; } while (0)

static cJSON *snapshot(helios_store_id_t id)
{
    cJSON *doc = helios_storage_document();
    if (!doc) return NULL;
    N(doc, "updated_at", (double)time(NULL));
    if (id == HELIOS_STORE_USER) {
        if (!helios_user_serialize(doc)) goto failed;
        return doc;
    }
    cJSON *array = cJSON_AddArrayToObject(doc, "items");
    if (!array) goto failed;
    if (id == HELIOS_STORE_NOTIFICATIONS) {
        for (uint32_t i = 0; i < helios_notifications_count(); i++) {
            const helios_notification_t *v = helios_notifications_get(i);
            cJSON *row = cJSON_CreateObject();
            if (!row || !cJSON_AddItemToArray(array, row)) { cJSON_Delete(row); goto failed; }
            S(row, "title", v->title); S(row, "time", v->time); S(row, "message", v->message);
            N(row, "icon", icon_id(v->icon)); B(row, "read", v->read);
        }
    } else if (id == HELIOS_STORE_CONTACTS) {
        N(doc, "sos_index", sos_contact);
        for (uint32_t i = 0; i < helios_contacts_count(); i++) {
            const helios_contact_t *v = helios_contacts_get(i);
            cJSON *row = cJSON_CreateObject();
            if (!row || !cJSON_AddItemToArray(array, row)) { cJSON_Delete(row); goto failed; }
            S(row, "name", v->name); S(row, "number", v->number);
        }
    } else if (id == HELIOS_STORE_QR) {
        for (unsigned i = 0; i < CHRONOS_QR_SIZE; i++) {
            cJSON *value = cJSON_CreateString(qr[i]);
            if (!value || !cJSON_AddItemToArray(array, value)) { cJSON_Delete(value); goto failed; }
        }
    } else if (id == HELIOS_STORE_WEATHER) {
        S(doc, "city", lv_subject_get_string(&sb_weather_location));
        S(doc, "update_time", lv_subject_get_string(&sb_weather_update_time));
        S(doc, "unit", lv_subject_get_string(&sb_weather_temp_unit));
        N(doc, "code", lv_subject_get_int(&sb_weather_code));
        N(doc, "temp", lv_subject_get_int(&sb_weather_temp));
        N(doc, "high", lv_subject_get_int(&sb_weather_temp_high));
        N(doc, "low", lv_subject_get_int(&sb_weather_temp_low));
        N(doc, "humidity", lv_subject_get_int(&sb_weather_humidity));
        N(doc, "uv", lv_subject_get_int(&sb_weather_uv));
        for (uint32_t i = 0; i < helios_weather_hourly_count(); i++) {
            const helios_hourly_forecast_t *v = helios_weather_hourly_get(i);
            cJSON *row = cJSON_CreateObject();
            if (!row || !cJSON_AddItemToArray(array, row)) { cJSON_Delete(row); goto failed; }
            N(row, "icon", v->icon_id); S(row, "time", v->time); S(row, "temp", v->temp); S(row, "humidity", v->humidity);
        }
        cJSON *days = cJSON_AddArrayToObject(doc, "daily");
        if (!days) goto failed;
        for (uint32_t i = 0; i < helios_weather_daily_count(); i++) {
            const helios_daily_forecast_t *v = helios_weather_daily_get(i);
            cJSON *row = cJSON_CreateObject();
            if (!row || !cJSON_AddItemToArray(days, row)) { cJSON_Delete(row); goto failed; }
            N(row, "icon", v->icon_id); S(row, "day", v->day); S(row, "temp", v->temp);
        }
    }
    return doc;
failed:
    cJSON_Delete(doc); return NULL;
}

static bool restore(helios_store_id_t id, const cJSON *doc)
{
    if (id == HELIOS_STORE_USER) return helios_user_restore(doc);
    const cJSON *array = item(doc, "items"), *row;
    if (!cJSON_IsArray(array)) return false;
    int count = cJSON_GetArraySize(array);
    if (id == HELIOS_STORE_NOTIFICATIONS) {
        if (count > HELIOS_NOTIFICATIONS_MAX) return false;
        cJSON_ArrayForEach(row, array) {
            if (!string_fits(row, "title", HELIOS_NOTIFICATION_TITLE_MAX) ||
                !string_fits(row, "time", HELIOS_NOTIFICATION_TIME_MAX) ||
                !string_fits(row, "message", HELIOS_NOTIFICATION_MESSAGE_MAX) ||
                !cJSON_IsNumber(item(row, "icon")) || !cJSON_IsBool(item(row, "read"))) return false;
        }
        cJSON_ArrayForEach(row, array) {
            helios_notifications_add(icon_ptr(num(row, "icon")), str(row, "title"), str(row, "time"), str(row, "message"));
            const helios_notification_t *v = helios_notifications_get(helios_notifications_count() - 1);
            helios_notifications_set_read(v->id, cJSON_IsTrue(item(row, "read")));
        }
    } else if (id == HELIOS_STORE_CONTACTS) {
        if (count > HELIOS_CONTACTS_MAX) return false;
        cJSON_ArrayForEach(row, array) {
            if (!string_fits(row, "name", HELIOS_CONTACT_NAME_MAX) || !string_fits(row, "number", HELIOS_CONTACT_NUMBER_MAX)) return false;
        }
        const cJSON *sos = item(doc, "sos_index");
        if (!cJSON_IsNumber(sos) || sos->valuedouble != sos->valueint || sos->valueint < -1 || sos->valueint >= count) return false;
        sos_contact = sos->valueint;
        chronos_core_t *core = chronos_core_get_state(); core->contact_size = count; core->sos_contact = sos_contact;
        int i = 0;
        cJSON_ArrayForEach(row, array) {
            helios_contacts_add(str(row, "name"), str(row, "number"));
            snprintf(core->contacts[i].name, sizeof(core->contacts[i].name), "%s", str(row, "name"));
            snprintf(core->contacts[i].number, sizeof(core->contacts[i].number), "%s", str(row, "number")); i++;
        }
    } else if (id == HELIOS_STORE_QR) {
        if (count != CHRONOS_QR_SIZE) return false;
        cJSON_ArrayForEach(row, array) if (!cJSON_IsString(row) || strlen(row->valuestring) >= CHRONOS_TEXT_LARGE_SIZE) return false;
        int i = 0;
        cJSON_ArrayForEach(row, array) {
            snprintf(qr[i], sizeof(qr[i]), "%s", row->valuestring);
            snprintf(chronos_core_get_state()->qr_links[i], sizeof(qr[i]), "%s", qr[i]); i++;
        }
    } else if (id == HELIOS_STORE_WEATHER) {
        const cJSON *days = item(doc, "daily");
        if (count > HELIOS_HOURLY_FORECAST_MAX || !cJSON_IsArray(days) || cJSON_GetArraySize(days) > HELIOS_DAILY_FORECAST_MAX) return false;
        if (!count && !cJSON_GetArraySize(days)) return true;
        if (!string_fits(doc, "city", 256) || !string_fits(doc, "update_time", 256) || !string_fits(doc, "unit", 16)) return false;
        const char *keys[] = {"code", "temp", "high", "low", "humidity", "uv"};
        for (unsigned i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            const cJSON *v = item(doc, keys[i]);
            if (!cJSON_IsNumber(v) || v->valuedouble < INT32_MIN || v->valuedouble > INT32_MAX || v->valuedouble != v->valueint) return false;
        }
        cJSON_ArrayForEach(row, array) {
            if (!string_fits(row, "time", HELIOS_WEATHER_TEXT_MAX) || !string_fits(row, "temp", HELIOS_WEATHER_TEXT_MAX) ||
                !string_fits(row, "humidity", HELIOS_WEATHER_TEXT_MAX) || !cJSON_IsNumber(item(row, "icon"))) return false;
        }
        cJSON_ArrayForEach(row, days) {
            if (!string_fits(row, "day", HELIOS_WEATHER_TEXT_MAX) || !string_fits(row, "temp", HELIOS_WEATHER_TEXT_MAX) ||
                !cJSON_IsNumber(item(row, "icon"))) return false;
        }
        const cJSON *updated = item(doc, "updated_at"); time_t now = time(NULL);
        /* Retain the file, but don't present a forecast from an unrelated date. */
        if (!cJSON_IsNumber(updated) || updated->valuedouble > (double)now || (double)now - updated->valuedouble > 7 * 86400) return true;
        helios_subject_set_weather_location(str(doc, "city")); helios_subject_set_weather_update_time(str(doc, "update_time"));
        helios_subject_set_weather_temp_unit(str(doc, "unit"));
        helios_subject_set_weather_code(num(doc, "code")); helios_subject_set_weather_icon((void *)helios_weather_icon_get(num(doc, "code")));
        helios_subject_set_weather_condition(helios_weather_condition_get(num(doc, "code")));
        helios_subject_set_weather_temp(num(doc, "temp")); helios_subject_set_weather_temp_high(num(doc, "high"));
        helios_subject_set_weather_temp_low(num(doc, "low")); helios_subject_set_weather_humidity(num(doc, "humidity")); helios_subject_set_weather_uv(num(doc, "uv"));
        cJSON_ArrayForEach(row, array) helios_weather_hourly_add(num(row, "icon"), str(row, "time"), str(row, "temp"), str(row, "humidity"));
        cJSON_ArrayForEach(row, days) helios_weather_daily_add(num(row, "icon"), str(row, "day"), str(row, "temp"));
    }
    return true;
}

void helios_storage_ui_init(void)
{
    restoring = true;
    memset(dirty, 0, sizeof(dirty));
    helios_user_reset_defaults();
    /* Firmware starts with empty collections; demo content remains available
     * to the UI editor/simulator and is not written into user data files. */
    helios_notifications_clear(); helios_contacts_clear();
    helios_weather_hourly_clear(); helios_weather_daily_clear();
    helios_subject_set_weather_location(""); helios_subject_set_weather_update_time("");
    helios_subject_set_weather_temp(0); helios_subject_set_weather_temp_high(0); helios_subject_set_weather_temp_low(0);
    helios_subject_set_weather_humidity(0); helios_subject_set_weather_uv(0);
    helios_subject_set_weather_condition(""); helios_subject_set_weather_icon(NULL);
    for (int i = HELIOS_STORE_WEATHER; i < HELIOS_STORE_COUNT; i++) {
        bool exists; cJSON *doc = helios_storage_load(i, &exists);
        if (doc && !restore(i, doc)) {
            helios_storage_preserve_invalid(i);
            LOG_W("invalid %d document preserved", i);
        } else if (!exists) dirty[i] = true;
        cJSON_Delete(doc);
    }
    if (!helios_notifications_observer_add(notifications_changed, NULL) ||
        !helios_contacts_observer_add(contacts_changed, NULL) ||
        !helios_weather_observer_add(weather_changed, NULL)) LOG_E("storage observer registration failed");
    restoring = false;
}

void helios_storage_ui_poll(void)
{
    helios_prefs_retry_save();
    for (int i = HELIOS_STORE_WEATHER; i < HELIOS_STORE_COUNT; i++) {
        if (!dirty[i]) continue;
        cJSON *doc = snapshot(i);
        if (doc && helios_storage_save(i, doc)) dirty[i] = false;
        cJSON_Delete(doc);
    }
}

void helios_storage_ui_sync_contacts(void)
{
    helios_contacts_clear();
    int count = chronos_core_get_contact_count();
    sos_contact = chronos_core_get_sos_contact_index();
    if (sos_contact >= count || sos_contact >= HELIOS_CONTACTS_MAX) sos_contact = -1;
    for (int i = 0; i < count && i < HELIOS_CONTACTS_MAX; i++) {
        const chronos_contact_t *c = chronos_core_get_contact(i);
        if (c) helios_contacts_add(c->name, c->number);
    }
}
void helios_storage_ui_sync_qr(uint32_t index)
{
    if (index >= CHRONOS_QR_SIZE) return;
    const char *value = chronos_core_get_qr_at(index);
    if (value) { snprintf(qr[index], sizeof(qr[index]), "%s", value); dirty[HELIOS_STORE_QR] = true; }
}

void helios_storage_ui_sync_user(uint32_t a, uint32_t b)
{
    helios_user_profile_t p, previous = helios_user_get();
    if (!helios_user_decode(a, b, &p) || !helios_user_set(&p)) {
        LOG_W("invalid phone user profile ignored");
        return;
    }
    if (p.age != previous.age || p.height != previous.height || p.weight != previous.weight ||
        p.step_length != previous.step_length || p.target_steps != previous.target_steps)
        dirty[HELIOS_STORE_USER] = true;
}
