# Imported watchfaces

Each subfolder contains a watchface and its assets. The current faces are
designed for the `sf32lb52-lcd_n16r8` board's display.

Firmware selects faces explicitly in its `src/boards/` capability header.
For example, `src/boards/sf32lb52_lcd_n16r8.h` enables:

```c
#define ENABLE_FACE_174_390 1
#define ENABLE_FACE_228_390 1
#define ENABLE_FACE_1889_2_390 1
#define ENABLE_FACE_1891_2_390 1
```

Set a flag to `0` or omit it to exclude that face. SFWatch v1.0 and the custom
410x502 profile currently enable none of these imports; they retain the built-in
default watchface.

`src/SConscript` reads literal `#define ENABLE_FACE_<FOLDER> 1` declarations
from the selected board header. It compiles only enabled face folders, including
their assets, and supplies their `ENABLE_FACE_*` compiler definitions. Disabled
faces occupy no firmware space and do not register in the selector. Changing a
face's generated header alone does not opt it into a board's build.

Enabled faces use `HELIOS_REGISTER_WATCHFACE(...)` to queue registration with the
watchface manager, which runs their initializers during UI startup.

To add a compatible face, place it in a new subfolder and add its enable flag to
each supported board header. The folder name is uppercased and non-alphanumeric
characters are replaced with underscores to form the flag suffix. New folders
remain disabled until explicitly enabled by a board.
