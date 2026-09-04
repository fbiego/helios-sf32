# Helios App Runtime

This folder owns runtime data that should survive LVGL screen deletion.

LVGL Editor generated files should stay focused on layout and reusable components. Custom screen/event code should treat generated screens as temporary views over this API state.

For the platform/BLE-facing callable API, see `API.md`.

Each app that owns runtime data lives in its own folder:

- `contacts/`
- `notifications/`
- `stopwatch/`
- `weather/`

Shared app infrastructure lives beside those folders:

- `app_init.c` registers built-in apps and initializes their state.
- `app_manager.c` owns the applications registry used by the app list.
- `app_screens.c` owns reusable screen wrappers and navigation gestures.

## Lifecycle

Screens are created when opened and deleted when navigating away because `lv_screen_load_anim(..., true)` is used.

That means:

- Do not store important app data inside screen objects.
- Do not keep long-lived `lv_obj_t *` pointers after `LV_EVENT_DELETE`.
- Store data in `custom/apps/<app_name>`.
- Render the current API data when a screen loads.
- Register a temporary observer while the screen is alive.
- Unregister that observer on `LV_EVENT_DELETE`.

The current API stores data in static RAM. It survives screen deletion, but it does not survive a device reboot, process restart, or firmware reload. If data must survive reboot, load it into these APIs from flash, filesystem, Bluetooth sync, phone sync, or another platform service during startup.

## Notifications

Use notifications for messages that can be added while the UI is running.

```c
helios_notifications_add(
    icon_chat,
    "Messages",
    "18:42",
    "New message received"
);
```

If the notifications screen is open, the item appears immediately. If the screen is not open, the item is stored and appears the next time the screen is created.

## Contacts

Contacts are stored in RAM and rendered into the contacts list when the contacts screen opens.

```c
helios_contacts_add("John Doe", "03244875324");
```

Use `helios_contacts_clear()` before resyncing a fresh phone contact list.

## Weather

Weather data is split into hourly and daily forecast rows.

```c
helios_weather_hourly_clear();
helios_weather_daily_clear();

helios_weather_hourly_add(HELIOS_WEATHER_ICON_RAIN, "18:00", "21°", "42%");
helios_weather_daily_add(HELIOS_WEATHER_ICON_CLOUD, "Thu", "20°");
```

The weather screen keeps the generated weather cards, then clears and redraws the hourly row and daily column from this API.

## Applications

The app registry drives the applications list.

```c
static lv_obj_t * my_app_create(void)
{
    lv_obj_t * screen = lv_obj_create(NULL);
    /* build screen */
    return screen;
}

helios_apps_register(
    icon_application,
    "My App",
    "my_app",
    my_app_create,
    LV_SCR_LOAD_ANIM_OVER_LEFT
);
```

External apps can also self-register from their own `.c` file with a constructor macro:

```c
static lv_obj_t * my_app_create(void)
{
    lv_obj_t * screen = lv_obj_create(NULL);
    /* build screen */
    return screen;
}

HELIOS_REGISTER_APP(
    icon_application,
    "My App",
    "my_app",
    my_app_create,
    LV_SCR_LOAD_ANIM_OVER_LEFT
);
```

The macro does not register the app immediately inside the constructor. It stores a small initializer callback, then the app manager runs that callback during `helios_apps_init_all()`, after generated images and subjects are initialized.

For generated screens that only need the common app gesture, register the generated create function as a simple app:

```c
helios_apps_register_simple(
    icon_phone_link,
    "Phone Link",
    "",
    phone_create,
    LV_SCR_LOAD_ANIM_OVER_LEFT
);
```

Or use the matching constructor macro:

```c
HELIOS_REGISTER_SIMPLE_APP(
    icon_phone_link,
    "Phone Link",
    "phone_link",
    phone_create,
    LV_SCR_LOAD_ANIM_OVER_LEFT
);
```

This launches the app later with `simple_app_screen_create(phone_create)`, so the screen is created on tap, not during registration.

To keep an app ready for a custom transition later, use the transition variants:

```c
helios_apps_register_simple_transition(
    icon_phone_link,
    "Phone Link",
    "",
    phone_create,
    LV_SCR_LOAD_ANIM_OVER_LEFT,
    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT
);
```

Custom transitions are currently routed through the normal `lv_screen_load_anim()` path. The transition enum is kept in the API so app registrations do not need to change when a safer custom animation is added later.

If `create_cb` is `NULL`, the app item is still displayed, but tapping it will not launch anything.

## Stopwatch

The stopwatch stores elapsed time and lap rows in `custom/apps/stopwatch/stopwatch.c`.

The visible time, checked button states, and button icons are synchronized through subjects:

- `sb_stopwatch_time`
- `sb_stopwatch_state`
- `sb_stopwatch_button_start_state`
- `sb_stopwatch_button_reset_state`
- `sb_stopwatch_button_start_icon`
- `sb_stopwatch_button_reset_icon`

Lap rows are not subject-backed. They are stored in RAM by the stopwatch API and redrawn into `lap_container` when the stopwatch screen is opened again.

The LVGL timer runs only while the stopwatch is running.

## Threading

Call these APIs from the LVGL/UI thread.

If data arrives from another task, Bluetooth callback, transport thread, or ISR-adjacent code, queue it into the UI thread first. Depending on the platform, use `lv_async_call()` or the system UI message queue, then call the API from there.
