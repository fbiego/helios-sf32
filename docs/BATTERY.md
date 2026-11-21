# Battery calibration and APIs

Include `core/helios_battery.h` (or `helios_battery.h` with the core include path).
All APIs below run in thread context.

## Percentage calibration

The usable range is provisionally **3500–4200 mV**, mapped linearly to 0–100%.
The previous 3300–4200 mV mapping reported 20% at 3480 mV, near the observed
shutdown point. The new empty threshold adds a small reserve above that point.
This changes the estimate; it does not extend physical runtime or change the
hardware shutdown threshold. UI and BLE battery reports use the same calibration.

Override `HELIOS_BATTERY_EMPTY_MV` and `HELIOS_BATTERY_FULL_MV` in the board
profile or build definitions once loaded full/near-shutdown voltages have been
measured. These thresholds are provisional: a voltage-only estimate varies
with load, temperature and cell age. `battery_status` prints the measured VBAT.

## Last 24 hours

```c
helios_battery_usage_t usage;
if (helios_battery_get_usage_24h(time(NULL), &usage)) {
    for (unsigned i = 0; i < usage.count; ++i) {
        const helios_battery_hour_t *hour = &usage.hours[i];
        if (!hour->samples) continue; /* Render a gap, not 0% battery. */
        /* Plot hour->timestamp and hour->percent. */
    }
}
```

Call from a worker and dispatch the copied result to the UI thread. This is a
synchronous filesystem read protected against concurrent storage writes. Do not
call it directly from an LVGL event/timer callback.

`start` and `end` describe the requested rolling 86400-second interval. `hours`
contains up to 25 overlapping local hourly buckets, oldest first, including the
current partial hour. Each bucket contains sample count, average/minimum/maximum
and last voltage, last charging flag, and percentage derived from the last
voltage using the current calibration. Older history therefore uses the same
percentage scale after an update.

The first boundary bucket can include readings from before `start`: storage is
hourly, so it cannot provide an exact second-by-second consumption total. Clip
the chart axis to `[start, end]`. Charging is the last observed state in each
hour, not charging duration. The data describes battery level/voltage, not
measured current or energy in mAh/Wh.

Existing daily battery files retain history across reboots. Current worker-owned
samples are included before their next checkpoint (normally every five minutes);
queued samples can lag by a storage-worker cycle. An abrupt power loss may lose
uncheckpointed readings. Missing, invalid or unreadable days have `samples == 0`.
`false` means storage is unavailable, the output pointer is NULL, or the supplied
time is outside the supported 2020–2099 range. Set the watch clock before use.

## Estimated remaining runtime

```c
helios_battery_runtime_t runtime;
if (helios_battery_estimate_remaining(time(NULL), &runtime) &&
    runtime.status == HELIOS_BATTERY_RUNTIME_AVAILABLE) {
    uint32_t minutes = runtime.remaining_seconds / 60;
    /* Display minutes, or format as hours/minutes. */
}
```

The normal successful 10-second battery poll calls `helios_battery_observe()`.
Each poll also updates `helios_subject_set_battery_usage()` on the UI thread.
Available estimates use days and whole hours, for example `2d 3h` (minutes are
truncated). Learning displays `Estimating...`, charging displays `Charging`,
and stale or unavailable estimates display `--`.
No additional ADC sampling or filesystem access is needed. The estimator keeps
one point per minute over the latest two hours in RAM and extrapolates the net
percentage drop: `remaining_seconds = current_percent * observed_seconds / drop`.
At least 30 minutes and a drop of two percentage points are required. The result
assumes continued similar use; display it as an estimate.

| Status | Display behavior |
| --- | --- |
| `HELIOS_BATTERY_RUNTIME_AVAILABLE` | Show `remaining_seconds`; 0% yields zero seconds. |
| `HELIOS_BATTERY_RUNTIME_LEARNING` | Show “Estimating…”; insufficient time/drop or voltage recovery. |
| `HELIOS_BATTERY_RUNTIME_CHARGING` | Show “Charging”; this API does not estimate time to full. |
| `HELIOS_BATTERY_RUNTIME_STALE` | Show unavailable; no sample, clock moved backward, or sample older than five minutes. |

`observed_seconds` and `percent_used` expose the evidence for a discharge
estimate. `remaining_seconds` is valid only for `AVAILABLE`. NULL output returns
false; otherwise inspect `status`. Charging, reboot, backward clock changes and
sampling gaps longer than five minutes restart learning. Flat or rising battery
levels never produce infinite/negative runtime. Runtime learning does not resume
from hourly files after reboot because hourly records cannot reliably identify
charging transitions within an hour.

## External UI integration

The UI calls its weak `battery_view_attach(lv_obj_t *view)` hook after creating
the battery settings component. `src/core/helios_battery_ui.c` supplies a strong
override, finds the child named `battery_chart`, and populates its first series.
The UI must retain that chart name and create its series before calling the hook.

The override replaces preview data with gaps, displays a cached snapshot if it
is at most five minutes old, and requests fresh data from a lazily started
worker. Only this worker performs history filesystem reads. A UI timer applies
completed snapshots within one second and requests another snapshot every minute
while the chart exists. The point count adapts to the returned hourly buckets;
missing hours use `LV_CHART_POINT_NONE` so they do not appear as zero percent.

The chart's delete event stops its UI timer and invalidates pending responses.
Responses from an earlier view cannot update a reopened view, even if LVGL
reuses a widget address. The worker receives generation numbers, never chart
pointers. It remains asleep between requests, retaining one cached snapshot.
Only one battery view is actively updated at a time. Allocation/read failures
leave gaps and allow reopening or a later refresh to retry.

No imported UI files are edited by this integration. Further widget or subject
changes belong in the external UI project.
