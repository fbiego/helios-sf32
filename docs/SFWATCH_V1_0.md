# SFWatch v1.0

Custom SFWatch board pinout.

The `sf32lb52-lcd_custom_466x466_co5300` board profile has been tested and works on this board. The board includes a BMI270 IMU.

## Firmware capabilities

SFWatch applies a 90-degree clockwise display mounting offset using
`HELIOS_DISPLAY_ROTATION_CW=90` in its board header. All LVGL screens and
watchfaces rotate at the display flush boundary, and touch coordinates use
the matching inverse transform. Other board profiles default to 0 degrees.
The adapter preserves the SDK framebuffer transfer/completion callbacks and
uses one aligned scratch buffer sized like an LVGL partial draw buffer
(about 46.6 kB with the current 466x50 RGB565 buffers). This mounting offset is
independent of the stored screen-rotation preference.

The profile's Helios capabilities are defined in
[`src/boards/sfwatch_v1_0.h`](../src/boards/sfwatch_v1_0.h).
BMI270 is enabled; MAX30100 is disabled. The buzzer is declared present and
the PA20 motor output is enabled for the planned connection. The diagnostics
buttons test the passive buzzer with a 200 ms, 2 kHz PWM tone on PA41
(`pwmt2`, channel 2), and pulse the active-high motor output for 250 ms.
Soft timers switch both outputs off without blocking the UI.

BMI270 uses `i2c2` on PA39/PA40 and probes addresses `0x68` and `0x69`.
The profile currently selects 100 kHz for bring-up with
`HELIOS_BMI270_I2C_HZ`. Register access uses the SDK's I2C memory-transfer mode
with an 8-bit register address, including a repeated start for reads.
Initialization failures retry every five seconds. Diagnostics and
`bmi270_status` report the failing stage, Bosch error, address-probe results
(expected chip ID `0x24`), last I2C register/length/result, and attempt count.
The second address is not probed once the first identifies the BMI270.
The driver uploads Bosch's configuration image and enables the accelerometer,
step counter, step detector, and wrist-wear wake feature. The gyroscope is off
by default; opening Diagnostics requests gyro sampling from the sensor worker,
and leaving the page disables it again. Set `HELIOS_IMU_DEBUG_GYRO=1` to force
it on outside Diagnostics as well.

INT1 on PA44 carries latched, active-high wrist-wake and step-detector events.
The GPIO interrupt wakes the sensor worker; it does not access I2C or LVGL.
The worker reads/clears the sensor interrupt status and keeps a thread-safe
snapshot. A one-second timeout refreshes readings and provides a polling fallback
if an interrupt is missed. INT2 on PA43 is reserved and unused.

Step counts are published to the UI once per second. Counts are since sensor
initialization; daily reset and persistence are not implemented. The hardware
counter is the authoritative total: the diagnostic `step_events` count can be
lower if detector interrupts coalesce.

Raise to wake is consumed by the UI's 250 ms idle check when the display is off.
It turns the display on only when `helios_subject_get_screen_rtw()` is nonzero.
The existing `sb_screen_rtw` preference is saved/restored across reboots.
Sensor wake events remain visible in Diagnostics while this setting is off,
but are consumed without waking the display. Button/touch wake is independent.
Events have a 1.5 second debounce and expire after 2 seconds. This wakes the
display in the current running firmware; MCU standby/deep-sleep wake is not
configured.

After screen-off, the UI returns to Home without animation after about 500 ms,
unloading and deleting the previous screen. A wake before that delay restores
and refreshes Home before enabling brightness, so the previous app is not
resumed. The active watchface selection is preserved.

The `bmi270_status` shell command prints remapped accelerometer/gyroscope axes,
steps, detector events, wrist-wake events, and debug roll/pitch when FinSH is
enabled. Gyroscope readings are zero when the gyro is off. Roll and pitch
are in tenths of a degree (`450` means 45 degrees), calculated from gravity.
They are intended for stationary debugging, are affected by motion, and do not
provide compass heading/yaw.

Independent switches in `helios_features.h`, overridable in the board header:

| Flag | SFWatch default |
| --- | --- |
| `HELIOS_IMU_STEPS` | `1` |
| `HELIOS_IMU_RAISE_TO_WAKE` | `1` |
| `HELIOS_IMU_DEBUG_ORIENTATION` | `1` |
| `HELIOS_IMU_DEBUG_GYRO` | `0` |

### Verify sensor mounting

The sensor is top-mounted with its marker toward the bottom left. Face-up
measurements gave roll/pitch -1.1/0 degrees. Held upright with the rotated UI
readable, the original sensor frame gave -32.4/85.1 degrees, indicating gravity
mostly along -sensor X. The profile therefore maps watch X = sensor Y,
watch Y = -sensor X, and watch Z = sensor Z. This is a right-handed mapping
aligned with the rotated UI and is applied through Bosch's axis-remapping API
before enabling wrist wake and step features.

The board header's
`HELIOS_BMI270_MAP_X/Y/Z` values use Bosch's `BMI2_X`, `BMI2_Y`, `BMI2_Z` or
their `BMI2_NEG_*` equivalents. The target watch frame is +X right, +Y toward
the top edge, and +Z out of the display.

With the watch stationary, run `bmi270_status` in these positions and check the
expected positive axis (about 16384 counts for 1 g at the configured range):

| Position | Expected gravity reading |
| --- | --- |
| Display flat and facing up | +Z |
| Display facing you, top edge pointing upward | +Y |
| Display facing you, right edge pointing upward | +X |

