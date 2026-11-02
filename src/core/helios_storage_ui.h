#ifndef HELIOS_STORAGE_UI_H
#define HELIOS_STORAGE_UI_H
#include <stdint.h>
void helios_storage_ui_init(void);
void helios_storage_ui_poll(void);
/* Called from the UI thread after queued phone updates. */
void helios_storage_ui_sync_contacts(void);
void helios_storage_ui_sync_qr(uint32_t index);
void helios_storage_ui_sync_user(uint32_t a, uint32_t b);
#endif
