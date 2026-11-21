#ifndef HELIOS_BATTERY_H
#define HELIOS_BATTERY_H

#include <stdbool.h>
#include <time.h>
#include "helios_platform.h"
#include "helios_features.h"

/* Provisional usable voltage range under load. Override for a measured cell/board. */
#ifndef HELIOS_BATTERY_EMPTY_MV
#define HELIOS_BATTERY_EMPTY_MV 3500
#endif
#ifndef HELIOS_BATTERY_FULL_MV
#define HELIOS_BATTERY_FULL_MV 4200
#endif
#if HELIOS_BATTERY_FULL_MV <= HELIOS_BATTERY_EMPTY_MV
#error Battery full voltage must exceed empty voltage
#endif

static inline uint8_t helios_battery_percent_from_mv(uint32_t mv)
{
    if (mv <= HELIOS_BATTERY_EMPTY_MV) return 0;
    if (mv >= HELIOS_BATTERY_FULL_MV) return 100;
    return (uint8_t)((mv - HELIOS_BATTERY_EMPTY_MV) * 100 /
                     (HELIOS_BATTERY_FULL_MV - HELIOS_BATTERY_EMPTY_MV));
}

typedef struct {
    time_t timestamp; /* Local hour start; buckets overlap [start, end]. */
    uint32_t samples, average_mv, min_mv, max_mv, last_mv;
    uint8_t percent, charging;
} helios_battery_hour_t;

typedef struct {
    time_t start, end;
    unsigned count; /* Up to 25 overlapping hourly buckets, oldest first. */
    helios_battery_hour_t hours[25]; /* samples == 0 means missing data. */
} helios_battery_usage_t;

/* Thread context: filesystem reads under the storage I/O lock. Includes current
 * uncheckpointed samples. Returns false if unavailable/invalid time/arguments.
 * Missing or unreadable days have empty buckets. Do not call on the LVGL thread. */
bool helios_battery_get_usage_24h(time_t now, helios_battery_usage_t *usage);

typedef enum {
    HELIOS_BATTERY_RUNTIME_LEARNING,
    HELIOS_BATTERY_RUNTIME_AVAILABLE,
    HELIOS_BATTERY_RUNTIME_CHARGING,
    HELIOS_BATTERY_RUNTIME_STALE
} helios_battery_runtime_status_t;

typedef struct {
    helios_battery_runtime_status_t status;
    uint32_t remaining_seconds; /* Valid only with AVAILABLE. */
    uint32_t observed_seconds;
    uint8_t percent_used;
} helios_battery_runtime_t;

/* Thread context, no I/O. Feed successful periodic reads (including charging).
 * Time changes, gaps > 5 min and charging restart the learning window. */
void helios_battery_observe(time_t now, const helios_battery_t *battery);
bool helios_battery_estimate_remaining(time_t now, helios_battery_runtime_t *runtime);

#endif
