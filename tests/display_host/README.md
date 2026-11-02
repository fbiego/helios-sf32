# Display mounting rotation host tests

```sh
python3 tests/display_host/run.py
```

Compiles the real display adapter with a fake LVGL/SDK display interface.
Checks a known clockwise pixel pattern, source stride, dirty rectangles,
restoration of the SDK layer source area, asynchronous scratch-buffer reuse,
allocation/configuration failures, idempotent initialization, and the unrotated
board path. The mocks follow the bundled LVGL pixel/area rotation conventions;
the touch inverse is checked against clockwise physical pixel positions.

These tests do not simulate LCD DMA or the touch controller. Rotation direction,
touch alignment, and animation performance still need an on-board check.
