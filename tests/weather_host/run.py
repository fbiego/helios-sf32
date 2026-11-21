from pathlib import Path
import os
import subprocess
import tempfile
root = Path(__file__).resolve().parents[2]
test = Path(__file__).resolve().parent
support = root / "tests/storage_host"
sdk = Path(os.environ.get("SIFLI_SDK", "/Users/felix/sifli/SiFli-SDK/v2.5"))
cjson = sdk / "external/cJSON-1.7.17"
with tempfile.TemporaryDirectory(prefix="helios-weather-") as temporary:
    exe = Path(temporary) / "test"
    subprocess.run([
        "cc", "-std=c99", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra", "-Werror",
        "-include", str(support / "ui_support.h"),
        f"-I{support / 'stubs'}", f"-I{support}", f"-I{root / 'src'}",
        f"-I{root / 'src/chronos_core'}", f"-I{cjson}",
        str(test / "test.c"), str(support / "ui_support.c"),
        str(root / "src/core/helios_weather_data.c"), str(root / "src/core/helios_weather_ui.c"),
        str(root / "src/chronos_core/chronos_core.c"),
        str(root / "src/helios_ui/custom/apps/weather/weather.c"),
        str(cjson / "cJSON.c"), "-o", str(exe)
    ], check=True)
    subprocess.run([str(exe)], check=True, env={**os.environ, "TZ": "UTC"})
