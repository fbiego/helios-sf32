from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="helios-ui-dispatch-") as temporary:
    executable = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-pthread",
        "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
        f"-I{test / 'stubs'}", f"-I{root / 'src/core'}",
        str(test / "test.c"), str(root / "src/core/helios_ui_dispatch.c"),
        "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
