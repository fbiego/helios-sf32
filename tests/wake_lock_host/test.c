#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "core/helios_screen_wake_lock.h"

static uint32_t now, idle_started;
static unsigned wakes, resets;
uint32_t lv_tick_get(void) { return now; }
void lv_display_trigger_activity(void *display)
{ assert(!display); idle_started = now; resets++; }
void helios_screen_wake(void) { wakes++; }

int main(void)
{
    static char owner[18];
    assert(!helios_screen_wake_lock_active());
    assert(!helios_screen_wake_lock_acquire(NULL, 0));
    assert(helios_screen_wake_lock_acquire(&owner[0], 0));
    assert(helios_screen_wake_lock_acquire(&owner[1], 1000));
    assert(wakes == 2);
    now = 1000;
    assert(helios_screen_wake_lock_active()); /* One expiry cannot release another app. */
    assert(resets == 0);
    now = 5000;
    helios_screen_wake_lock_release(&owner[0]);
    assert(!helios_screen_wake_lock_active() && idle_started == 5000);
    unsigned before = resets;
    helios_screen_wake_lock_release(&owner[0]); /* Unload, then delete: safe twice. */
    helios_screen_wake_lock_release(NULL);
    helios_screen_wake_lock_release(&owner[17]);
    assert(resets == before);

    assert(helios_screen_wake_lock_acquire(&owner[0], 1000));
    now = 5500;
    assert(helios_screen_wake_lock_acquire(&owner[0], 2000)); /* Renew, no nested reference. */
    now = 7499; assert(helios_screen_wake_lock_active());
    now = 7500; assert(!helios_screen_wake_lock_active() && idle_started == 7500);
    assert(helios_screen_wake_lock_acquire(&owner[0], 100));
    assert(helios_screen_wake_lock_acquire(&owner[0], 0)); /* Upgrade to indefinite. */
    now += 10000; assert(helios_screen_wake_lock_active());
    helios_screen_wake_lock_release(&owner[0]);

    now = UINT32_MAX - 49;
    assert(helios_screen_wake_lock_acquire(&owner[0], 100));
    now = 49; assert(helios_screen_wake_lock_active());
    now = 50; assert(!helios_screen_wake_lock_active()); /* Tick wrap. */

    for (unsigned i = 0; i < 16; i++) assert(helios_screen_wake_lock_acquire(&owner[i], 100));
    assert(!helios_screen_wake_lock_acquire(&owner[16], 0));
    now += 100;
    assert(helios_screen_wake_lock_acquire(&owner[16], 0)); /* Expired capacity reused. */
    helios_screen_wake_lock_release(&owner[16]);
    assert(!helios_screen_wake_lock_active());
    puts("Wake locks: multiple owners, duplicate lifecycle events, renewal, expiry, capacity and tick wrap passed");
    return 0;
}
