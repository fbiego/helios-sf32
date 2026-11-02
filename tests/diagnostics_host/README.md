# Diagnostic output host tests

```sh
python3 tests/diagnostics_host/run.py
```

Compiles the real platform adapter against fake RT-Thread GPIO, PWM, and timer
interfaces. Tests cover a 2 kHz/50% buzzer tone, repeated button presses, 200 ms
buzzer and 250 ms motor auto-off, missing devices, PWM/open/timer failures, raw
charge-detect levels, and builds without the output capabilities.

Physical sound, vibration, pin routing, and drive polarity still need checking
on the board. The shared basic type stubs are in the BMI270 and battery host tests.
