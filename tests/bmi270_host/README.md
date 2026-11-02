# BMI270 adapter host tests

Run from the repository root:

```sh
python3 tests/bmi270_host/run.py
```

Requires a C compiler and Python 3. Binaries are built in a temporary directory.
The tests compile the real Helios adapter against fake RT-Thread interfaces and
Bosch API calls. They check address probing, init failure propagation, feature
selection, INT1 mapping, ISR isolation from I2C, startup cleanup, one-shot wake
consumption, debounce, expired gestures, tick wraparound, and gravity tilt angles.
They also check diagnostics-driven gyro requests, I2C memory-transfer framing,
firmware-upload error reporting, missing buses/devices, unexpected chip IDs,
retry recovery, and preservation of the underlying error in the read API.

Variants cover default SFWatch features, gyro debugging, steps only, wrist wake
only, and all optional IMU features disabled. These tests do not emulate Bosch's
feature engine or validate physical wiring, mounting, or real wrist gestures.
