# Screen wake locks

Header: `core/helios_screen_wake_lock.h` (also included by `core/helios_app.h`).
These functions are **UI-thread only**, unlike the locked collection APIs.

```c
bool helios_screen_wake_lock_acquire(const void *owner, uint32_t timeout_ms);
void helios_screen_wake_lock_release(const void *owner);
bool helios_screen_wake_lock_active(void);
```

Use a unique non-NULL owner, such as the screen object or a static token. Pass
`0` to hold until released, or a duration in milliseconds for a timed lock.
Acquiring wakes the display and prevents automatic screen timeout. Multiple
owners are independent; acquiring again for the same owner renews its duration.
Up to 16 owners are supported; acquisition returns `false` for a NULL owner or
when all slots are occupied. Releasing an unknown/already-released owner is safe.
After the final lock releases or expires, the normal configured idle timeout
starts afresh. The physical sleep button still works, and wake locks do not
change the saved screen timeout or the existing global awake override.

For a screen callback:

```c
lv_obj_t *screen = lv_event_get_current_target(e);
switch (lv_event_get_code(e)) {
case LV_EVENT_SCREEN_LOADED:
    helios_screen_wake_lock_acquire(screen, 0); // Or 120000 for two minutes.
    break;
case LV_EVENT_SCREEN_UNLOAD_START:
case LV_EVENT_DELETE:
    helios_screen_wake_lock_release(screen);
    break;
default:
    break;
}
```

Release before an owner identity can be reused. Diagnostics uses an indefinite
lock for its visible lifetime on every board, releasing on unload or deletion.

