#ifndef HELIOS_SCREEN_WAKE_LOCK_H
#define HELIOS_SCREEN_WAKE_LOCK_H

#include <stdbool.h>
#include <stdint.h>

/* UI-thread only. owner must be a non-NULL, unique identity (e.g. the screen
 * object or a static token). At most 16 owners; false means no lock acquired.
 * Acquisition wakes the screen. timeout_ms=0 holds until release; acquiring
 * again for the same owner renews/replaces its duration without nesting.
 * A lock prevents idle timeout; the sleep button can still turn the screen off.
 * Release on screen unload/delete, before an owner identity can be reused. */
bool helios_screen_wake_lock_acquire(const void *owner, uint32_t timeout_ms);
void helios_screen_wake_lock_release(const void *owner);
/* Also expires timed locks. Last release/expiry starts a fresh idle timeout. */
bool helios_screen_wake_lock_active(void);

#endif
