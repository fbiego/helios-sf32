#include "helios_storage.h"
#include "helios_battery.h"
#include "helios_user.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dfs_posix.h>
#include <dfs_fs.h>
#include <drivers/mtd_nor.h>
#include "drv_flash.h"
#include "ptab.h"
#define LOG_TAG "helios.storage"
#include "log.h"

#ifndef HELIOS_STORAGE_ROOT
#define HELIOS_STORAGE_ROOT "/data"
#endif
#define STORE_LIMIT (64 * 1024)
#define HISTORY_DAYS 30
#define CHECKPOINT_SECONDS 300
static const char *const names[] = {"settings", "weather", "notifications", "contacts", "qr", "user", "alarms"};
static struct rt_mutex lock;
static struct rt_mutex io_lock;
static struct rt_semaphore changed;
static bool ready, blocked[HELIOS_STORE_COUNT];
static char *pending[HELIOS_STORE_COUNT];
static uint32_t write_errors;
static uint32_t daily_steps;
static uint32_t hourly_steps[HELIOS_STEPS_HOURS_PER_DAY];
static char hourly_steps_date[16];
static helios_steps_record_cb_t requested_steps_cb;
static void *requested_steps_user;
static bool steps_request_pending, steps_export_busy;
static struct {
    time_t battery_time, steps_time;
    uint32_t mv, raw_steps;
    uint8_t percent, charging;
    bool battery_new, steps_new;
} incoming;

static void take(void) { rt_mutex_take(&lock, RT_WAITING_FOREVER); }
static void give(void) { rt_mutex_release(&lock); }
/* SiFli's DFS reports negative errno; host POSIX reports positive errno. */
static bool errno_is(int code) { return errno == code || errno == -code; }
static void path_for(char *path, size_t size, helios_store_id_t id)
{ snprintf(path, size, HELIOS_STORAGE_ROOT "/%s.json", names[id]); }

cJSON *helios_storage_document(void)
{
    cJSON *doc = cJSON_CreateObject();
    if (doc && !cJSON_AddNumberToObject(doc, "version", 1)) {
        cJSON_Delete(doc); return NULL;
    }
    return doc;
}

static cJSON *read_json(const char *path, bool *exists)
{
    *exists = false;
    int fd = open(path, O_RDONLY);
    if (fd < 0) { *exists = !errno_is(ENOENT); return NULL; }
    *exists = true;
    struct stat info;
    if (fstat(fd, &info) || info.st_size <= 0 || info.st_size > STORE_LIMIT) {
        close(fd); return NULL;
    }
    size_t size = (size_t)info.st_size, done = 0;
    char *text = malloc(size + 1);
    if (!text) { close(fd); return NULL; }
    while (done < size) {
        int n = read(fd, text + done, size - done);
        if (n <= 0) break;
        done += (size_t)n;
    }
    close(fd);
    text[done] = 0;
    cJSON *doc = done == size ? cJSON_ParseWithOpts(text, NULL, true) : NULL;
    free(text);
    cJSON *version = cJSON_GetObjectItemCaseSensitive(doc, "version");
    if (!cJSON_IsObject(doc) || !cJSON_IsNumber(version) || version->valuedouble != 1) {
        cJSON_Delete(doc); return NULL;
    }
    return doc;
}

/* LittleFS implements replacement rename atomically. Keep the previous file
 * until the new file has been fully written, synced and closed successfully. */
static bool write_json_text(const char *path, const char *text)
{
    char temp[160];
    if (snprintf(temp, sizeof(temp), "%s.tmp", path) >= (int)sizeof(temp)) return false;
    int fd = open(temp, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) { write_errors++; return false; }
    size_t size = strlen(text), done = 0;
    bool ok = true;
    while (done < size) {
        int n = write(fd, text + done, size - done);
        if (n <= 0) { ok = false; break; }
        done += (size_t)n;
    }
    if (ok && fsync(fd)) ok = false;
    if (close(fd)) ok = false;
    if (ok && rename(temp, path)) ok = false;
    if (!ok) { unlink(temp); write_errors++; }
    return ok;
}

static bool write_json(const char *path, const cJSON *doc)
{
    char *text = cJSON_PrintUnformatted(doc);
    if (!text) return false;
    bool ok = strlen(text) <= STORE_LIMIT && write_json_text(path, text);
    cJSON_free(text);
    return ok;
}

