# Helios External API

This document lists the APIs intended to be called from platform code outside the UI layer, such as BLE, phone sync, storage restore, or a companion-service task.

The functions below acquire `lv_lock()` / `lv_unlock()` internally when they mutate UI-owned runtime state. Call them from normal task/thread context. Do not call them from an ISR; queue into a task first.

## General Rules

- Use `helios_subject_set_*()` for subject-backed values.
- Use the app runtime APIs below for dynamic list data such as notifications, contacts, weather, and app registration.
- Use `custom/watchfaces/watchface_manager.h` to register external watchfaces and restore the active watchface.
- Do not keep returned item pointers outside the call site. The current storage is static RAM and may be changed by later API calls.
- Data survives LVGL screen deletion because it is stored in `custom/apps`, not inside screen objects.
- Data does not survive reboot unless your platform code reloads it into these APIs at startup.

## Callbacks

External application code usually does not need to implement callbacks for UI redraw.

The runtime uses internal observer callbacks to update open screens. For example, when BLE calls `helios_notifications_add()`, the notification is stored; if the notifications screen is open, its internal observer redraws the list. If the screen is closed, the data is still stored and will render next time the screen opens.

Use callbacks in two cases:

- UI to platform: implement generated weak callbacks or subject change callbacks when the user changes something in the UI and the platform must react, such as brightness, volume, or music controls.
- Custom app integration: provide an app create function when registering an external app.

Avoid registering your own observers from BLE/platform code unless you are extending the runtime itself. Observers are meant to be UI-layer plumbing and are called while the runtime is updating UI state.

## Subjects

Subject setters are generated in `custom/subjects/subjects.h`.

Examples:

```c
helios_subject_set_time_string("18:42");
helios_subject_set_date_string("Thu, 2 Aug");
helios_subject_set_battery_percent(78);
helios_subject_set_music_title("Midnight Drive");
helios_subject_set_music_artist("Synthwave Mix");
```

Use subjects for scalar or single-value UI state: time, date, battery, connectivity, music metadata, settings values, selected icons, and similar values.

## Notifications

Header: `custom/apps/notifications/notifications.h`

```c
bool helios_notifications_add(const void * icon,
                              const char * title,
                              const char * time,
                              const char * message);

void helios_notifications_clear(void);
```

`helios_notifications_add()` appends a notification. If the list is full, the oldest notification is dropped.

`helios_notifications_clear()` removes all notifications. If the notifications screen is open, the list hides so the empty-state text underneath can show.

## Contacts

Header: `custom/apps/contacts/contacts.h`

```c
bool helios_contacts_add(const char * name, const char * number);
void helios_contacts_clear(void);
```

`helios_contacts_add()` appends a contact row.

`helios_contacts_clear()` removes all contacts. Use it before loading a fresh contact sync from the phone.

## Weather

Header: `custom/apps/weather/weather.h`

```c
bool helios_weather_hourly_add(int32_t icon_id,
                               const char * time,
                               const char * temp,
                               const char * humidity);

bool helios_weather_daily_add(int32_t icon_id,
                              const char * day,
                              const char * temp);

void helios_weather_hourly_clear(void);
void helios_weather_daily_clear(void);
```

Use the clear functions before loading a full replacement forecast. Use the add functions to append rows to the generated hourly and daily forecast containers. Pass a `helios_weather_icon_id_t` value; the API resolves it to the matching weather image internally.

## App Registry

Header: `custom/apps/app_manager.h`

```c
bool helios_apps_register(const void * icon,
                          const char * name,
                          const char * tag,
                          helios_app_create_cb_t create_cb,
                          lv_scr_load_anim_t anim);

bool helios_apps_register_simple(const void * icon,
                                 const char * name,
                                 const char * tag,
                                 helios_app_create_cb_t create_cb,
                                 lv_scr_load_anim_t anim);

bool helios_apps_register_simple_events(const void * icon,
                                        const char * name,
                                        const char * tag,
                                        helios_app_create_cb_t create_cb,
                                        lv_scr_load_anim_t anim,
                                        lv_event_cb_t event_cb);

void helios_apps_clear(void);
```

Use `helios_apps_register()` when `create_cb` returns a full screen.

Use `helios_apps_register_simple()` when `create_cb` is an LVGL Editor generated app content function such as `phone_create`, `music_create`, or `navigation_create`. The runtime wraps it in `simple_app_screen_create()` so the common app gesture is attached.

Use `helios_apps_register_simple_events()` when a simple app also needs an event callback attached to its screen, such as the stopwatch screen.

If `create_cb` is `NULL`, the app item is displayed but tapping it does not launch anything.

Apps can self-register from their own `.c` file with constructor macros:

```c
HELIOS_REGISTER_APP(icon_application, "My App", "my_app", my_app_create, LV_SCR_LOAD_ANIM_OVER_LEFT);
HELIOS_REGISTER_SIMPLE_APP(icon_music, "Music", "music", music_create, LV_SCR_LOAD_ANIM_OVER_LEFT);
```

The constructor only queues a registration callback. The callback runs from `helios_apps_init_all()` after generated assets are initialized, so image globals such as `icon_application` are valid.

## Watchface Registry

Header: `custom/watchfaces/watchface_manager.h`

Full guide: `custom/watchfaces/README.md`

```c
bool helios_watchfaces_register(const char * name,
                                const char * tag,
                                helios_watchface_create_cb_t create_cb,
                                const void * preview);

bool helios_watchfaces_set_active(uint32_t index);
uint32_t helios_watchfaces_active_index(void);
```

Use `helios_watchfaces_register()` for external watchfaces. External watchfaces use one preview image; the built-in default watchface uses the internal multi-preview API because it ships different previews for different screen resolutions.

Watchfaces can also self-register from their own `.c` file:

```c
HELIOS_REGISTER_WATCHFACE("My Face", "my_face", my_watchface_create_cb, img_my_face_preview);
```

The home screen renders the active registered watchface. Long-pressing home opens the selector, which scrolls to the active watchface.

## Stopwatch

Header: `custom/apps/stopwatch/stopwatch.h`

```c
void helios_stopwatch_start(void);
void helios_stopwatch_pause(void);
void helios_stopwatch_toggle(void);
void helios_stopwatch_lap_or_reset(void);
void helios_stopwatch_reset(void);
void helios_stopwatch_lap(void);
```

The stopwatch stores elapsed time and laps in RAM. Time, button checked state, and button icons are synchronized to subjects. Lap rows are redrawn from RAM when the stopwatch screen reopens.

These controls are callable from UI events or platform code. The LVGL timer only runs while the stopwatch is running.

## UI-Layer Read APIs

The runtime also exposes count/get and observer functions for the screen renderers:

```c
uint32_t helios_notifications_count(void);
const helios_notification_t * helios_notifications_get(uint32_t index);
bool helios_notifications_observer_add(...);
void helios_notifications_observer_remove(...);
```

Similar APIs exist for contacts, weather, apps, and stopwatch laps.

Treat these as UI-layer/internal APIs. They return pointers into static runtime storage, so external code should not hold those pointers while another task may mutate the same data.
