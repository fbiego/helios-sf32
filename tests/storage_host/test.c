#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
static int fail_write, fail_sync, fail_rename, partial_write;
static time_t clock_now;
static ssize_t test_write(int fd, const void *p, size_t n) {
    if (fail_write) { errno = ENOSPC; return -1; }
    return write(fd, p, partial_write && n > 3 ? 3 : n);
}
static int test_fsync(int fd) { return fail_sync ? -1 : fsync(fd); }
static int test_rename(const char *a, const char *b) { return fail_rename ? -1 : rename(a, b); }
static time_t test_time(time_t *p) { if (p) *p = clock_now; return clock_now; }
#define write test_write
#define fsync test_fsync
#define rename test_rename
#define time test_time
#include "core/helios_storage.c"
#undef LOG_TAG
#include "core/helios_prefs.c"
#undef write
#undef fsync
#undef rename
#undef time

static int mounted, blank = 1, formatted, imports, migration_error;
int rt_mutex_init(struct rt_mutex *m, const char *n, int f) { (void)m;(void)n;(void)f; return 0; }
int rt_mutex_take(struct rt_mutex *m, int t) { (void)m;(void)t; return 0; }
int rt_mutex_release(struct rt_mutex *m) { (void)m; return 0; }
int rt_sem_init(struct rt_semaphore *s, const char *n, unsigned v, int f) { (void)s;(void)n;(void)v;(void)f; return 0; }
int rt_sem_take(struct rt_semaphore *s, int t) { (void)s;(void)t; return -1; }
int rt_sem_release(struct rt_semaphore *s) { (void)s; return 0; }
int rt_tick_from_millisecond(int t) { return t; }
void rt_thread_mdelay(int ms) { (void)ms; }
rt_thread_t rt_thread_create(const char *n, void (*fn)(void *), void *p, unsigned s, unsigned pr, unsigned t) {
    (void)n;(void)fn;(void)p;(void)s;(void)pr;(void)t; return (void *)1;
}
int rt_thread_startup(rt_thread_t t) { (void)t; return 0; }
int rt_thread_delete(rt_thread_t t) { (void)t; return 0; }
rt_device_t rt_device_find(const char *n) { assert(!strcmp(n,"heliosfs")); return (void *)1; }
void register_mtd_device(uint32_t a, uint32_t s, char *n) { assert(a == FS_REGION_START_ADDR && s == FS_REGION_SIZE && !strcmp(n,"heliosfs")); }
rt_size_t rt_mtd_nor_read(struct rt_mtd_nor_device *d, rt_off_t o, rt_uint8_t *p, rt_uint32_t n) {
    (void)d; assert(o >= 0 && o+n <= FS_REGION_SIZE); memset(p, blank ? 0xff : 0, n); if(blank==2 && o+n==FS_REGION_SIZE) p[n-1]=0; return n;
}
int dfs_mount(const char *d, const char *p, const char *f, unsigned flags, const void *data) {
    assert(!strcmp(d,"heliosfs") && !strcmp(p,"/") && !strcmp(f,"lfs")); (void)flags;(void)data; return mounted ? 0 : -1;
}
int dfs_mkfs(const char *f, const char *d) { assert(!strcmp(f,"lfs") && !strcmp(d,"heliosfs")); formatted++; mounted = 1; return 0; }
int fdb_kvdb_init(struct fdb_kvdb *d, const char *n, const char *p, struct fdb_default_kv *v, void *u) {
    (void)d;(void)v;(void)u; assert(!strcmp(n,"helios") && !strcmp(p,"dfu")); imports++; return migration_error;
}
int fdb_kvdb_deinit(struct fdb_kvdb *d) { (void)d; return 0; }
struct fdb_blob *fdb_blob_make(struct fdb_blob *b, void *p, size_t s) { b->buffer=p;b->size=s;b->saved.len=0;return b; }
size_t fdb_kv_get_blob(struct fdb_kvdb *d, const char *k, struct fdb_blob *b) {
    (void)d; if (strcmp(k,"screen.brightness")) return 0;
    *(int32_t *)b->buffer=37;b->saved.len=sizeof(int32_t);return sizeof(int32_t);
}
char *fdb_kv_get(struct fdb_kvdb *d, const char *k) { (void)d; return !strcmp(k,"ui.watchface.tag") ? "test-face" : NULL; }