bool helios_storage_ready(void) { return ready; }
void helios_storage_preserve_invalid(helios_store_id_t id)
{ if ((unsigned)id < HELIOS_STORE_COUNT) blocked[id] = true; }

cJSON *helios_storage_load(helios_store_id_t id, bool *exists)
{
    bool found = false;
    if (exists) *exists = false;
    if (!ready || (unsigned)id >= HELIOS_STORE_COUNT) return NULL;
    char path[96]; path_for(path, sizeof(path), id);
    cJSON *doc = read_json(path, &found);
    if (found && !doc) {
        blocked[id] = true;
        LOG_W("preserving unreadable/unsupported %s", path);
    }
    if (exists) *exists = found;
    return doc;
}

bool helios_storage_save(helios_store_id_t id, const cJSON *doc)
{
    if (!ready || (unsigned)id >= HELIOS_STORE_COUNT || blocked[id] || !doc) return false;
    char *text = cJSON_PrintUnformatted(doc);
    if (!text) return false;
    if (strlen(text) > STORE_LIMIT) { cJSON_free(text); return false; }
    take();
    cJSON_free(pending[id]);
    pending[id] = text;
    give();
    rt_sem_release(&changed);
    return true;
}

void helios_storage_record_battery(time_t now, uint32_t mv, uint8_t percent, uint8_t charging)
{
    if (!ready) return;
    take(); incoming.battery_time = now; incoming.mv = mv;
    incoming.percent = percent; incoming.charging = charging; incoming.battery_new = true; give();
}
void helios_storage_record_steps(time_t now, uint32_t raw_steps)
{
    if (!ready) return;
    take(); incoming.steps_time = now; incoming.raw_steps = raw_steps; incoming.steps_new = true; give();
}
uint32_t helios_storage_daily_steps(void)
{ if (!ready) return 0; take(); uint32_t value = daily_steps; give(); return value; }

void helios_storage_hourly_steps(time_t now, uint32_t bins[HELIOS_STEPS_HOURS_PER_DAY])
{
    memset(bins, 0, sizeof(hourly_steps));
    struct tm calendar;
    char date[sizeof(hourly_steps_date)];
    if (!ready || !localtime_r(&now, &calendar) || !strftime(date, sizeof(date), "%Y-%m-%d", &calendar))
        return;

    take();
    if (!strcmp(date, hourly_steps_date))
        memcpy(bins, hourly_steps, sizeof(hourly_steps));
    give();
}

void helios_storage_request_steps(helios_steps_record_cb_t callback, void *user)
{
    if (!ready) return;
    take();
    requested_steps_cb = callback;
    requested_steps_user = user;
    steps_request_pending = true;
    steps_export_busy = true;
    give();
    rt_sem_release(&changed);
}

/* History is owned by the storage worker. One file per day, 24 hourly slots.
 * Persist sum/count as well as average so a reboot can resume aggregation. */
typedef struct {
    uint64_t sum;
    uint32_t samples, min_mv, max_mv, last_mv, percent, charging;
    uint32_t steps;
} hour_t;
static hour_t hours[HELIOS_STEPS_HOURS_PER_DAY];
static char history_date[16];
static time_t history_start, checkpoint;
static uint32_t day_total, previous_raw;
static int history_hour = -1;
static bool raw_seen, history_dirty, battery_blocked, steps_blocked;

static uint32_t number(const cJSON *doc, const char *key)
{
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(doc, key);
    return cJSON_IsNumber(v) && v->valuedouble >= 0 && v->valuedouble <= UINT32_MAX ? (uint32_t)v->valuedouble : 0;
}

static bool valid_uint(const cJSON *doc, const char *key, uint32_t max)
{
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(doc, key);
    return cJSON_IsNumber(v) && v->valuedouble >= 0 && v->valuedouble <= max &&
           (double)(uint32_t)v->valuedouble == v->valuedouble;
}

