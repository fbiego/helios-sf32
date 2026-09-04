# Helios UI

Helios UI is an LVGL-based smartwatch UI layer built with LVGL Pro Editor. It combines generated screens, components, widgets, images, and fonts with custom LVGL runtime code for watch apps, watchfaces, subjects, and UI state.

The project is intended for smartwatch displays on embedded targets such as ESP32-based devices or other suitable hardware. It is only the LVGL UI layer; hardware, connectivity, storage, sensors, and system services are expected to live outside this repository and provide data to the UI. The included desktop and web simulator targets are mainly for testing, debugging, and previewing the UI without flashing hardware.

## Features

- Generated LVGL screens for home, applications, settings, notifications, contacts, weather, music, stopwatch, timer, phone, navigation, and simulator views.
- Reusable generated components for controls, cards, rows, text styles, weather widgets, timer widgets, dialogs, and watchface previews.
- Custom runtime state for apps whose data should survive screen deletion, including contacts, notifications, weather, and stopwatch.
- Watchface registry for built-in and external watchfaces.
- LVGL helper APIs for updating visible UI data from application code.
- Desktop simulator and Emscripten web preview targets for testing.
- Multiple display targets defined in `project.xml`, including round and rounded-rectangle watch displays.

## Preview

You can preview and interact with the UI directly in your browser through the LVGL online viewer:

[https://viewer.lvgl.io/?repo=https://github.com/fbiego/helios_ui](https://viewer.lvgl.io/?repo=https://github.com/fbiego/helios_ui)

A live web build is also available here:

[https://htmlpreview.github.io/?https://github.com/fbiego/helios_ui/blob/master/preview-bin/web/index.html](https://htmlpreview.github.io/?https://github.com/fbiego/helios_ui/blob/master/preview-bin/web/index.html)

## Repository Layout

```text
.
├── components/          Generated reusable LVGL components
├── custom/              Hand-written runtime, app, event, subject, and watchface code
├── fonts/               Generated font assets and raw source fonts
├── images/              Generated image assets and source PNGs
├── screens/             Generated LVGL screens
├── sim/                 Desktop simulator target
├── sim/web/             Emscripten web simulator target
├── widgets/             Generated widget library
├── CMakeLists.txt       Generated CMake entry point for simulator-style builds
├── project.xml          Project metadata and display targets
└── user_config.cmake    Custom source list included by the generated build
```

Most generated files use the `_gen.c` / `_gen.h` suffix. Prefer making custom behavior changes under `custom/` or through `user_config.cmake` so generated exports can be refreshed safely.


## Custom UI Runtime

Runtime state that should outlive generated screen objects lives under `custom/apps/`. Screens are created and deleted during navigation, so app data should not be stored in screen-local LVGL objects.

Useful entry points:

- [custom/apps/README.md](custom/apps/README.md) explains app lifecycle and runtime storage patterns.
- [custom/apps/API.md](custom/apps/API.md) documents UI update APIs intended to be called by application code.
- [custom/watchfaces/README.md](custom/watchfaces/README.md) documents watchface registration and selection.

## Adding Code

Add hand-written C sources to `user_config.cmake`:

```cmake
list(APPEND LV_EDITOR_PROJECT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/custom/my_feature.c
)
```

This keeps custom sources separate from generated source lists such as `file_list_gen.cmake`.

## Display Targets

Display targets are declared in `project.xml`. Current targets include:

- `target_466R` - 466 x 466 round
- `target_360R` - 360 x 360 round
- `target_240R` - 240 x 240 round
- `target_240` - 240 x 280 rounded rectangle
- `target_390` - 390 x 450 rounded rectangle
- `target_410` - 410 x 502 rounded rectangle

## Notes

- Generated CMake and LVGL Editor files may be overwritten by future exports.
- Use subjects for scalar UI state such as time, date, battery, connectivity, and music metadata.
- Use the runtime APIs for LVGL-visible dynamic data such as notifications, contacts, weather forecasts, app registration, and watchface registration.
