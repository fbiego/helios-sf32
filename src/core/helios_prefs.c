#include "helios_prefs.h"
#include "helios_storage.h"
#include <rtthread.h>
#include <string.h>
#include <limits.h>
#ifdef PKG_USING_FLASHDB
#include <flashdb.h>
#endif
#define LOG_TAG "helios.prefs"
#include "log.h"

static cJSON *settings, *values;
static struct rt_mutex settings_lock;
static bool initialized, dirty;

/* Import the existing typed FlashDB keys once. The old partition is left
 * intact; after settings.json exists it is never used as a second authority. */
static bool migrate_legacy(void)
{
#ifdef PKG_USING_FLASHDB
    static struct fdb_kvdb legacy;
    struct fdb_default_kv defaults = {.kvs = NULL, .num = 0};
    if (fdb_kvdb_init(&legacy, "helios", "dfu", &defaults, NULL) != FDB_NO_ERR) return false;
    bool ok = true;
    const char *keys[] = {"ui.app_list_mode", "screen.brightness", "screen.rotation",
        "screen.raise_to_wake", "screen.timeout", "notifications.focusable", "ui.language"};
    for (unsigned i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        struct fdb_blob blob; int32_t value;
        size_t size = fdb_kv_get_blob(&legacy, keys[i], fdb_blob_make(&blob, &value, sizeof(value)));
        if (size == sizeof(value) && blob.saved.len == sizeof(value)) {
            if (!cJSON_AddNumberToObject(values, keys[i], value)) { ok = false; break; }
        }
    }
    const char *tag = fdb_kv_get(&legacy, "ui.watchface.tag");
    if (tag && !cJSON_AddStringToObject(values, "ui.watchface.tag", tag)) ok = false;
    fdb_kvdb_deinit(&legacy);
    return ok;
#else
    return true;
#endif
}

void helios_prefs_retry_save(void)
{
    if (!settings) return;
    rt_mutex_take(&settings_lock, RT_WAITING_FOREVER);
    if (dirty && helios_storage_save(HELIOS_STORE_SETTINGS, settings)) dirty = false;
    rt_mutex_release(&settings_lock);
}

bool helios_prefs_init(void)
{
    if (initialized) return settings != NULL;
    initialized = true;
    if (!helios_storage_ready() || rt_mutex_init(&settings_lock, "hprefs", RT_IPC_FLAG_PRIO) != RT_EOK)
        return false;
    bool exists;
    settings = helios_storage_load(HELIOS_STORE_SETTINGS, &exists);
    if (settings) {
        values = cJSON_GetObjectItemCaseSensitive(settings, "values");
        if (!cJSON_IsObject(values)) {
            LOG_E("settings.json has invalid values; preserving file");
            helios_storage_preserve_invalid(HELIOS_STORE_SETTINGS);
            cJSON_Delete(settings); settings = values = NULL; return false;
        }
    } else if (exists) return false;
    else {
        settings = helios_storage_document();
        values = cJSON_AddObjectToObject(settings, "values");
        if (!settings || !values) { cJSON_Delete(settings); settings = values = NULL; return false; }
        if (!migrate_legacy()) {
            LOG_E("preference migration failed; preserving legacy data for retry on reboot");
            cJSON_Delete(settings); settings = values = NULL; return false;
        }
        dirty = true;
    }
    helios_prefs_retry_save();
    return true;
}

bool helios_prefs_ready(void) { return settings != NULL; }

bool helios_prefs_get_i32(const char *key, int32_t *value, int32_t fallback)
{
    if (value) *value = fallback;
    if (!settings || !key || !value) return false;
    rt_mutex_take(&settings_lock, RT_WAITING_FOREVER);
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(values, key);
    bool ok = cJSON_IsNumber(v) && v->valuedouble >= INT32_MIN && v->valuedouble <= INT32_MAX &&
              (double)(int32_t)v->valuedouble == v->valuedouble;
    if (ok) *value = (int32_t)v->valuedouble;
    rt_mutex_release(&settings_lock);
    return ok;
}

static bool set_value(const char *key, cJSON *value)
{
    if (!settings || !key || !value) { cJSON_Delete(value); return false; }
    rt_mutex_take(&settings_lock, RT_WAITING_FOREVER);
    cJSON *previous = cJSON_GetObjectItemCaseSensitive(values, key);
    bool ok = true;
    if (previous && cJSON_Compare(previous, value, true)) cJSON_Delete(value);
    else {
        ok = previous ? cJSON_ReplaceItemInObjectCaseSensitive(values, key, value) : cJSON_AddItemToObject(values, key, value);
        if (!ok) cJSON_Delete(value);
        else dirty = true;
    }
    rt_mutex_release(&settings_lock);
    helios_prefs_retry_save();
    return ok;
}
bool helios_prefs_set_i32(const char *key, int32_t value) { return set_value(key, cJSON_CreateNumber(value)); }
bool helios_prefs_get_u32(const char *key, uint32_t *value, uint32_t fallback)
{
    if (value) *value = fallback;
    if (!settings || !key || !value) return false;
    rt_mutex_take(&settings_lock, RT_WAITING_FOREVER);
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(values, key);
    bool ok = cJSON_IsNumber(v) && v->valuedouble >= 0 && v->valuedouble <= UINT32_MAX &&
              (double)(uint32_t)v->valuedouble == v->valuedouble;
    if (ok) *value = (uint32_t)v->valuedouble;
    rt_mutex_release(&settings_lock);
    return ok;
}
bool helios_prefs_set_u32(const char *key, uint32_t value) { return set_value(key, cJSON_CreateNumber(value)); }
bool helios_prefs_get_bool(const char *key, bool *value, bool fallback)
{
    int32_t stored = fallback;
    bool ok = helios_prefs_get_i32(key, &stored, fallback);
    if (value) *value = stored != 0;
    return ok;
}
bool helios_prefs_set_bool(const char *key, bool value) { return helios_prefs_set_i32(key, value ? 1 : 0); }
bool helios_prefs_get_str(const char *key, char *buffer, size_t length, const char *fallback)
{
    if (!buffer || !length) return false;
    rt_strncpy(buffer, fallback ? fallback : "", length - 1); buffer[length - 1] = 0;
    if (!settings || !key) return false;
    rt_mutex_take(&settings_lock, RT_WAITING_FOREVER);
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(values, key);
    bool ok = cJSON_IsString(v);
    if (ok) { rt_strncpy(buffer, v->valuestring, length - 1); buffer[length - 1] = 0; }
    rt_mutex_release(&settings_lock);
    return ok;
}
bool helios_prefs_set_str(const char *key, const char *value)
{ return value && strlen(value) <= 512 ? set_value(key, cJSON_CreateString(value)) : false; }
bool helios_prefs_del(const char *key)
{
    if (!settings || !key) return false;
    rt_mutex_take(&settings_lock, RT_WAITING_FOREVER);
    if (cJSON_HasObjectItem(values, key)) { cJSON_DeleteItemFromObjectCaseSensitive(values, key); dirty = true; }
    rt_mutex_release(&settings_lock);
    helios_prefs_retry_save();
    return true;
}