static bool history_valid_for_date(const cJSON *doc, int kind, const char *expected_date)
{
    const cJSON *date = cJSON_GetObjectItemCaseSensitive(doc, "date");
    const cJSON *array = cJSON_GetObjectItemCaseSensitive(doc, "hours"), *row;
    if (!cJSON_IsString(date) || strcmp(date->valuestring, expected_date) ||
        !cJSON_IsArray(array) || cJSON_GetArraySize(array) != 24) return false;
    uint32_t seen = 0;
    uint64_t sum_steps = 0;
    cJSON_ArrayForEach(row, array) {
        if (!valid_uint(row, "hour", 23)) return false;
        uint32_t h = number(row, "hour");
        if (seen & (1UL << h)) return false;
        seen |= 1UL << h;
        if (kind) {
            if (!valid_uint(row, "steps", UINT32_MAX)) return false;
            if (cJSON_HasObjectItem(row, "calories") && !valid_uint(row, "calories", UINT32_MAX)) return false;
            sum_steps += number(row, "steps");
        } else {
            const cJSON *sum = cJSON_GetObjectItemCaseSensitive(row, "sum_mv");
            if (!valid_uint(row, "samples", UINT32_MAX) || !valid_uint(row, "percentage", 100) ||
                !valid_uint(row, "min_mv", 5000) || !valid_uint(row, "max_mv", 5000) ||
                !valid_uint(row, "last_mv", 5000) || !cJSON_IsNumber(sum) || sum->valuedouble < 0 ||
                sum->valuedouble > (double)number(row, "samples") * 5000 ||
                !cJSON_IsBool(cJSON_GetObjectItemCaseSensitive(row, "charging"))) return false;
        }
    }
    return !kind || (valid_uint(doc, "daily_steps", UINT32_MAX) && sum_steps == number(doc, "daily_steps"));
}

static bool history_valid(const cJSON *doc, int kind)
{ return history_valid_for_date(doc, kind, history_date); }

bool helios_battery_get_usage_24h(time_t now, helios_battery_usage_t *usage)
{
    if (!usage) return false;
    memset(usage, 0, sizeof(*usage));
    struct tm slot;
    if (!ready || !localtime_r(&now, &slot) || slot.tm_year < 120 || slot.tm_year > 199)
        return false;
    usage->end = now;
    usage->start = now - 86400;
    time_t cursor = usage->start;
    if (!localtime_r(&cursor, &slot)) return false;
    /* Include the partially overlapping first and current local hours. */
    cursor -= slot.tm_min * 60 + slot.tm_sec;
    char loaded_day[16] = "";
    cJSON *doc = NULL;
    rt_mutex_take(&io_lock, RT_WAITING_FOREVER);
    while (cursor <= now && usage->count < 25) {
        if (!localtime_r(&cursor, &slot)) break;
        char day[16], path[112];
        strftime(day, sizeof(day), "%Y-%m-%d", &slot);
        bool live = !strcmp(day, history_date) && !battery_blocked;
        if (strcmp(day, loaded_day)) {
            cJSON_Delete(doc); doc = NULL;
            snprintf(loaded_day, sizeof(loaded_day), "%s", day);
            if (!live) {
                bool exists;
                snprintf(path, sizeof(path), HELIOS_STORAGE_ROOT "/battery/%s.json", day);
                doc = read_json(path, &exists);
                if (doc && !history_valid_for_date(doc, 0, day)) {
                    cJSON_Delete(doc); doc = NULL;
                }
            }
        }
        helios_battery_hour_t *out = &usage->hours[usage->count++];
        out->timestamp = cursor;
        if (live) {
            const hour_t *h = &hours[slot.tm_hour];
            out->samples = h->samples;
            out->average_mv = h->samples ? h->sum / h->samples : 0;
            out->min_mv = h->min_mv; out->max_mv = h->max_mv;
            out->last_mv = h->last_mv; out->charging = h->charging;
        } else if (doc) {
            const cJSON *row;
            cJSON_ArrayForEach(row, cJSON_GetObjectItemCaseSensitive(doc, "hours")) {
                if (number(row, "hour") != (unsigned)slot.tm_hour) continue;
                out->samples = number(row, "samples");
                const cJSON *sum = cJSON_GetObjectItemCaseSensitive(row, "sum_mv");
                out->average_mv = out->samples ? (uint64_t)sum->valuedouble / out->samples : 0;
                out->min_mv = number(row, "min_mv"); out->max_mv = number(row, "max_mv");
                out->last_mv = number(row, "last_mv");
                out->charging = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(row, "charging"));
                break;
            }
        }
        /* Old history retains voltage, so apply today's calibration consistently. */
        if (out->samples) out->percent = helios_battery_percent_from_mv(out->last_mv);
        cursor += 3600;
    }
    cJSON_Delete(doc);
    rt_mutex_release(&io_lock);
    return true;
}

