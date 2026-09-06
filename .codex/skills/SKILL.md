---
name: helios-sf32-project
description: Work on the Helios SF32 watch project in this external repository, including LVGL v9 UI, Nordic UART Service BLE, Chronos protocol hooks, sf32lb52-lcd_n16r8 builds, and board peripheral access.
---

# Helios SF32 Project

Use this skill when modifying or explaining the Helios SF32 watch project in this external repository.

## Project Map

- Build project: `project/`
- App sources: `src/`
- Source discovery: `src/SConscript`
- Chronos parser/state: `src/chronos_core/`
- BLE/NUS service: `src/helios_ble.c`
- Protocol extension hook: `src/helios_chronos.c`
- iOS ANCS/AMS bridge: `src/helios_ios.c`
- Platform-to-UI glue: `src/helios_app.c`
- LVGL watch UI: `src/helios_ui.c`
- Generated/custom UI tree: `src/helios_ui/`
- External watchfaces: `src/faces/`
- Health sensor: `src/helios_max30100.c`
- Board/peripheral helpers: `src/helios_platform.c`
- Target board in current testing: `sf32lb52-lcd_n16r8`

## Build

Always build from this repository's `project/` directory after source or config changes:

```bash
cd project
scons --board=sf32lb52-lcd_n16r8 -j8
```

The SDK environment must be active first with `source ./export.sh` from the SiFli SDK root if needed.

Expected non-fatal warning: `img "dfu" not found`, because the current partition table reserves a DFU slot but this minimal project does not build a DFU image.

`src/SConscript` compiles:

- Top-level `src/*.c` files.
- `src/chronos_core/*.c`, excluding `watch_example.c`.
- Generated/custom LVGL sources listed in `src/helios_ui/file_list_gen.cmake` and `src/helios_ui/user_config.cmake`, excluding XML parsers and large preview files disabled for firmware.
- Custom app sources under `src/helios_ui/custom/apps/apps/`.
- Every `.c` under `src/faces/`, with `ENABLE_FACE_<FACE_NAME>` defines generated from folder names.

## BLE Rules

This project must expose real Nordic UART Service UUIDs, not SiFli's transparent serial service:

- Service: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- RX/write: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E`
- TX/notify: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E`

SiFli ATT UUID arrays are LSB-first. Keep the byte order in `helios_ble.c` reversed from the human-readable UUID.

Do not re-enable `CONFIG_BSP_BLE_SERIAL_TRANSMISSION` for NUS work. Register the service directly with `sibles_register_svc_128()`.

Use:

- `helios_chronos_on_rx(const uint8_t *data, uint16_t len)` for incoming RX writes.
- `helios_ble_send(const uint8_t *data, uint16_t len)` for TX notifications.

Only send TX data after the central enables the TX CCCD. `helios_ble_send()` checks this.

## iOS ANCS/AMS Rules

Keep iOS support in `src/helios_ios.c` with the small public hook surface in `src/helios_ios.h`.

- The iPhone exposes ANCS and AMS as remote GATT services; the watch discovers them as a GATT client after the iPhone connects.
- Keep `CONFIG_BSP_BLE_ANCS=y` and `CONFIG_BSP_BLE_AMS=y` enabled in `project/proj.conf`.
- On connect, request bonding with `ble_gap_security_request()` using `GAP_AUTH_REQ_SEC_CON_BOND`.
- Start ANCS/AMS discovery on connection, but refresh subscriptions again after `GAPC_PAIRING_SUCCEED` and `BLE_GAP_ENCRYPT_IND`; iOS may not fully honor notification/media CCCDs until bonding/encryption is settled.
- ANCS notifications arrive with iOS app bundle IDs in the APP_ID attribute. Map common bundle IDs to the existing Chronos-style notification icon IDs before queueing them into `helios_app_chronos_notification()`.
- AMS media commands should be attempted first from `helios_chronos_send_music_action()`, falling back to Chronos/NUS only when AMS is not ready or the command mask does not support the action.
- Avoid direct LVGL calls from BLE event callbacks. Queue UI updates through `helios_app_chronos_notification()` or `helios_app_music_update()`.

Useful iOS debug logs:

- `ANCS ready result=0` means ANCS service discovery and registration succeeded.
- `AMS ready result=0` means AMS service discovery and registration succeeded.
- `iOS bonded refresh` should appear after pairing/encryption to re-enable ANCS/AMS CCCDs.
- `AMS command mask=...` must be nonzero before media control buttons can work through AMS.
- `ANCS publish icon=... app='...'` shows the final app label and icon ID selected for the UI.

## UI Rules

Keep display and touch access abstracted through `littlevgl2rtt_init("lcd")`. Do not bind app UI code directly to the CO5300 display or FT6146 touch driver unless the task is explicitly driver-specific.

For generated/custom UI imports, keep generated LVGL code isolated from the platform, BLE, and protocol files. Prefer wiring it from `helios_ui.c`.

