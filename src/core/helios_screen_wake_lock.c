#include "helios_screen_wake_lock.h"
#include "lvgl.h"

void helios_screen_wake(void);

#define HELIOS_SCREEN_WAKE_LOCK_MAX 16

static struct {
    const void *owner;
    uint32_t started_ms, timeout_ms;
} locks[HELIOS_SCREEN_WAKE_LOCK_MAX];

bool helios_screen_wake_lock_active(void)
{
    bool expired = false, active = false;
    uint32_t now = lv_tick_get();
    for (unsigned i = 0; i < HELIOS_SCREEN_WAKE_LOCK_MAX; i++) {
        if (!locks[i].owner) continue;
        if (locks[i].timeout_ms && (uint32_t)(now - locks[i].started_ms) >= locks[i].timeout_ms) {
            locks[i].owner = NULL;
            expired = true;
        } else {
            active = true;
        }
    }
    if (expired && !active) lv_display_trigger_activity(NULL);
    return active;
}

bool helios_screen_wake_lock_acquire(const void *owner, uint32_t timeout_ms)
{
    if (!owner) return false;
    helios_screen_wake_lock_active();
    int slot = -1;
    for (unsigned i = 0; i < HELIOS_SCREEN_WAKE_LOCK_MAX; i++) {
        if (locks[i].owner == owner) { slot = (int)i; break; }
        if (!locks[i].owner && slot < 0) slot = (int)i;
    }
    if (slot < 0) return false;
    locks[slot].owner = owner;
    locks[slot].started_ms = lv_tick_get();
    locks[slot].timeout_ms = timeout_ms;
    helios_screen_wake();
    return true;
}

void helios_screen_wake_lock_release(const void *owner)
{
    if (!owner) return;
    for (unsigned i = 0; i < HELIOS_SCREEN_WAKE_LOCK_MAX; i++) {
        if (locks[i].owner != owner) continue;
        locks[i].owner = NULL;
        if (!helios_screen_wake_lock_active()) lv_display_trigger_activity(NULL);
        return;
    }
}