static bool history_save(void)
{
    bool ok = true;
    for (int kind = 0; kind < 2; kind++) {
        if (kind ? steps_blocked : battery_blocked) continue;
        cJSON *doc = helios_storage_document();
        if (!doc) return false;
        if (!cJSON_AddStringToObject(doc, "date", history_date)) goto history_failed;
        if (kind && !cJSON_AddNumberToObject(doc, "daily_steps", day_total)) goto history_failed;
        if (kind && !cJSON_AddNumberToObject(doc, "daily_calories", helios_user_calories(day_total))) goto history_failed;
        cJSON *array = cJSON_AddArrayToObject(doc, "hours");
        if (!array) { cJSON_Delete(doc); return false; }
        for (int i = 0; i < 24; i++) {
            cJSON *row = cJSON_CreateObject();
            if (!row) { cJSON_Delete(doc); return false; }
            if (!cJSON_AddItemToArray(array, row)) { cJSON_Delete(row); goto history_failed; }
            if (!cJSON_AddNumberToObject(row, "hour", i)) goto history_failed;
            struct tm slot;
            if (!localtime_r(&history_start, &slot)) goto history_failed;
            slot.tm_hour = i; slot.tm_isdst = -1;
            if (!cJSON_AddNumberToObject(row, "timestamp", (double)mktime(&slot))) goto history_failed;
            if (kind) {
                if (!cJSON_AddNumberToObject(row, "steps", hours[i].steps)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "calories", helios_user_calories(hours[i].steps))) goto history_failed;
            } else {
                if (!cJSON_AddNumberToObject(row, "samples", hours[i].samples)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "sum_mv", (double)hours[i].sum)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "millivolts", hours[i].samples ? hours[i].sum / hours[i].samples : 0)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "min_mv", hours[i].min_mv)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "max_mv", hours[i].max_mv)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "last_mv", hours[i].last_mv)) goto history_failed;
                if (!cJSON_AddNumberToObject(row, "percentage", hours[i].percent)) goto history_failed;
                if (!cJSON_AddBoolToObject(row, "charging", hours[i].charging)) goto history_failed;
            }
        }
        char path[112];
        snprintf(path, sizeof(path), HELIOS_STORAGE_ROOT "/%s/%s.json", kind ? "steps" : "battery", history_date);
        if (!write_json(path, doc)) ok = false;
        cJSON_Delete(doc);
        continue;
history_failed:
        cJSON_Delete(doc);
        return false;
    }
    if (ok) history_dirty = false;
    return ok;
}

static void history_load(void)
{
    memset(hours, 0, sizeof(hours)); day_total = 0;
    battery_blocked = steps_blocked = false;
    for (int kind = 0; kind < 2; kind++) {
        char path[112]; bool exists;
        snprintf(path, sizeof(path), HELIOS_STORAGE_ROOT "/%s/%s.json", kind ? "steps" : "battery", history_date);
        cJSON *doc = read_json(path, &exists);
        if (doc && !history_valid(doc, kind)) { cJSON_Delete(doc); doc = NULL; }
        if (exists && !doc) {
            if (kind) steps_blocked = true; else battery_blocked = true;
            LOG_W("preserving invalid history %s", path);
        }
        if (kind) day_total = number(doc, "daily_steps");
        const cJSON *array = cJSON_GetObjectItemCaseSensitive(doc, "hours"), *row;
        cJSON_ArrayForEach(row, array) {
            unsigned h = number(row, "hour");
            if (h >= 24) continue;
            if (kind) hours[h].steps = number(row, "steps");
            else {
                hours[h].samples = number(row, "samples");
                const cJSON *sum = cJSON_GetObjectItemCaseSensitive(row, "sum_mv");
                if (cJSON_IsNumber(sum) && sum->valuedouble >= 0 && sum->valuedouble <= 1e12)
                    hours[h].sum = (uint64_t)sum->valuedouble;
                hours[h].min_mv = number(row, "min_mv"); hours[h].max_mv = number(row, "max_mv");
                hours[h].last_mv = number(row, "last_mv"); hours[h].percent = number(row, "percentage");
                hours[h].charging = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(row, "charging"));
            }
        }
        cJSON_Delete(doc);
    }
    take();
    daily_steps = day_total;
    memcpy(hourly_steps_date, history_date, sizeof(hourly_steps_date));
    for (unsigned i = 0; i < HELIOS_STEPS_HOURS_PER_DAY; ++i)
        hourly_steps[i] = hours[i].steps;
    give();
}

