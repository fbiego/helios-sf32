# Persistent watch data

Firmware uses LittleFS on the partition defined by `FS_REGION_START_ADDR` and
`FS_REGION_SIZE` in the generated partition table. It registers this bounded
region as `heliosfs`, mounts it at `/`, and stores application files under
`/data`. It never treats the whole flash chip as a filesystem.

Required project configuration:

```text
CONFIG_RT_USING_DFS=y
CONFIG_RT_USING_MTD_NOR=y
CONFIG_RT_USING_LITTLE_FS=y
CONFIG_PKG_USING_CJSON=y
```

FlashDB remains enabled for one-time preference migration. FAT support may
remain enabled for other media, but these application files use LittleFS.

## Files

```text
/data/settings.json
/data/weather.json
/data/notifications.json
/data/contacts.json
/data/qr.json
/data/user.json
/data/battery/YYYY-MM-DD.json
/data/steps/YYYY-MM-DD.json
```

Every JSON document has `"version": 1`. Application collection files also have
an `updated_at` Unix timestamp. Strings are escaped with cJSON, including UTF-8
notification messages. Image addresses, LVGL objects and pointers are never
serialized. Notification icons use a stable numeric table; future entries
should be appended rather than reordering existing IDs.

- `settings.json`: a `values` object containing preference keys such as
  `screen.brightness`, `screen.timeout`, and `ui.watchface.tag`.
- `weather.json`: current scalar values, city, units, update label, hourly
  `items`, and `daily` forecasts. Cached forecasts older than seven days, or
  dated in the future relative to the RTC, are not displayed on restore.
- `notifications.json`: `items` with title, displayed time, message, icon ID and
  read flag, in collection order. The current UI limit is 16 notifications.
- `contacts.json`: name/number `items` and `sos_index` (-1 means none). The
  current UI supports 64 contacts; larger phone lists are clipped to this limit.
- `qr.json`: nine strings in `items`, using the Chronos QR slot order. QR data
  also restores into Chronos state; this does not add a new QR application UI.
- `user.json`: the phone-synced age, height (cm), weight (kg), step length (cm)
  and target steps. See the profile/calorie behavior below.

## User profile and calories