Generated LVGL files live under `src/helios_ui/` and usually use `_gen.c` / `_gen.h` suffixes. Avoid hand-editing generated files unless the user explicitly asks for a generated-code patch; prefer `src/helios_ui/custom/` or top-level glue files.

Important UI runtime paths:

- `src/helios_ui/custom/subjects/`: subject setters for scalar state such as time, date, battery, BLE, music, health, settings, and selected icons.
- `src/helios_ui/custom/apps/`: runtime state for dynamic app data that survives screen deletion.
- `src/helios_ui/custom/apps/API.md`: platform-facing UI runtime API reference.
- `src/helios_ui/custom/events/`: UI event callbacks.
- `src/helios_ui/widgets/`: generated widget library.
- `src/helios_app.c`: cross-thread/LVGL-safe bridge from BLE, sensors, and platform tasks into the UI.

Screens are created and deleted during navigation. Do not keep long-lived `lv_obj_t *` pointers from platform/BLE/sensor code. Store dynamic data in the app runtime and update visible scalar state through `helios_subject_set_*()` helpers.

When data arrives from BLE callbacks, sensor threads, or ISR-adjacent code, queue it into the UI thread with `lv_async_call()` or existing wrappers such as `helios_app_chronos_notification()` and `helios_app_music_update()`.

## App Runtime Rules

Use the custom app runtime for data that must persist while screens are deleted/recreated:

- Notifications: `src/helios_ui/custom/apps/notifications/`
- Contacts: `src/helios_ui/custom/apps/contacts/`
- Weather: `src/helios_ui/custom/apps/weather/`
- Stopwatch: `src/helios_ui/custom/apps/stopwatch/`
- App registry/navigation: `src/helios_ui/custom/apps/app_manager.c` and `app_screens.c`
- Health app content: `src/helios_ui/custom/apps/apps/health.c`

The app registry supports built-in/simple app registration and constructor-style self-registration. Generated app screens should usually be registered with the simple app wrappers so common gestures and navigation behavior stay consistent.

## Watchface Rules

External watchfaces live under `src/faces/<face_name>/`; current imported faces include `174_390`, `228_390`, `1889_2_390`, and `1891_2_390`.

Each face folder may contain a `.c`/`.h` pair, `items.txt`, `watchface.png`, `preview.png`, and generated image assets. `src/SConscript` compiles face sources automatically.

Use `src/helios_ui/custom/watchfaces/watchface_manager.h` for registration and active-face selection:

- Register from platform/init code with `helios_watchfaces_register(...)`, or from a face `.c` file with `HELIOS_REGISTER_WATCHFACE(...)`.
- Keep watchface tags stable if platform storage persists the selected face.
- The active watchface is rendered on the home screen. Long-pressing home opens the selector.
- Preview images are optional for firmware builds; avoid pulling large preview assets into flash unless needed.
- See `src/helios_ui/custom/watchfaces/README.md` before changing the watchface manager.

## Peripheral Rules

Use `helios_platform.c` for board access:

- Battery: `helios_battery_read()`, currently `bat1` ADC channel 7.
- Buttons: `helios_button_pressed()`, based on board `BSP_KEY*` macros.
- I2C: `helios_i2c_bus()`, default `i2c2`.
- SPI: `helios_spi_device()`, default lookup is only a placeholder and may need board-specific attachment.
- Storage: use RT-Thread DFS/MTD helpers; avoid hard-coding flash layout outside project config.

When adding a real sensor, first check existing SDK drivers under `customer/peripherals/sensor/` and board config under `customer/boards/sf32lb52-lcd_n16r8/hcpu/board.conf`.

## MAX30100 Notes

`src/helios_max30100.c` is modeled after the oxullo Arduino MAX30100 behavior, adapted for RT-Thread/SF32:

- Return both heart rate and SpO2 to the health UI.
- Detect whether a finger is present before publishing live readings.
- Keep a configurable last-valid-value cache so readings do not drop immediately to zero when the finger is removed.
- Release cached values after the configured timeout so stale readings do not remain indefinitely.
- Keep sensor callbacks/thread work LVGL-safe by publishing through the app/UI subject helpers rather than touching generated widgets directly.

## Configuration Notes

Keep these enabled for the current minimal watch build:

- `CONFIG_BLUETOOTH=y`
- `CONFIG_BSP_BLE_ANCS=y`
- `CONFIG_BSP_BLE_AMS=y`
- `CONFIG_BSP_BLE_NVDS_SYNC=y`
- `CONFIG_PKG_USING_FLASHDB=y`
- `CONFIG_PKG_USING_LITTLEVGL2RTT=y`
- `CONFIG_LVGL_V9=y`
- `CONFIG_RT_USING_MEMHEAP=y`
- `CONFIG_USING_BUTTON_LIB=y`

The project intentionally has no DFU implementation yet. If adding OTA, use an SDK DFU v2 example as the reference and revisit the partition table.
