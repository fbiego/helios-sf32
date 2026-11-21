# Phone-synced alarms

Firmware restores eight alarm slots from `/data/alarms.json` at startup.
Chronos alarm packets (`0x73`, exposed as `CHRONOS_CONFIG_ALARM`) update one
slot at a time. The BLE callback only queues a fixed-size copy; the UI thread
applies updates and queues versioned JSON to the storage worker. Failed saves
retry once per second. Invalid existing files are preserved rather than
overwritten, following the project's storage policy.

Alarms use the watch's native local RTC time, independently of the 12/24-hour
display setting. Disabled slots never ring. Repeat follows
[Chronos's alarm checker](https://github.com/fbiego/chronos-esp32/blob/main/src/ChronosESP32.cpp):
Monday through Saturday are bits 0–5, Sunday is bit 6. `0x7f` and `0x80`
match every day; zero has no selected days. These protocol values are not
converted into one-shot alarms.

During a matching minute, a due alarm starts a repeating double beep at 4 kHz:
150 ms on, 100 ms off, 150 ms on, then 850 ms off. Each pair starts 1.25 seconds
after the previous pair; UI scheduling may slightly delay a beep. Delayed
polls skip overdue pairs instead of playing a backlog.
It continues after that minute until either physical button is pressed or the
phone disables/changes all ringing slots. The dismissal press is consumed
before normal button navigation. Old button presses cannot dismiss a new
alarm: detection uses a separate, non-consuming press sequence. A tone is
silenced immediately when dismissal is processed, normally within 250 ms
with the display off. There is no snooze, automatic timeout or alarm UI.

The normal UI loop services alarms with the display on or off. Short PWM
tones hold the CPU out of deep sleep only while sounding. Notifications,
hourly chimes and startup tones cannot replace an active alarm. Simultaneous
alarms share the sound and one button dismisses all of them.

Each slot's last-fired local date is saved with its configuration. Identical
phone resyncs preserve this marker, preventing another ring in the same minute
after dismissal or restart. A changed configuration resets that slot's marker.
As with other queued storage, abrupt power loss before the worker commits can
lose the newest update or firing marker. Missed alarm minutes are not replayed;
starting the watch within a matching minute can ring the restored alarm.

Validation:

- `python3 tests/alarms_host/run.py`: real phone packets, all weekday bits,
  daily masks, disabled slots, simultaneous alarms, restart, dismissal,
  corrupt documents, allocation failure and save retries under sanitizers.
- `python3 tests/diagnostics_host/run.py`: PWM timing, immediate silence,
  stale timer callbacks and missing hardware.
- Storage and sound host suites cover the new file and tone priority.

On hardware, sync an alarm for the next minute, disconnect the phone and
let the display sleep. Check ringing and both buttons. Reboot after sync to
check restoration, and dismiss/reboot within the same alarm minute to check
the committed firing marker. A physical soak test is still required.