static cJSON *document(int value) { cJSON *d=helios_storage_document(); assert(d && cJSON_AddNumberToObject(d,"test",value)); return d; }
static void check_file(const char *path, int value) {
    bool exists; cJSON *d=read_json(path,&exists); assert(exists && d && number(d,"test")== (unsigned)value);cJSON_Delete(d);
}
static time_t when(int day, int hour, int minute) {
    struct tm t={0};t.tm_year=126;t.tm_mon=10;t.tm_mday=day;t.tm_hour=hour;t.tm_min=minute;t.tm_isdst=-1;return mktime(&t);
}
static void *no_memory(size_t size) { (void)size; return NULL; }

int main(void) {
    blank=0;assert(storage_init()!=0 && !ready && !formatted);
    blank=2;assert(storage_init()!=0 && !formatted);
    blank=1;assert(storage_init()==0 && ready && formatted==1);
    assert(errno_is(EEXIST)==false); errno=-ENOENT; assert(errno_is(ENOENT));errno=ENOENT;assert(errno_is(ENOENT));
    bool exists; assert(!helios_storage_load(HELIOS_STORE_WEATHER,&exists) && !exists);
    cJSON *d=document(1);assert(helios_storage_save(HELIOS_STORE_WEATHER,d));cJSON_Delete(d);
    char path[128];path_for(path,sizeof(path),HELIOS_STORE_WEATHER);assert(access(path,F_OK));
    d=document(2);assert(helios_storage_save(HELIOS_STORE_WEATHER,d));cJSON_Delete(d);
    partial_write=1;assert(flush_pending());partial_write=0;check_file(path,2);
    for (int failure=0;failure<3;failure++) {
        d=document(3);assert(helios_storage_save(HELIOS_STORE_WEATHER,d));cJSON_Delete(d);
        fail_write=failure==0;fail_sync=failure==1;fail_rename=failure==2;
        assert(!helios_storage_flush() && pending[HELIOS_STORE_WEATHER]);check_file(path,2);
        fail_write=fail_sync=fail_rename=0;
    }
    assert(helios_storage_flush());check_file(path,3);
    d=document(4);cJSON_Hooks hooks={no_memory,free};cJSON_InitHooks(&hooks);
    assert(!helios_storage_save(HELIOS_STORE_WEATHER,d));cJSON_InitHooks(NULL);cJSON_Delete(d);check_file(path,3);
    FILE *f=fopen(path,"w");fputs("{bad",f);fclose(f);
    assert(!helios_storage_load(HELIOS_STORE_WEATHER,&exists) && exists && blocked[HELIOS_STORE_WEATHER]);
    d=document(9);assert(!helios_storage_save(HELIOS_STORE_WEATHER,d));cJSON_Delete(d);

    migration_error=-1;assert(!helios_prefs_init() && !settings && !pending[HELIOS_STORE_SETTINGS]);
    migration_error=0;initialized=false;
    assert(helios_prefs_init() && imports==2);
    int32_t value;assert(helios_prefs_get_i32("screen.brightness",&value,0) && value==37);
    char tag[32];assert(helios_prefs_get_str("ui.watchface.tag",tag,sizeof(tag),"") && !strcmp(tag,"test-face"));
    assert(helios_prefs_set_i32("screen.brightness",68));assert(helios_prefs_set_u32("max",UINT32_MAX));
    assert(flush_pending());cJSON_Delete(settings);settings=values=NULL;initialized=false;
    assert(helios_prefs_init() && imports==2 && helios_prefs_get_i32("screen.brightness",&value,0) && value==68);
    uint32_t uv;assert(helios_prefs_get_u32("max",&uv,0) && uv==UINT32_MAX);
    assert(helios_prefs_set_bool("b",true));bool bv;assert(helios_prefs_get_bool("b",&bv,false) && bv);
    assert(helios_prefs_del("b") && !helios_prefs_get_bool("b",&bv,false));

    clock_now=when(1,12,0);helios_storage_record_battery(clock_now,3900,66,0);helios_storage_record_steps(clock_now,100);history_process();
    assert(day_total==100 && hours[12].samples==1 && hours[12].sum==3900);
    clock_now+=300;helios_storage_record_battery(clock_now,3800,55,1);helios_storage_record_steps(clock_now,110);history_process();
    assert(day_total==110 && !history_dirty && hours[12].sum==7700);
    memset(hours,0,sizeof(hours));history_date[0]=0;history_dirty=false;raw_seen=false;previous_raw=0;
    clock_now+=60;helios_storage_record_steps(clock_now,5);history_process();
    assert(day_total==115 && hours[12].steps==115 && hours[12].sum==7700 && hours[12].samples==2);
    /* Restart flush commits history even inside its five-minute checkpoint. */
    assert(history_dirty);
    assert(helios_storage_flush() && !history_dirty);
    clock_now=when(1,13,0);helios_storage_record_steps(clock_now,15);history_process();
    assert(day_total==125 && hours[13].steps==10);
    clock_now=when(2,0,0);helios_storage_record_steps(clock_now,18);history_process();
    assert(day_total==3 && hours[0].steps==3 && !strcmp(history_date,"2026-11-02"));
    clock_now+=1;helios_storage_record_steps(clock_now,2);history_process();assert(day_total==5);
    assert(history_save());
    char calories_path[160];snprintf(calories_path,sizeof(calories_path),HELIOS_STORAGE_ROOT "/steps/2026-11-02.json");
    bool calories_exist; cJSON *calories_doc=read_json(calories_path,&calories_exist);
    assert(calories_exist && number(calories_doc,"daily_calories")==helios_user_calories(day_total));
    cJSON *calorie_row=cJSON_GetArrayItem(cJSON_GetObjectItem(calories_doc,"hours"),0);
    assert(number(calorie_row,"calories")==helios_user_calories(hours[0].steps));cJSON_Delete(calories_doc);
    cJSON_InitHooks(&hooks);history_dirty=true;assert(!history_save());cJSON_InitHooks(NULL);
    char old_path[160], keep_path[160], other_path[160];
    snprintf(old_path,sizeof(old_path),HELIOS_STORAGE_ROOT "/battery/2026-10-03.json");
    snprintf(keep_path,sizeof(keep_path),HELIOS_STORAGE_ROOT "/battery/2026-10-04.json");
    snprintf(other_path,sizeof(other_path),HELIOS_STORAGE_ROOT "/battery/notes.json");
    assert(write_json_text(old_path,"{}") && write_json_text(keep_path,"{}") && write_json_text(other_path,"{}"));
    history_prune(when(2,12,0));
    assert(access(old_path,F_OK) && !access(keep_path,F_OK) && !access(other_path,F_OK));
    char step_path[160];snprintf(step_path,sizeof(step_path),HELIOS_STORAGE_ROOT "/steps/2026-11-02.json");
    cJSON *bad_history=read_json(step_path,&exists);assert(bad_history && history_valid(bad_history,1));
    cJSON *first=cJSON_GetArrayItem(cJSON_GetObjectItem(bad_history,"hours"),0);
    cJSON_ReplaceItemInObject(first,"steps",cJSON_CreateString("bad"));
    assert(write_json(step_path,bad_history));history_load();assert(steps_blocked && !day_total);
    assert(history_save());cJSON *kept=read_json(step_path,&exists);
    assert(cJSON_Compare(kept,bad_history,true));cJSON_Delete(kept);cJSON_Delete(bad_history);
    puts("storage: blank-only format, batched/retry/atomic writes, invalid/OOM protection, migration/reboot, battery aggregation, daily/hourly steps: passed");
    return 0;
}
