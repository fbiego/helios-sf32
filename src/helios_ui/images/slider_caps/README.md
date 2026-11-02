# Slider end overlays

Black PNG overlays with transparent semicircular cutouts and antialiased edges.
Place the left and right images flush against the respective ends of a rectangular
slider on a black background. Render them above both the track and indicator,
with no padding, scaling, or input handling.

| Slider height | Radius | Each cap's dimensions | Files |
| --- | --- | --- | --- |
| 110 px | 55 px | 55 × 110 px | `slider_cap_left_110.png`, `slider_cap_right_110.png` |
| 90 px | 45 px | 45 × 90 px | `slider_cap_left_90.png`, `slider_cap_right_90.png` |
| 60 px | 30 px | 30 × 60 px | `slider_cap_left_60.png`, `slider_cap_right_60.png` |

Set the slider track and indicator radii to zero when using these overlays.
The images cover only the corner areas; the slider shows through the cutouts.
The two small end images avoid a transparent image spanning the full slider width.

Regenerate with `python3 scripts/generate_slider_caps.py` (no dependencies).
The PNGs use 8-bit grayscale plus alpha; LVGL asset conversion determines their
final runtime color format and memory use.
