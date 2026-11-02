from pathlib import Path
import os
import subprocess
import tempfile
root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
sdk = Path(os.environ.get("SIFLI_SDK", "/Users/felix/sifli/SiFli-SDK/v2.5"))
cjson = sdk / "external/cJSON-1.7.17"
with tempfile.TemporaryDirectory(prefix="helios-storage-") as temporary:
    exe = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra", "-Werror",
        "-DPKG_USING_FLASHDB", f'-DHELIOS_STORAGE_ROOT="{temporary}/data"',
        f"-I{test / 'stubs'}", f"-I{root / 'src'}", f"-I{cjson}",
        str(test / "test.c"), str(root / "src/core/helios_user.c"), str(cjson / "cJSON.c"), "-lm", "-o", str(exe)
    ], check=True)
    subprocess.run([str(exe)], check=True, env={**os.environ, "TZ": "UTC"})
    ui_exe = Path(temporary) / "test_ui"
    subprocess.run([
        "cc", "-std=c99", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra", "-Werror",
        "-include", str(test / "ui_support.h"),
        f"-I{test / 'stubs'}", f"-I{test}", f"-I{root / 'src'}", f"-I{root / 'src/chronos_core'}", f"-I{cjson}",
        str(test / "test_ui.c"), str(test / "ui_support.c"),
        str(root / "src/core/helios_user.c"),
        str(root / "src/chronos_core/chronos_core.c"),
        *[str(root / f"src/helios_ui/custom/apps/{name}/{name}.c") for name in ("notifications", "contacts", "weather")],
        str(cjson / "cJSON.c"), "-lm", "-o", str(ui_exe)
    ], check=True)
    subprocess.run([str(ui_exe)], check=True, env={**os.environ, "TZ": "UTC"})
