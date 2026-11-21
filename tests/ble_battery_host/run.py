from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
sdk = Path(os.environ.get("SIFLI_SDK", "/Users/felix/sifli/SiFli-SDK/v2.5"))
with tempfile.TemporaryDirectory(prefix="helios-ble-battery-") as temporary:
    executable = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
        f"-I{test / 'stubs'}", f"-I{root / 'tests/bmi270_host/stubs'}",
        f"-I{root / 'src'}", f"-I{root / 'src/chronos_core'}",
        f"-I{sdk / 'external/cJSON-1.7.17'}",
        str(test / "test.c"), str(root / "src/chronos_core/chronos_core.c"),
        "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
    print("BLE battery subscription, request, update, failure and reconnect: passed")