static void history_prune(time_t now)
{
    struct tm tm;
    if (!localtime_r(&now, &tm)) return;
    tm.tm_mday -= HISTORY_DAYS - 1; tm.tm_hour = tm.tm_min = tm.tm_sec = 0; tm.tm_isdst = -1;
    if (mktime(&tm) == (time_t)-1) return;
    char oldest[16]; strftime(oldest, sizeof(oldest), "%Y-%m-%d", &tm);
    const char *dirs[] = {HELIOS_STORAGE_ROOT "/battery", HELIOS_STORAGE_ROOT "/steps"};
    for (int k = 0; k < 2; k++) {
        DIR *dir = opendir(dirs[k]); if (!dir) continue;
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            const char *n = entry->d_name;
            if (strlen(n) != 15 || n[4] != '-' || n[7] != '-' || strcmp(n + 10, ".json")) continue;
            bool digits = true;
            for (int i = 0; i < 10; i++) if (i != 4 && i != 7 && (n[i] < '0' || n[i] > '9')) digits = false;
            if (digits && strncmp(n, oldest, 10) < 0) {
                char path[128]; snprintf(path, sizeof(path), "%s/%s", dirs[k], n); unlink(path);
            }
        }
        closedir(dir);
    }
}

static int history_select(time_t now)
{
    struct tm tm;
    if (!localtime_r(&now, &tm) || tm.tm_year < 120 || tm.tm_year > 199) return -1;
    int hour = tm.tm_hour; char date[16];
    strftime(date, sizeof(date), "%Y-%m-%d", &tm);
    if (strcmp(date, history_date)) {
        if (history_dirty && !history_save()) return -1;
        snprintf(history_date, sizeof(history_date), "%s", date);
        tm.tm_hour = tm.tm_min = tm.tm_sec = 0; tm.tm_isdst = -1;
        history_start = mktime(&tm);
        history_load(); history_prune(now); checkpoint = now;
        history_hour = hour;
    } else if (history_hour != hour) {
        if (history_dirty && history_save()) checkpoint = now;
        history_hour = hour;
    }
    return hour;
}

static void history_process(void)
{
    time_t bt, st; uint32_t mv, raw; uint8_t percent, charging; bool bn, sn;
    take(); bt = incoming.battery_time; st = incoming.steps_time; mv = incoming.mv;
    raw = incoming.raw_steps; percent = incoming.percent; charging = incoming.charging;
    bn = incoming.battery_new; sn = incoming.steps_new;
    incoming.battery_new = incoming.steps_new = false; give();
    if (bn) {
        int h = history_select(bt);
        if (h >= 0 && !battery_blocked) {
            hour_t *v = &hours[h];
            if (!v->samples || mv < v->min_mv) v->min_mv = mv;
            if (!v->samples || mv > v->max_mv) v->max_mv = mv;
            v->sum += mv; v->samples++; v->last_mv = mv; v->percent = percent; v->charging = charging;
            history_dirty = true;
        }
    }
    if (sn) {
        int h = history_select(st);
        if (h >= 0 && !steps_blocked) {
            /* Counter starts at zero on boot. A later sensor reset must never
             * produce an unsigned underflow or erase a saved daily total. */
            uint32_t delta = raw_seen && raw >= previous_raw ? raw - previous_raw : raw;
            previous_raw = raw; raw_seen = true;
            if (UINT32_MAX - day_total < delta) delta = UINT32_MAX - day_total;
            hours[h].steps += delta; day_total += delta;
            if (delta) history_dirty = true;
            take();
            daily_steps = day_total;
            hourly_steps[h] = hours[h].steps;
            give();
        }
    }
    time_t now = time(NULL);
    if (history_dirty && (now < checkpoint || now - checkpoint >= CHECKPOINT_SECONDS)) {
        if (history_save()) checkpoint = now;
    }
}

