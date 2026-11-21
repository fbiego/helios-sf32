#include <assert.h>
#include <string.h>
#include "core/helios_battery_ui.c"

static bool worker_context, read_ok = true, fail_worker, fail_timer;
static unsigned reads, worker_count, signals;
static uint32_t ticks;
static helios_battery_usage_t fixture;
static lv_timer_t timers[16];
static unsigned timer_count;
static lv_chart_series_t series;

int rt_mutex_init(struct rt_mutex *m, const char *n, int f)
{ (void)m; (void)n; (void)f; return RT_EOK; }
int rt_mutex_take(struct rt_mutex *m, int t) { (void)m; (void)t; return RT_EOK; }
int rt_mutex_release(struct rt_mutex *m) { (void)m; return RT_EOK; }
int rt_sem_init(struct rt_semaphore *s, const char *n, unsigned v, int f)
{ (void)s; (void)n; (void)v; (void)f; return RT_EOK; }
int rt_sem_take(struct rt_semaphore *s, int t) { (void)s; (void)t; return -1; }
int rt_sem_release(struct rt_semaphore *s) { (void)s; signals++; return RT_EOK; }
rt_thread_t rt_thread_create(const char *n, void (*fn)(void *), void *p, unsigned stack, unsigned pr, unsigned t)
{
    (void)n; (void)p; (void)pr; (void)t;
    assert(fn == history_worker && stack >= 6144);
    worker_count++;
    return fail_worker ? NULL : (void *)1;
}
int rt_thread_startup(rt_thread_t t) { (void)t; return RT_EOK; }
int rt_thread_delete(rt_thread_t t) { (void)t; return RT_EOK; }
bool helios_battery_get_usage_24h(time_t now, helios_battery_usage_t *out)
{
    (void)now; assert(worker_context); reads++; *out = fixture; return read_ok;
}
static void check_ui(lv_obj_t *obj) { assert(!worker_context && obj && !obj->deleted); }
lv_obj_t *lv_obj_find_by_name(lv_obj_t *view, const char *name)
{ check_ui(view); assert(!strcmp(name, "battery_chart")); return view->child; }
void lv_obj_add_event_cb(lv_obj_t *obj, void (*cb)(lv_event_t *), int event, void *data)
{ check_ui(obj); assert(event == LV_EVENT_DELETE && !data); obj->on_delete = cb; }
lv_obj_t *lv_event_get_target(lv_event_t *event) { return event->target; }
lv_chart_series_t *lv_chart_get_series_next(lv_obj_t *obj, lv_chart_series_t *previous)
{ check_ui(obj); assert(!previous); return &series; }
void lv_chart_set_point_count(lv_obj_t *obj, uint32_t count)
{ check_ui(obj); assert(count <= 25); obj->count = count; }
void lv_chart_set_series_values(lv_obj_t *obj, lv_chart_series_t *s, const int32_t *values, size_t count)
{ check_ui(obj); assert(s == &series && count == obj->count); memcpy(obj->values, values, count * sizeof(*values)); }
void lv_chart_refresh(lv_obj_t *obj) { check_ui(obj); obj->renders++; }
uint32_t lv_tick_get(void) { assert(!worker_context); return ticks; }
uint32_t lv_tick_elaps(uint32_t start) { return lv_tick_get() - start; }
lv_timer_t *lv_timer_create(void (*cb)(lv_timer_t *), uint32_t ms, void *data)
{
    assert(!worker_context && ms == 1000 && !data);
    if (fail_timer) return NULL;
    assert(timer_count < 16); lv_timer_t *timer = &timers[timer_count++];
    timer->callback = cb; return timer;
}
void lv_timer_delete(lv_timer_t *timer)
{ assert(!worker_context && !timer->deleted); timer->deleted = true; }
static void load(uint32_t version)
{ worker_context = true; history_load(version); worker_context = false; }
static void poll(void)
{ assert(refresh_timer && !refresh_timer->deleted); refresh_timer->callback(refresh_timer); }
static void destroy(lv_obj_t *obj)
{ lv_event_t event = {.target = obj}; obj->on_delete(&event); obj->deleted = true; }

int main(void)
{
    assert(battery_chart_init() == RT_EOK);
    lv_obj_t a = {0}, b = {0}, c = {0}, d = {0};
    lv_obj_t view = {.child = &a};
    battery_view_attach(NULL);
    fail_worker = true; battery_view_attach(&view);
    assert(!refresh_timer && a.values[0] == LV_CHART_POINT_NONE);
    destroy(&a);
    fail_worker = false; view.child = &b; battery_view_attach(&view);
    assert(worker_count == 2 && reads == 0 && signals == 1);
    unsigned created = timer_count;
    battery_view_attach(&view); assert(timer_count == created);
    uint32_t old_version = generation;
    lv_timer_t *old_timer = refresh_timer;
    destroy(&b); assert(old_timer->deleted && !chart && !refresh_timer);
    fixture.count = 25; fixture.end = time(NULL);
    fixture.hours[0].samples = 1; fixture.hours[0].percent = 0;
    fixture.hours[24].samples = 1; fixture.hours[24].percent = 72;
    view.child = &c; battery_view_attach(&view);
    unsigned renders = c.renders;
    load(old_version); poll();
    assert(c.renders == renders); /* Old completion cannot update the new view. */
    load(generation); poll();
    assert(c.count == 25 && c.values[0] == 0 && c.values[1] == LV_CHART_POINT_NONE && c.values[24] == 72);
    /* A second view uses cache immediately; deleting the previous one is harmless. */
    view.child = &d; battery_view_attach(&view);
    assert(d.values[24] == 72 && worker_count == 2);
    destroy(&c); assert(chart == &d && refresh_timer);
    ticks = 60000; poll(); assert(last_request_ms == ticks);
    read_ok = false; load(generation); poll();
    assert(d.values[24] == LV_CHART_POINT_NONE);
    destroy(&d); assert(!chart && !refresh_timer);
    lv_obj_t e = {0}; view.child = &e;
    fail_timer = true; battery_view_attach(&view); assert(!refresh_timer);
    destroy(&e);
    return 0;
}
