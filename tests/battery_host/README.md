# Battery adapter host tests

```sh
python3 tests/battery_host/run.py
```

The tests compile the real platform adapter with fake RT-Thread/ADC interfaces.
They verify internal channel 7 selection, 0.1 mV conversion, percentage bounds,
missing-device and driver failures, invalid voltage rejection, mutex release,
and ADC-disabled behavior. Shared basic stubs live in `tests/bmi270_host/stubs`.
Binaries are built in a temporary directory.

These tests do not replace measuring VBAT on the board or validating calibration.
