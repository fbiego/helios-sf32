from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-wake-lock-") as temporary:
    executable = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
        f"-I{test / 'stubs'}", f"-I{root / 'src'}",
        str(test / "test.c"), str(root / "src/core/helios_screen_wake_lock.c"),
        "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
