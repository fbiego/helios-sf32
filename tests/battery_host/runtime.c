#include <assert.h>
#include <string.h>
#include "core/helios_battery.c"

static int locked;
rt_err_t rt_mutex_init(struct rt_mutex *m, const char *name, uint8_t flags)
{ (void)m; (void)flags; assert(!strcmp(name, "battime")); return RT_EOK; }
rt_err_t rt_mutex_take(struct rt_mutex *m, int32_t timeout)
{ (void)m; (void)timeout; assert(!locked); locked = 1; return RT_EOK; }
rt_err_t rt_mutex_release(struct rt_mutex *m)
{ (void)m; assert(locked); locked = 0; return RT_EOK; }

static void feed(time_t start, unsigned minutes, unsigned percent)
{
    for (unsigned i = 0; i <= minutes; i++) {
        helios_battery_t sample = {.percent = percent - i / 15};
        helios_battery_observe(start + i * 60, &sample);
    }
}

int main(void)
{
    assert(battery_runtime_init() == RT_EOK);
    helios_battery_runtime_t result;
    assert(!helios_battery_estimate_remaining(1, NULL));
    assert(helios_battery_estimate_remaining(1, &result));
    assert(result.status == HELIOS_BATTERY_RUNTIME_STALE);
    feed(1000, 29, 80);
    helios_battery_estimate_remaining(2740, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_LEARNING);
    feed(1000, 30, 80); /* Backward time must restart the window. */
    helios_battery_estimate_remaining(2800, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_AVAILABLE);
    assert(result.remaining_seconds == 78 * 900 && result.percent_used == 2);
    assert(result.observed_seconds == 1800);
    feed(1000, 300, 80); /* Ring wraps; only the most recent two hours count. */
    helios_battery_estimate_remaining(19000, &result);
    assert(count == 121 && result.observed_seconds == 7200);
    assert(result.remaining_seconds == 60 * 900 && result.percent_used == 8);
    helios_battery_estimate_remaining(19301, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_STALE && !result.remaining_seconds);
    helios_battery_t sample = {.percent = 60, .charging = 1};
    helios_battery_observe(19060, &sample);
    helios_battery_estimate_remaining(19060, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_CHARGING);
    sample.charging = 0;
    helios_battery_observe(19120, &sample);
    helios_battery_estimate_remaining(19120, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_LEARNING && count == 1);
    sample.percent = 65; /* Rebound cannot yield negative or infinite runtime. */
    helios_battery_observe(19180, &sample);
    helios_battery_estimate_remaining(19180, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_LEARNING && !result.percent_used);
    helios_battery_observe(20000, &sample); /* Long sampling gap. */
    assert(count == 1);
    sample.percent = 0;
    helios_battery_observe(20060, &sample);
    helios_battery_estimate_remaining(20060, &result);
    assert(result.status == HELIOS_BATTERY_RUNTIME_AVAILABLE && !result.remaining_seconds);
    helios_battery_observe(900, &sample);
    assert(count == 1);
    for (unsigned i = 0; i <= 35; i++) {
        sample.percent = 50 + i / 15;
        helios_battery_observe(100000 + i * 60, &sample);
    }
    helios_battery_estimate_remaining(102100, &result);
    assert(result.observed_seconds == 2100 && !result.percent_used);
    assert(result.status == HELIOS_BATTERY_RUNTIME_LEARNING && !result.remaining_seconds);
    return 0;
}