The profile matches the five fields and defaults in
[Chronos Watchy's HealthModule](https://github.com/fbiego/chronos-watchy/blob/master/lib/HealthModule/HealthModule.cpp):

```json
{
  "version": 1,
  "updated_at": 1793534400,
  "age": 25,
  "height": 175,
  "weight": 70,
  "stepLength": 70,
  "targetSteps": 5000
}
```

These defaults are used until a valid stored or phone-synced profile is
available. The Chronos user packet is decoded using the same packed field
positions as Watchy's `CF_USER` handler. The phone encodes the step target in
thousands, which firmware expands into `targetSteps`. BLE callbacks queue the
profile into the UI thread; no profile file is written from a BLE callback.
Repeated identical profiles do not trigger another save. Invalid/missing,
noninteger or out-of-range JSON fields are rejected as a complete profile;
zero height, weight or step length is rejected. Zero age/target is permitted as
unspecified. Invalid stored profiles are preserved by the existing file policy.

Calories use Watchy's rough step-based estimate, truncated to whole kcal:

```text
distance_km = steps * stepLength_cm / 100000
calories_kcal = distance_km * weight_kg * 1.036
```

For example, 1,000 steps with the default profile gives 50 kcal; 1,000 steps at
90 kg and an 80 cm step length gives 74 kcal. Age, height and daily target are
stored but do not enter this formula. The implementation converts to floating
point before multiplying, avoiding integer overflow for large step counts.

The watch's calorie subject is recalculated from its daily step total and
updates when the phone profile changes. Boards with BMI270 steps enabled also
send live daily steps/calories over Chronos every ten seconds while NUS
notifications are enabled. Editor sample calories are cleared on startup.
Step history gains optional per-hour `calories` and `daily_calories` fields at
its next checkpoint; existing files without these fields remain compatible.
History estimates use the current profile when saved, so changing weight or
step length changes the derived estimates. Individually truncated hourly
calories need not sum exactly to the truncated daily estimate.

`helios_user_distance(steps, metric)` also provides Watchy's distance conversion
(kilometers, or kilometers multiplied by 0.621371 for miles). This change does
not assume an integer scale for the existing distance UI subject.

The firmware removes editor demo collections before restoring real data. On
first use, the collection files are created empty. A complete contacts sync or
an explicit empty contacts list replaces the old list; an initial nonempty
sync announcement alone does not clear the saved contacts.

Notification read-state changes should use
`helios_notifications_set_read(id, read)` so both storage and visible list
observers receive the change. Add/clear operations already notify observers.

## Writes and recovery

Collections are captured in the UI thread. The resulting JSON is copied into a
bounded queue with one pending document per file. A separate `hstore` worker
performs all runtime file writes; it never traverses live UI collections.
Settings setters update RAM and queue a copy rather than writing flash directly.
Brightness still saves only when the slider is released.

The worker batches arrivals for two seconds. Further changes replace an older
pending snapshot. Failed writes retain the newest pending data and retry after
30 seconds. The original file is preserved until the replacement has been fully
written to `.json.tmp`, synced, closed and atomically renamed by LittleFS.
Uncommitted temporary files are ignored after reboot.

A reset can lose updates still in the batch window or an unsuccessful write.
Hourly history is checkpointed every five minutes and at observed hour/day
changes, so a sudden reset can lose the most recent uncommitted interval.

At startup the partition is formatted automatically **only if every byte is
0xff**. A nonblank partition that cannot mount is left untouched. This includes
an existing FAT filesystem; back it up before deliberately migrating its format.
A failed mount disables persistence for that boot rather than falling back to
writing the firmware area or another filesystem.

Invalid JSON, unsupported document versions and rejected collection schemas
are preserved and blocked from replacement. The watch continues with empty
collections/default settings. Export or repair the affected file before
removing it and rebooting; it will not be silently erased or replaced with
sample data. The filesystem is never reformatted as an error-recovery action.

## Preference migration

When `settings.json` does not yet exist, firmware imports the previously used
FlashDB keys from the `helios` database in `dfu`. The original FlashDB data is
left intact. Once a valid JSON settings file exists, that file is authoritative
and the old database is no longer read or written. Unknown preference keys used
by new code can be stored through the existing `helios_prefs_*` APIs.

If migration cannot allocate an imported value, the migration must be retried
rather than committing a partial settings document. If the filesystem cannot
mount, settings persistence is unavailable; firmware does not create a second
writable settings authority in FlashDB.

## Hourly histories

Each day's file contains 24 local-hour slots. Battery rows contain sample count,
sum, average millivolts, minimum, maximum, latest voltage, latest percentage and
latest charging state. Empty hours have zero samples; they are not actual
zero-volt readings. Sampling continues at the existing ten-second battery
interval, without adding another ADC polling timer.

Step rows contain the number of steps observed during each hour; the file also
contains `daily_steps`. The worker tracks the BMI270's cumulative counter deltas,
adds them to the saved daily total after a reboot, and avoids unsigned underflow
if the sensor counter resets. The watchface uses the restored daily total.
Diagnostics continue to show the sensor's raw counter separately.

The most recent 30 local dates are retained. Pruning is restricted to date-named
`.json` files in the battery and steps directories, and occurs when entering a
new logging day. Calendar jumps select the corresponding day's file rather than
subtracting timestamps from unsigned counters. Steps between two observations
are assigned to the hour of the later observation. Repeated daylight-saving
hours share one local-hour slot.

Logging requires a plausible RTC year (2020-2099). The RTC fallback date is
plausible but is not proof that the phone has synchronized time; records made
before an initial time sync will belong to that fallback date. Existing history
from an unsupported/corrupt schema is preserved rather than overwritten.

## Checking on hardware

Run `storage_status` to inspect the mount state, blocked documents, pending
writes and write errors. The SDK's filesystem shell can inspect `/data` (for
example, `ls /data` and `cat /data/settings.json`).

Verify receiving/saving data, waiting for the batch window, rebooting without a
phone, and reconnecting for an updated sync. Verify hourly/day rollover and
current draw with logging enabled. Host tests exercise failure paths and
serialization; actual flash power-loss behavior and battery cost still require
hardware measurement.

Routine flashing preserves data only when the tool leaves `fs_region` intact.
A full-chip erase, filesystem image download, or overlapping future partition
layout can erase it. A factory reset must explicitly clear these application
files if that behavior is desired; this change does not add a factory-reset UI.
