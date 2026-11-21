#ifndef HELIOS_UI_DISPATCH_H
#define HELIOS_UI_DISPATCH_H

#include <stdbool.h>

typedef void (*helios_ui_work_t)(void *);

/* Thread-safe, allocation-free handoff. Repeated queued callbacks coalesce.
 * Callbacks receive NULL and must snapshot their own shared payload safely. */
bool helios_ui_dispatch_post(helios_ui_work_t callback);
/* UI thread only. Runs at most the work queued when this call began. */
void helios_ui_dispatch_poll(void);

#endif