static bool flush_pending(void)
{
    bool failed = false;
    for (int i = 0; i < HELIOS_STORE_COUNT; i++) {
        take(); char *text = pending[i]; pending[i] = NULL; give();
        if (!text) continue;
        char path[96]; path_for(path, sizeof(path), i);
        if (!write_json_text(path, text)) {
            failed = true;
            take(); if (!pending[i]) { pending[i] = text; text = NULL; } give();
        }
        cJSON_free(text);
    }
    return !failed;
}

bool helios_storage_flush(void)
{
    if (!ready) return true; /* No mounted store and no queued writes. */
    rt_mutex_take(&io_lock, RT_WAITING_FOREVER);
    bool ok = flush_pending();
    history_process();
    if (history_dirty && !history_save()) ok = false;
    rt_mutex_release(&io_lock);
    return ok;
}

/* Worker-owned export cursor. Keep only one day's small snapshot in RAM. */
static struct {
    helios_steps_record_cb_t cb;
    void *user;
    struct tm today, date;
    int day_index, hour;
    uint32_t steps[24], calories[24];
} steps_export;

/* Called with io_lock held. Never invoke BLE callbacks under that lock. */
static bool steps_export_next(helios_steps_record_t *record,
                              helios_steps_record_cb_t *callback, void **user)
{
    take();
    if (steps_request_pending) {
        memset(&steps_export, 0, sizeof(steps_export));
        steps_export.cb = requested_steps_cb;
        steps_export.user = requested_steps_user;
        steps_export.hour = 24;
        steps_request_pending = false;
        time_t now = time(NULL);
        if (!localtime_r(&now, &steps_export.today) || steps_export.today.tm_year < 120 ||
            steps_export.today.tm_year > 199) steps_export.cb = NULL;
    }
    give();

    while (steps_export.cb) {
        if (steps_export.hour >= 24) {
            if (steps_export.day_index >= HISTORY_DAYS) break;
            struct tm date = steps_export.today;
            date.tm_mday -= HISTORY_DAYS - 1 - steps_export.day_index++;
            date.tm_hour = 12; date.tm_min = date.tm_sec = 0; date.tm_isdst = -1;
            if (mktime(&date) == (time_t)-1) continue;
            steps_export.date = date;
            char day[16], path[112];
            strftime(day, sizeof(day), "%Y-%m-%d", &date);
            if (steps_export.day_index == HISTORY_DAYS && !strcmp(day, history_date) && !steps_blocked) {
                /* Include samples not yet due for a five-minute checkpoint. */
                for (int h = 0; h < 24; h++) {
                    steps_export.steps[h] = hours[h].steps;
                    steps_export.calories[h] = helios_user_calories(hours[h].steps);
                }
            } else {
                bool exists;
                snprintf(path, sizeof(path), HELIOS_STORAGE_ROOT "/steps/%s.json", day);
                cJSON *doc = read_json(path, &exists);
                if (!doc || !history_valid_for_date(doc, 1, day)) { cJSON_Delete(doc); continue; }
                const cJSON *row;
                cJSON_ArrayForEach(row, cJSON_GetObjectItemCaseSensitive(doc, "hours")) {
                    unsigned h = number(row, "hour");
                    steps_export.steps[h] = number(row, "steps");
                    steps_export.calories[h] = cJSON_HasObjectItem(row, "calories") ?
                        number(row, "calories") : helios_user_calories(steps_export.steps[h]);
                }
                cJSON_Delete(doc);
            }
            steps_export.hour = 0;
        }
        int h = steps_export.hour++;
        if (steps_export.day_index == HISTORY_DAYS && h > steps_export.today.tm_hour) break;
        *record = (helios_steps_record_t){
            .steps = steps_export.steps[h], .calories = steps_export.calories[h],
            .year = steps_export.date.tm_year + 1900, .month = steps_export.date.tm_mon + 1,
            .day = steps_export.date.tm_mday, .hour = h
        };
        *callback = steps_export.cb; *user = steps_export.user;
        return true;
    }
    steps_export.cb = NULL;
    take(); steps_export_busy = steps_request_pending; give();
    return false;
}

