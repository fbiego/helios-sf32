from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
sdk = Path(os.environ.get("SIFLI_SDK", "/Users/felix/sifli/SiFli-SDK/v2.5"))
cjson = sdk / "external/cJSON-1.7.17"
with tempfile.TemporaryDirectory(prefix="helios-alarms-") as temporary:
    executable = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra", "-Werror",
        "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
        f"-I{test / 'stubs'}", f"-I{root / 'tests/ble_battery_host/stubs'}",
        f"-I{root / 'src'}", f"-I{root / 'src/chronos_core'}", f"-I{cjson}",
        str(test / "test.c"), str(root / "src/chronos_core/chronos_core.c"),
        str(cjson / "cJSON.c"), "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
