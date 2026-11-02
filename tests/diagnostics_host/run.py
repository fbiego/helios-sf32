#!/usr/bin/env python3
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-diagnostics-") as temporary:
    for enabled in (1, 0):
        executable = Path(temporary) / f"outputs_{enabled}"
        subprocess.run([
            "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
            f"-I{test / 'stubs'}", f"-I{root / 'tests/bmi270_host/stubs'}",
            f"-I{root / 'src'}", "-DRT_USING_PWM", "-DRT_USING_TIMER_SOFT",
            f"-DHELIOS_HAS_BUZZER={enabled}", f"-DHELIOS_HAS_MOTOR={enabled}",
            f"-DHELIOS_HAS_CHARGE_DETECT={enabled}",
            str(test / "test.c"), "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)
        print(f"diagnostic outputs {'enabled' if enabled else 'disabled'}: passed", flush=True)
