#include "helios_prefs.h"

#include <rtthread.h>
#include <string.h>

#ifdef PKG_USING_FLASHDB
#include <flashdb.h>
#endif

#define LOG_TAG "helios.prefs"
#include "log.h"

#ifndef HELIOS_PREFS_FDB_PARTITION
#define HELIOS_PREFS_FDB_PARTITION "dfu"
#endif

#define HELIOS_PREFS_DB_NAME "helios"

#ifdef PKG_USING_FLASHDB
static struct fdb_kvdb g_prefs_db;
static struct rt_mutex g_prefs_mutex;
static bool g_prefs_mutex_ready;
#endif

static bool g_prefs_ready;

#ifdef PKG_USING_FLASHDB
static void prefs_lock(fdb_db_t db)
{
    (void)db;
    if (g_prefs_mutex_ready)
        rt_mutex_take(&g_prefs_mutex, RT_WAITING_FOREVER);
}

static void prefs_unlock(fdb_db_t db)
{
    (void)db;
    if (g_prefs_mutex_ready)
        rt_mutex_release(&g_prefs_mutex);
}
#endif

bool helios_prefs_init(void)
{
    if (g_prefs_ready)
        return true;

#ifndef PKG_USING_FLASHDB
    LOG_W("FlashDB is not enabled");
    return false;
#else
    struct fdb_default_kv default_kv = {
        .kvs = NULL,
        .num = 0,
    };

    if (!g_prefs_mutex_ready) {
        if (rt_mutex_init(&g_prefs_mutex, "hprefs", RT_IPC_FLAG_FIFO) != RT_EOK) {
            LOG_E("mutex init failed");
            return false;
        }
        g_prefs_mutex_ready = true;
    }

    memset(&g_prefs_db, 0, sizeof(g_prefs_db));
    fdb_kvdb_control(&g_prefs_db, FDB_KVDB_CTRL_SET_LOCK, prefs_lock);
    fdb_kvdb_control(&g_prefs_db, FDB_KVDB_CTRL_SET_UNLOCK, prefs_unlock);

    fdb_err_t err = fdb_kvdb_init(&g_prefs_db,
                                  HELIOS_PREFS_DB_NAME,
                                  HELIOS_PREFS_FDB_PARTITION,
                                  &default_kv,
                                  NULL);
    if (err != FDB_NO_ERR) {
        LOG_E("init failed partition=%s err=%d", HELIOS_PREFS_FDB_PARTITION, err);
        return false;
    }

    g_prefs_ready = true;
    LOG_I("ready partition=%s", HELIOS_PREFS_FDB_PARTITION);
    return true;
#endif
}

bool helios_prefs_ready(void)
{
    return g_prefs_ready;
}

bool helios_prefs_get_i32(const char *key, int32_t *value, int32_t fallback)
{
    if (value)
        *value = fallback;

#ifndef PKG_USING_FLASHDB
    return false;
#else
    if (!key || !value || !g_prefs_ready)
        return false;

    struct fdb_blob blob;
    int32_t stored = fallback;
    size_t read = fdb_kv_get_blob(&g_prefs_db,
                                  key,
                                  fdb_blob_make(&blob, &stored, sizeof(stored)));
    if (read != sizeof(stored) || blob.saved.len != sizeof(stored))
        return false;

    *value = stored;
    return true;
#endif
}

bool helios_prefs_set_i32(const char *key, int32_t value)
{
#ifndef PKG_USING_FLASHDB
    return false;
#else
    if (!key || !g_prefs_ready)
        return false;

    struct fdb_blob blob;
    return fdb_kv_set_blob(&g_prefs_db,
                           key,
                           fdb_blob_make(&blob, &value, sizeof(value))) == FDB_NO_ERR;
#endif
}

bool helios_prefs_get_u32(const char *key, uint32_t *value, uint32_t fallback)
{
    int32_t stored = (int32_t)fallback;
    bool ok = helios_prefs_get_i32(key, &stored, (int32_t)fallback);
    if (value)
        *value = (uint32_t)stored;
    return ok;
}

bool helios_prefs_set_u32(const char *key, uint32_t value)
{
    return helios_prefs_set_i32(key, (int32_t)value);
}

bool helios_prefs_get_bool(const char *key, bool *value, bool fallback)
{
    int32_t stored = fallback ? 1 : 0;
    bool ok = helios_prefs_get_i32(key, &stored, stored);
    if (value)
        *value = stored != 0;
    return ok;
}

bool helios_prefs_set_bool(const char *key, bool value)
{
    return helios_prefs_set_i32(key, value ? 1 : 0);
}

bool helios_prefs_get_str(const char *key, char *buf, size_t len, const char *fallback)
{
    if (buf && len > 0) {
        const char *src = fallback ? fallback : "";
        rt_strncpy(buf, src, len - 1);
        buf[len - 1] = '\0';
    }

#ifndef PKG_USING_FLASHDB
    return false;
#else
    if (!key || !buf || len == 0 || !g_prefs_ready)
        return false;

    char *stored = fdb_kv_get(&g_prefs_db, key);
    if (!stored)
        return false;

    rt_strncpy(buf, stored, len - 1);
    buf[len - 1] = '\0';
    return true;
#endif
}

bool helios_prefs_set_str(const char *key, const char *value)
{
#ifndef PKG_USING_FLASHDB
    return false;
#else
    if (!key || !value || !g_prefs_ready)
        return false;

    return fdb_kv_set(&g_prefs_db, key, value) == FDB_NO_ERR;
#endif
}

bool helios_prefs_del(const char *key)
{
#ifndef PKG_USING_FLASHDB
    return false;
#else
    if (!key || !g_prefs_ready)
        return false;

    return fdb_kv_del(&g_prefs_db, key) == FDB_NO_ERR;
#endif
}
