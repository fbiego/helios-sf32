#include "helios_battery.h"
#include "lvgl.h"
#include <string.h>

#define LOG_TAG "helios.batui"
#include "log.h"

/* Only the UI thread touches chart/timer/generation. The worker sees generation
 * numbers, never widget pointers, including while a view is being deleted. */
static lv_obj_t *chart;
static lv_timer_t *refresh_timer;
static uint32_t generation, last_request_ms;
static struct rt_mutex history_lock;
static struct rt_semaphore history_changed;
static rt_thread_t history_thread;
static bool initialized, requested, result_ready, result_ok;
static uint32_t requested_generation, result_generation;
static helios_battery_usage_t cached;

static int battery_chart_init(void)
{
    if (rt_mutex_init(&history_lock, "batgraph", RT_IPC_FLAG_PRIO) != RT_EOK ||
        rt_sem_init(&history_changed, "batgraph", 0, RT_IPC_FLAG_FIFO) != RT_EOK)
        return -RT_ERROR;
    initialized = true;
    return RT_EOK;
}
INIT_ENV_EXPORT(battery_chart_init);

static void history_load(uint32_t job_generation)
{
    helios_battery_usage_t usage;
    bool ok = helios_battery_get_usage_24h(time(NULL), &usage);
    rt_mutex_take(&history_lock, RT_WAITING_FOREVER);
    if (ok) cached = usage;
    result_generation = job_generation;
    result_ok = ok;
    result_ready = true;
    rt_mutex_release(&history_lock);
}

static void history_worker(void *unused)
{
    (void)unused;
    while (1) {
        if (rt_sem_take(&history_changed, RT_WAITING_FOREVER) != RT_EOK) continue;
        rt_mutex_take(&history_lock, RT_WAITING_FOREVER);
        bool pending = requested;
        uint32_t job_generation = requested_generation;
        requested = false;
        rt_mutex_release(&history_lock);
        if (pending) history_load(job_generation);
    }
}

static void history_request(void)
{
    rt_mutex_take(&history_lock, RT_WAITING_FOREVER);
    bool wake = !requested;
    requested = true;
    requested_generation = generation;
    rt_mutex_release(&history_lock);
    if (wake) rt_sem_release(&history_changed);
    last_request_ms = lv_tick_get();
}

static void chart_render(const helios_battery_usage_t *usage)
{
    lv_chart_series_t *series = lv_chart_get_series_next(chart, NULL);
    if (!series) return;
    unsigned count = usage && usage->count && usage->count <= 25 ? usage->count : 25;
    int32_t values[25];
    for (unsigned i = 0; i < count; i++)
        values[i] = usage && usage->count == count && usage->hours[i].samples ?
                    usage->hours[i].percent : LV_CHART_POINT_NONE;
    lv_chart_set_point_count(chart, count);
    lv_chart_set_series_values(chart, series, values, count);
    lv_chart_refresh(chart);
}

static void history_poll(lv_timer_t *timer)
{
    (void)timer;
    helios_battery_usage_t usage;
    rt_mutex_take(&history_lock, RT_WAITING_FOREVER);
    bool apply = result_ready && result_generation == generation;
    bool ok = result_ok;
    if (apply && ok) usage = cached;
    result_ready = false;
    rt_mutex_release(&history_lock);
    if (apply) chart_render(ok ? &usage : NULL);
    if (lv_tick_elaps(last_request_ms) >= 60000) history_request();
}

static void chart_deleted(lv_event_t *event)
{
    if (lv_event_get_target(event) != chart) return;
    chart = NULL;
    generation++;
    if (refresh_timer) lv_timer_delete(refresh_timer);
    refresh_timer = NULL;
}

/* Strong override of the imported UI's weak hook. UI thread only. */
void battery_view_attach(lv_obj_t *view)
{
    if (!view) return;
    lv_obj_t *next = lv_obj_find_by_name(view, "battery_chart");
    if (!next || next == chart) return;
    if (refresh_timer) lv_timer_delete(refresh_timer);
    refresh_timer = NULL;
    chart = next;
    generation++;
    lv_obj_add_event_cb(chart, chart_deleted, LV_EVENT_DELETE, NULL);
    chart_render(NULL); /* Replace generated preview values with missing data. */
    if (!initialized) { LOG_W("battery history synchronization unavailable"); return; }
    if (!history_thread) {
        rt_thread_t worker = rt_thread_create("batgraph", history_worker, NULL, 6144, 24, 10);
        if (!worker) { LOG_W("battery history worker allocation failed"); return; }
        if (rt_thread_startup(worker) != RT_EOK) {
            rt_thread_delete(worker);
            LOG_W("battery history worker start failed");
            return;
        }
        history_thread = worker;
    }
    helios_battery_usage_t usage;
    rt_mutex_take(&history_lock, RT_WAITING_FOREVER);
    usage = cached;
    rt_mutex_release(&history_lock);
    time_t now = time(NULL);
    if (usage.count && now >= usage.end && now - usage.end <= 300) chart_render(&usage);
    refresh_timer = lv_timer_create(history_poll, 1000, NULL);
    if (!refresh_timer) { LOG_W("battery history timer allocation failed"); return; }
    history_request();
}
