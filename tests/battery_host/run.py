#!/usr/bin/env python3
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-battery-") as temporary:
    charge_flags = ["-DRT_USING_ADC", "-DHELIOS_HAS_CHARGE_DETECT=1",
                    "-DHELIOS_CHARGE_DETECT_PIN=25", "-DHELIOS_CHARGE_DETECT_PAD=25",
                    "-DHELIOS_CHARGE_DETECT_GPIO=25"]
    for name, flags in [("adc_enabled", ["-DRT_USING_ADC"]), ("adc_disabled", []),
                        ("charge_active_low", charge_flags),
                        ("charge_active_high", charge_flags + ["-DHELIOS_CHARGE_DETECT_ACTIVE_LEVEL=1"])]:
        executable = Path(temporary) / name
        subprocess.run([
            "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
            f"-I{test / 'stubs'}", f"-I{root / 'tests/bmi270_host/stubs'}",
            f"-I{root / 'src'}", *flags,
            str(test / "test.c"), "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)
        print(f"{name}: passed", flush=True)