After flashing this mapping, confirm near-zero roll/pitch face-up and roughly
+90/0 degrees upright. Also verify the right-edge-up position and raise to wake
while wearing the watch. Hardware validation is still required;
the earlier working-profile test covered the board before this IMU addition.

The project-specific `proj.conf` overrides charger detect to PA25, freeing PA44
for BMI270 INT1. Platform initialization changes PA25 from the SDK base SPI1
input mux to GPIO input with a pull-up for the open-drain signal.
It preserves the SDK's existing charger polarity setting;
the open-drain description alone does not establish the active level.

## Buttons

| Button | GPIO |
| --- | --- |
| Button 1 | PA11 |
| Button 2 | PA34 |

## BMI270 IMU

| Signal | GPIO |
| --- | --- |
| SDA | PA39 |
| SCL | PA40 |
| INT1 | PA44 |
| INT2 | PA43 |

## AMOLED display

| Signal | GPIO |
| --- | --- |
| RST | PA0 |
| TE | PA2 |
| CS | PA3 |
| SCK | PA4 |
| S0 | PA5 |
| S1 | PA6 |
| S2 | PA7 |
| S3 | PA8 |
| EN | PA26 |

## Touch

| Signal | GPIO |
| --- | --- |
| RST | PA9 |
| INT | PA31 |
| SDA | PA33 |
| SCL | PA30 |

## Buzzer and motor

| Function | GPIO | Notes |
| --- | --- | --- |
| Buzzer | PA41 | |
| Motor | PA20 | Not yet connected. |

## Power and charging

### Screen-off low power

SFWatch enables SDK power management (`CONFIG_BSP_USING_PM=y`) using HCPU
DeepSleep (`CONFIG_PM_DEEP_ENABLE=y`). `HELIOS_HAS_LOW_POWER` selects the
SFWatch implementation; other profiles retain their existing display behavior.
The UI holds an IDLE request while the display is on. Screen-off waits for the
pending pixel copy, suppresses LVGL invalidation/refresh, sends the SDK's
synchronous LCD power-off command, disables panel EN on PA26, and releases the
UI's IDLE request. The SDK can then enter DeepSleep whenever threads and active
drivers allow it. No application code forces sleep through an active transfer.

During screen-off, LVGL work is serviced every 250 ms instead of following its
normal animation/input refresh deadlines. Battery reporting continues every
10 seconds and the BMI270 continues counting steps and detecting wrist events.
The BLE stack remains running; its own scheduled radio work wakes the chip as
needed. Buttons, touch and IMU GPIO interrupts remain enabled. The PA10 shared
3.3 V rail stays on for the sensor and touch controller. Disabling raise-to-wake
still prevents wrist events from lighting the display.

Wake acquires the IDLE request, enables PA26, reinitializes the LCD through the
SDK and restores its RGB565 buffer format before allowing rendering. Home is
redrawn before applying the saved brightness. The inherited devkit sleep hooks
are overridden because they otherwise drive SFWatch's PA11 button as a memory
power output. The startup override forcing an always-on timeout is removed;
the selected timeout, including an explicitly selected Always On, is respected.

After flashing, verify timeout/manual sleep, PA11/PA34 and touch wake, wrist
wake with the setting enabled/disabled, and BLE notifications while asleep.
Compare screen-on/off current with a current meter after startup has settled;
actual battery life cannot be inferred from a successful build. Disconnect a
debugger for the measurement. `pm_dump` and `list_timer` can help identify sleep
locks and short timer deadlines, but running a shell command itself wakes the
CPU. A display-off CPU wake is not necessarily a screen-on event.

### Battery readings

Battery voltage is read from the SF32LB52's internal VBAT ADC channel 7 through
the SDK's `bat1` device. Both `CONFIG_BSP_USING_ADC=y` and
`CONFIG_BSP_USING_ADC1=y` are required; enabling ADC alone does not register
`bat1`. The SFWatch project override also sets the SDK battery-detect channel
to 7.

Firmware samples at UI startup and every 10 seconds. The SDK conversion already
includes ADC calibration and the internal VBAT scaling; its 0.1 mV result is
divided by 10 for millivolts. The percentage remains a basic linear estimate
between 3.3 V and 4.2 V. ADC failures are logged and invalid samples are rejected.

Watch battery percentage and charging state are sent over Chronos/NUS when the
phone enables notifications, on device-info requests, and every 10 seconds while
subscribed. Failed ADC reads do not send a placeholder battery level. PA25 LOW
is interpreted as charging; HIGH is interpreted as not charging. Change
`HELIOS_CHARGE_DETECT_ACTIVE_LEVEL` in the board profile if the charger uses the
opposite polarity. The same charging state updates the watch UI.

After flashing, run `battery_status` in the serial shell. It reports measured
VBAT in millivolts and the estimated percentage, or an error if the device or
conversion is unavailable. `list_device` should include `bat1`.

### RTC retention

Startup reads the RTC and preserves an existing calendar in 2020-2099. Only an
unset/out-of-range calendar receives the fallback of 2026-01-01 12:00:00; phone
synchronization can then set the correct time. An unavailable RTC or failed read
does not trigger a clock write. Normal resets and reflashing can therefore keep
time while the RTC remains powered and the programming procedure preserves its
power domain. Battery loss or an RTC-domain reset can still lose the calendar.

| Signal | GPIO | Notes |
| --- | --- | --- |
| 3.3 V enable | PA10 | |
| Charge detect | PA25 | Open drain. |
