from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-battery-ui-") as temporary:
    executable = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
        "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
        f"-I{test / 'stubs'}", f"-I{root / 'tests/storage_host/stubs'}",
        f"-I{root / 'src'}", str(test / "test.c"), "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
    print("battery chart: async loading, deletion/reopen, cached render, missing data, refresh and allocation failures: passed")
