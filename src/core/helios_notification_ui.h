#ifndef HELIOS_NOTIFICATION_UI_H
#define HELIOS_NOTIFICATION_UI_H

#include <stdbool.h>
#include <stdint.h>

/* These functions run on the UI thread. */
void helios_notification_preview_reset(void);
void helios_notification_preview_add(uint32_t notification_id);
bool helios_notification_preview_show(void);
bool helios_notification_preview_is_open(void);
void helios_notification_preview_interact(void);
/* Returns true while an untouched preview still has time left. */
bool helios_notification_preview_poll(void);
void helios_notification_preview_next(void);
void helios_notification_preview_dismiss(void);
void helios_notification_list_mark_visible_read(void);

#endif
