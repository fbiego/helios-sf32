#!/usr/bin/env python3
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-display-") as temporary:
    for angle in (90, 0):
        executable = Path(temporary) / f"display_{angle}"
        subprocess.run([
            "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
            f"-I{test / 'stubs'}", f"-I{root / 'src'}",
            f"-DHELIOS_DISPLAY_ROTATION_CW={angle}",
            str(test / "test.c"), "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)
        print(f"display rotation {angle}: passed", flush=True)
