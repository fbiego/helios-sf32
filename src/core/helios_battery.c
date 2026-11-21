#include "helios_battery.h"
#include <string.h>

#define WINDOW_SECONDS 7200
#define MAX_GAP_SECONDS 300
#define POINTS 121
static struct rt_mutex runtime_lock;
static struct { time_t time; uint8_t percent; } points[POINTS];
static unsigned first, count;
static time_t last_time;
static uint8_t last_percent, last_charging;
static bool seen;

static int battery_runtime_init(void)
{
    return rt_mutex_init(&runtime_lock, "battime", RT_IPC_FLAG_PRIO);
}
INIT_PREV_EXPORT(battery_runtime_init);

void helios_battery_observe(time_t now, const helios_battery_t *battery)
{
    if (!battery || battery->percent > 100 || now <= 0) return;
    rt_mutex_take(&runtime_lock, RT_WAITING_FOREVER);
    if (!seen || now < last_time || now - last_time > MAX_GAP_SECONDS ||
        battery->charging || last_charging) {
        first = count = 0;
    }
    seen = true;
    last_time = now;
    last_percent = battery->percent;
    last_charging = battery->charging;
    if (!battery->charging) {
        while (count && now - points[first].time > WINDOW_SECONDS) {
            first = (first + 1) % POINTS; count--;
        }
        unsigned tail = count ? (first + count - 1) % POINTS : first;
        if (!count || now - points[tail].time >= 60) {
            if (count == POINTS) { first = (first + 1) % POINTS; count--; }
            unsigned index = (first + count++) % POINTS;
            points[index].time = now;
            points[index].percent = battery->percent;
        }
    }
    rt_mutex_release(&runtime_lock);
}

bool helios_battery_estimate_remaining(time_t now, helios_battery_runtime_t *runtime)
{
    if (!runtime) return false;
    memset(runtime, 0, sizeof(*runtime));
    rt_mutex_take(&runtime_lock, RT_WAITING_FOREVER);
    runtime->status = HELIOS_BATTERY_RUNTIME_LEARNING;
    if (!seen || now < last_time || now - last_time > MAX_GAP_SECONDS)
        runtime->status = HELIOS_BATTERY_RUNTIME_STALE;
    else if (last_charging)
        runtime->status = HELIOS_BATTERY_RUNTIME_CHARGING;
    else if (!last_percent)
        runtime->status = HELIOS_BATTERY_RUNTIME_AVAILABLE;
    else if (count) {
        time_t duration = last_time - points[first].time;
        int drop = (int)points[first].percent - last_percent;
        runtime->observed_seconds = (uint32_t)duration;
        runtime->percent_used = drop > 0 ? (uint8_t)drop : 0;
        if (duration >= 1800 && drop >= 2) {
            runtime->status = HELIOS_BATTERY_RUNTIME_AVAILABLE;
            runtime->remaining_seconds = (uint32_t)((uint64_t)last_percent * duration / drop);
        }
    }
    rt_mutex_release(&runtime_lock);
    return true;
}
