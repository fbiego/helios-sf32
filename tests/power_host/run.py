from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-power-") as temporary:
    for enabled in (1, 0):
        executable = Path(temporary) / f"power_{enabled}"
        subprocess.run([
            "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
            f"-I{test / 'stubs'}", f"-I{root / 'tests/bmi270_host/stubs'}",
            f"-I{root / 'src'}", f"-DHELIOS_HAS_LOW_POWER={enabled}",
            "-DHELIOS_DISPLAY_ENABLE_PIN=26", "-DBSP_USING_PM", "-DRT_USING_PM",
            "-DPM_DEEP_ENABLE", str(test / "test.c"), "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)
        print(f"power management {'enabled' if enabled else 'disabled'}: passed", flush=True)
