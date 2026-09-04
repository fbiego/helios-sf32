# Helios Watchface Manager

The watchface manager owns the list of available watchfaces, the active watchface index, and the preview item data used by the selector screen.

Generated watchface components should stay focused on layout. Register them with this manager so the home screen and selector can create them at runtime.

## Files

- `watchface_manager.h` exposes the registration and selection API.
- `watchface_manager.c` stores registered watchfaces in static RAM.
- `watchface_previews.c` is an editor/sim-only bridge that attaches preview image assets to registered watchfaces.
- `custom/apps/app_screens.c` renders the active watchface on the home screen and populates the selector.
- `custom/apps/app_init.c` calls `helios_watchfaces_init_all()` during startup.

## Lifecycle

Watchfaces are registered during startup, similar to applications.

The home screen creates only the active watchface. When the user long-presses the home screen, the selector screen opens, builds one `face_preview` item per registered watchface, highlights the active one, and scrolls it into view.

When the user taps a preview, the manager updates the active index and returns to the home screen.

## Registering A Watchface

Header:

```c
#include "custom/watchfaces/watchface_manager.h"
```

A watchface create callback receives the parent container and returns the root object it created:

```c
static lv_obj_t * my_watchface_create_cb(lv_obj_t * parent)
{
    return my_watchface_create(parent);
}
```

Register external watchfaces with one preview image:

```c
helios_watchfaces_register(
    "My Face",
    "my_face",
    my_watchface_create_cb,
    img_my_face_preview
);
```

The preview image can be any image pointer exported by the project, such as an `img_*` symbol from `helios_ui_gen.h`.

You can also self-register from the watchface `.c` file:

```c
HELIOS_REGISTER_WATCHFACE(
    "My Face",
    "my_face",
    my_watchface_create_cb,
    img_my_face_preview
);
```

The macro uses `__attribute__((constructor))`, but the constructor does not directly register the watchface. It queues a small initializer callback, then the manager runs that callback during `helios_watchfaces_init_all()`, after generated assets and subjects are initialized.

## Startup Integration

Register external watchfaces after `helios_watchfaces_init()` has run and before the selector is opened.

The usual place is beside app registration during startup:

```c
void platform_ui_register_watchfaces(void)
{
    helios_watchfaces_register(
        "My Face",
        "my_face",
        my_watchface_create_cb,
        img_my_face_preview
    );
}
```

If your platform owns a broader startup hook, call it after `helios_ui_init()` has initialized generated assets and subjects.

## Explicit Preview Resolution

Most external watchfaces should use `helios_watchfaces_register()`.

If the preview image is generated for a known resolution and you want to record it, use:

```c
helios_watchfaces_register_preview_res(
    "My Face",
    "my_face",
    my_watchface_create_cb,
    img_my_face_preview,
    466,
    466
);
```

The selector currently falls back to the first preview when there is no exact resolution match, so this is optional for external watchfaces.

## Built-In Default Watchface

The default watchface is special because it has multiple preview images for different screen resolutions.

It is registered internally by `helios_watchfaces_init_all()` without preview data:

```c
helios_watchfaces_register("Default", "default", default_watchface_create, NULL);
```

When preview assets are enabled, `watchface_previews.c` runs as a deferred initializer and attaches the default previews with:

```c
helios_watchfaces_set_previews("default", previews, preview_count);
```

External watchfaces do not need to use the multi-preview API.

## Preview Assets

Preview image data files are optional. They are useful in the LVGL Editor selector screen, but they can waste flash in firmware builds.

The generated `img_preview_*` compatibility pointers are declared by the manager. In normal firmware builds they are weak `NULL` pointers, so generated placeholder UI can still compile without linking large preview image arrays.

The real preview descriptors live in:

```text
images/preview/img_preview_240_data.c
images/preview/img_preview_280_data.c
images/preview/img_preview_360_data.c
images/preview/img_preview_390_data.c
images/preview/img_preview_410_data.c
images/preview/img_preview_466_data.c
```

`user_config.cmake` includes those files only when `HELIOS_INCLUDE_WATCHFACE_PREVIEWS` is enabled. It defaults to enabled for LVGL Editor builds and disabled elsewhere.

Force previews on:

```bash
cmake -DHELIOS_INCLUDE_WATCHFACE_PREVIEWS=ON ...
```

Force previews off:

```bash
cmake -DHELIOS_INCLUDE_WATCHFACE_PREVIEWS=OFF ...
```

For firmware projects, leave this off unless you intentionally want preview images in flash.

## Selection API

```c
uint32_t helios_watchfaces_count(void);
const helios_watchface_t * helios_watchfaces_get(uint32_t index);

uint32_t helios_watchfaces_active_index(void);
const helios_watchface_t * helios_watchfaces_active(void);
bool helios_watchfaces_set_active(uint32_t index);
```

Use `helios_watchfaces_set_active()` if platform code restores the saved active watchface from flash. Store the active index or the watchface tag in platform storage, then set it after registering all watchfaces.

Example:

```c
helios_watchfaces_set_active(saved_watchface_index);
```

## Notes

- Registration order is the selector order.
- The manager stores data in static RAM. It does not persist across reboot.
- Do not keep `helios_watchface_t *` pointers long term; use count/get at the call site.
- The `tag` should be stable if platform code persists the selected watchface.
- Call these APIs from normal task/thread context, not from an ISR.
