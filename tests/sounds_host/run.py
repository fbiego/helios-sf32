from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory(prefix="helios-sounds-") as temporary:
    executable = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
        f"-I{root / 'tests/time_host/stubs'}",
        f"-I{root / 'tests/storage_host/stubs'}",
        f"-I{root / 'src'}", str(Path(__file__).with_name("test.c")),
        "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
