#!/usr/bin/env python3
"""Run BMI270 integration logic against fake RT-Thread and Bosch interfaces."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
variants = {
    "watch": [],
    "gyro_debug": ["-DHELIOS_IMU_DEBUG_GYRO=1"],
    "steps_only": ["-DHELIOS_IMU_RAISE_TO_WAKE=0"],
    "wake_only": ["-DHELIOS_IMU_STEPS=0"],
    "all_optional_off": [f"-DHELIOS_IMU_{name}=0" for name in
                         ("STEPS", "RAISE_TO_WAKE", "DEBUG_ORIENTATION", "DEBUG_GYRO")],
}
with tempfile.TemporaryDirectory(prefix="helios-bmi270-") as temporary:
    for name, defines in variants.items():
        executable = Path(temporary) / name
        subprocess.run([
            "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
            "-DRT_USING_I2C", "-DBSP_USING_I2C2",
            "-DHELIOS_BOARD_SF32LB52_LCD_CUSTOM_466X466_CO5300=1",
            f"-I{test / 'stubs'}", f"-I{root / 'src'}", *defines,
            str(test / "test.c"), "-lm", "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)
        print(f"{name}: passed", flush=True)