static void storage_worker(void *unused)
{
    (void)unused;
    while (1) {
        take(); bool exporting = steps_export_busy; give();
        if (rt_sem_take(&changed, rt_tick_from_millisecond(exporting ? 0 : 5000)) == RT_EOK) {
            take(); exporting = steps_export_busy; give();
            if (!exporting)
                rt_thread_mdelay(2000); /* Fixed batch window; continuous arrivals cannot defer forever. */
            while (rt_sem_take(&changed, 0) == RT_EOK) {}
        }
        rt_mutex_take(&io_lock, RT_WAITING_FOREVER);
        bool failed = !flush_pending();
        history_process();
        helios_steps_record_t record;
        helios_steps_record_cb_t callback;
        void *user;
        bool send_record = steps_export_next(&record, &callback, &user);
        rt_mutex_release(&io_lock);
        if (send_record) {
            if (!callback(&record, user)) {
                steps_export.cb = NULL;
                take(); steps_export_busy = steps_request_pending; give();
            }
            /* Limit notifications even when unrelated saves wake the worker. */
            rt_thread_mdelay(100);
        }
        if (failed) { LOG_W("write failed; retrying pending data"); rt_thread_mdelay(30000); }
    }
}

static bool partition_blank(struct rt_mtd_nor_device *dev)
{
    uint8_t buffer[256];
    for (uint32_t offset = 0; offset < FS_REGION_SIZE; offset += sizeof(buffer)) {
        uint32_t size = FS_REGION_SIZE - offset;
        if (size > sizeof(buffer)) size = sizeof(buffer);
        if (rt_mtd_nor_read(dev, offset, buffer, size) != size) return false;
        for (uint32_t i = 0; i < size; i++) if (buffer[i] != 0xff) return false;
    }
    return true;
}

static int storage_init(void)
{
    if (rt_mutex_init(&lock, "hstore", RT_IPC_FLAG_PRIO) != RT_EOK ||
        rt_mutex_init(&io_lock, "hstoreio", RT_IPC_FLAG_PRIO) != RT_EOK ||
        rt_sem_init(&changed, "hstore", 0, RT_IPC_FLAG_FIFO) != RT_EOK) return -RT_ERROR;
    register_mtd_device(FS_REGION_START_ADDR, FS_REGION_SIZE, "heliosfs");
    rt_device_t dev = rt_device_find("heliosfs");
    if (!dev) { LOG_E("fs_region device unavailable"); return -RT_ERROR; }
    if (dfs_mount("heliosfs", "/", "lfs", 0, NULL)) {
        if (!partition_blank((struct rt_mtd_nor_device *)dev)) {
            LOG_E("fs_region is nonblank and cannot mount; preserving existing data"); return -RT_ERROR;
        }
        if (dfs_mkfs("lfs", "heliosfs") || dfs_mount("heliosfs", "/", "lfs", 0, NULL)) return -RT_ERROR;
    }
    const char *dirs[] = {HELIOS_STORAGE_ROOT, HELIOS_STORAGE_ROOT "/battery", HELIOS_STORAGE_ROOT "/steps"};
    for (unsigned i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
        struct stat info;
        if (mkdir(dirs[i], 0700) && !errno_is(EEXIST)) return -RT_ERROR;
        if (stat(dirs[i], &info) || !S_ISDIR(info.st_mode)) return -RT_ERROR;
    }
    rt_thread_t worker = rt_thread_create("hstore", storage_worker, NULL, 6144, 24, 10);
    if (!worker) return -RT_ENOMEM;
    ready = true;
    if (rt_thread_startup(worker) != RT_EOK) { ready = false; rt_thread_delete(worker); return -RT_ERROR; }
    LOG_I("LittleFS ready at " HELIOS_STORAGE_ROOT "; history retention=%d days", HISTORY_DAYS);
    return RT_EOK;
}
INIT_ENV_EXPORT(storage_init);

#ifdef RT_USING_FINSH
static void storage_status(void)
{
    rt_kprintf("LittleFS: %s, root=" HELIOS_STORAGE_ROOT ", write errors=%lu\n",
               ready ? "ready" : "unavailable", (unsigned long)write_errors);
    if (ready) {
        take();
        for (int i = 0; i < HELIOS_STORE_COUNT; i++)
            rt_kprintf("%s.json: %s%s\n", names[i], blocked[i] ? "blocked (invalid file)" : "enabled", pending[i] ? ", pending" : "");
        give();
    }
}
MSH_CMD_EXPORT(storage_status, Show file storage and pending writes);
#endif
