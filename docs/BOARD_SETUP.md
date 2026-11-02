# Board Setup

This project builds as an out-of-tree SiFli application. The app sources live in this repository, but the board packages are resolved from the active SiFli SDK checkout.

## Required SDK Environment

Before building, activate the SiFli SDK environment:

```bash
source /path/to/SiFli-SDK/export.sh
cd project
```

The `SIFLI_SDK` environment variable must point at the SDK root. The build will stop with `Please source export.sh/export.ps1 in the SiFli SDK root first.` if it is missing.

## Supported Board Targets

The current targets are:

```bash
scons --board=sf32lb52-lcd_n16r8 -j8
scons --board=sf32lb52-lcd_custom_410x502 -j8
scons --board=sf32lb52-lcd_custom_466x466_co5300 -j8
```

`sf32lb52-lcd_n16r8` is the SDK LCD devkit target. It includes the MAX30100 heart-rate/SpO2 sensor path.

`sf32lb52-lcd_custom_410x502` is the custom watch board target. It uses a 410x502 LCD board package and does not populate the MAX30100 sensor.

`sf32lb52-lcd_custom_466x466_co5300` is a custom-board bring-up target for a 466x466 CO5300 AMOLED with CST820 touch, wired to the same custom-board LCD/touch pins. It also does not populate the MAX30100 sensor.

## Custom Board Hardware Notes

Current custom board revision:

- `LCD_EN` was not routed on the PCB.
- The bring-up workaround is to short `LCD_EN` to `3.3V`, so the panel enable is always high.
- A 466x466 CO5300 AMOLED with CST820 touch can be wired from the custom board to the same LCD/touch pins and built with `sf32lb52-lcd_custom_466x466_co5300`.
- The wired 466x466 CO5300 target currently disables `LCD_CO5300_VSYNC_ENABLE` because the TE/VSYNC signal is not proven on the wired bring-up. If TE is enabled without a working TE line, the LCD task can timeout in `draw_core`.
- Because `LCD_EN` is not MCU-controlled on this revision, firmware should not rely on toggling LCD enable for panel power sequencing or display blanking.
- A display failure was reproduced on the original ESP32 board as well, so the observed display issue is likely hardware-side, such as the panel, flex, connector, or power path, rather than this SF32 firmware target alone.

For the next PCB revision, route `LCD_EN` to the intended MCU GPIO or power-control circuit so the board package can manage panel enable normally.

## Custom Board Package

The custom board setup is split across two places:

- SDK board package: `$SIFLI_SDK/customer/boards/sf32lb52-lcd_custom_410x502`
- SDK wired 466x466 board package: `$SIFLI_SDK/customer/boards/sf32lb52-lcd_custom_466x466_co5300`
- Repo project override: `project/sf32lb52-lcd_custom_410x502_hcpu/custom_mem_map.h`
- Repo wired 466x466 override: `project/sf32lb52-lcd_custom_466x466_co5300_hcpu/custom_mem_map.h`

When someone else clones this repository, they also need a SiFli SDK checkout that contains any custom board package they want to build. If their SDK does not have them yet, copy or add the board packages under:

```text
$SIFLI_SDK/customer/boards/sf32lb52-lcd_custom_410x502/
$SIFLI_SDK/customer/boards/sf32lb52-lcd_custom_466x466_co5300/
```

The `sf32lb52-lcd_custom_466x466_co5300` package was cloned from the 410x502 custom board package, then changed to select the SDK's `AM151Q466466LK_151_466x466_C` CO5300/CST820 panel option. The active SDK also needs that panel option available from `$SIFLI_SDK/customer/boards/Kconfig_lcd`, and the shared custom-board LCD/touch power handling in `$SIFLI_SDK/customer/boards/sf32lb52-lcd_base/bsp_lcd_tp.c`.

After that, build from this repository's `project/` directory with the matching target:

```bash
scons --board=sf32lb52-lcd_custom_410x502 -j8
scons --board=sf32lb52-lcd_custom_466x466_co5300 -j8
```

The repo-side `project/<board>_hcpu/custom_mem_map.h` files keep the Helios FlashDB/DFU partition override matched to each custom board build output.

## Board Capability Flags

`src/SConscript` derives board identity flags from the selected `scons --board=...` target:

```c
HELIOS_BOARD_SF32LB52_LCD_N16R8=1
HELIOS_BOARD_SF32LB52_LCD_CUSTOM_410X502=1
HELIOS_BOARD_SF32LB52_LCD_CUSTOM_466X466_CO5300=1
HELIOS_BOARD_NAME="sf32lb52-lcd_custom_410x502"
```

Only the selected board macro is defined for a given build. Include
`helios_features.h` to load its capability header from `src/boards/`.
Use capability flags such as `HELIOS_HAS_MAX30100` for hardware differences in
app code, and reserve `HELIOS_BOARD_*` checks for board-specific exceptions.
Unknown targets default to having no optional sensors.

For example:

```c
#include "helios_features.h"

#if HELIOS_HAS_MAX30100
    /* Start or poll the MAX30100. */
#endif
```

Current capabilities:

| Board | Capability header | MAX30100 | BMI270 |
| --- | --- | --- | --- |
| `sf32lb52-lcd_n16r8` | `src/boards/sf32lb52_lcd_n16r8.h` | `1` | `0` |
| `sf32lb52-lcd_custom_410x502` | `src/boards/sf32lb52_lcd_custom_410x502.h` | `0` | `0` |
| `sf32lb52-lcd_custom_466x466_co5300` (SFWatch v1.0) | `src/boards/sfwatch_v1_0.h` | `0` | `1` |

Declare hardware capabilities as literal `0` or `1` in these headers. SCons
reads the same sensor declarations to omit unused driver sources, including
Bosch's BMI270 API/configuration image. No SDK source edits are needed for this
capability selection.

`helios_features.h` also defines `HELIOS_APP_*` switches for each app registration.
Set them in a board header to override the defaults. The Heart app defaults to
`HELIOS_HAS_MAX30100`; the Fitness entry defaults to `HELIOS_HAS_BMI270` (it is
still a placeholder entry). Other apps retain their enabled defaults. App
switches control registration and relevant app initialization; shared generated
UI code and phone-protocol state remain compiled. Full source exclusion applies
to the sensor drivers.

Use `#if HELIOS_HAS_*` / `#if HELIOS_APP_*`, rather than `#ifdef`, because disabled
features are defined as `0`.

See [SFWatch v1.0](SFWATCH_V1_0.md) for its pinout and BMI270 bring-up details.
