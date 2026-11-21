# iOS pairing and subscription lifecycle

## Long-running notification and media updates

BLE callbacks copy incoming data into bounded firmware buffers and post a
callback to `helios_ui_dispatch`. This fixed-size queue does not allocate or
call LVGL. The UI loop drains it before running LVGL timers, including while
the display is off. Repeated work coalesces; a notification burst processes
at most four queued entries per callback so other UI work can proceed.

Previously the BLE thread called `lv_async_call()`. In the bundled LVGL v9,
that function allocates an LVGL timer and modifies the global timer list
immediately without locking. The callback runs later, but scheduling it from
BLE could race the UI thread's timer traversal. That is a concrete corruption
risk consistent with an intermittent fault after many updates, though a fault
trace is still needed to attribute a particular crash.

The same handoff covers Chronos updates and phone battery/device information;
their subject setters also run on the UI thread.

`python3 tests/ui_dispatch_host/run.py` checks queue saturation, coalescing,
FIFO order, concurrent publishers and bounded self-rescheduling under memory
sanitizers. This validates the handoff, not an hours-long hardware soak test.

## Pairing

ANCS discovery starts on connection. AMS discovery waits for successful
pairing or encryption: SDK v2.5 leaves its AMS client in SEARCHING after a
failed search, so searching before the iPhone exposes the service can leave
the client unable to retry. ANCS can retry a failed pre-pairing search.

Discovery uses disabled CCCDs. Once security and discovery are complete,
the firmware enables each profile's subscriptions once per connection.
Pairing-success and encryption events share the same guard. Disconnect
clears the guard, including when the next connection reuses the same index.

Previously, both security events could disable and immediately re-enable
both profiles. This produced repeated GATT writes during pairing. The SDK's
ANCS CCCD writer asserts if its writes encounter transmit flow control or
allocation failure. Removing the repeated writes addresses that risk; it
does not establish the cause of a particular hard fault without its trace.

The bridge checks fixed event sizes, ANCS response connection indices and
AMS inline value lengths before using them. ANCS attribute contents are
out-of-line pointers owned by the SDK; the outer event length does not
describe their buffer size, so validation of that buffer remains in the SDK.

Useful serial messages:

- `iOS secured conn_idx=...`: first successful security event.
- `ANCS ready result=0` / `AMS ready result=0`: profile discovery succeeded.
- `ANCS secure subscribe ret=0` / `AMS secure subscribe ret=0`: subscriptions requested.
- `AMS pending result=0`: SDK completed a deferred subscription operation.

For a crash, capture the serial output before and through the fault, including
PC, LR, fault-status registers and the backtrace if present. Keep the matching
build's `main.elf`; addresses must be resolved against that exact image.
Check initial pairing, reconnect with an existing bond, receipt of a new
notification and Now Playing updates, including with the display off.

`python3 tests/ios_host/run.py` tests event ordering, duplicate security events,
reconnect, discovery retries, pending AMS subscriptions and malformed events
with the SDK's real profile structures and Address/UndefinedBehavior sanitizers.
These host tests mock the transport; physical pairing still needs hardware
verification.
