#ifndef HELIOS_CHRONOS_H
#define HELIOS_CHRONOS_H

#include <stdint.h>

void helios_chronos_on_rx(const uint8_t *data, uint16_t len);
void helios_chronos_prepare(void);
void helios_chronos_on_connected(void);
void helios_chronos_on_disconnected(void);
void helios_chronos_on_subscribed(void);
void helios_chronos_send_music_action(const char *action);
void helios_chronos_update_battery(uint8_t percent, uint8_t charging);
void helios_chronos_update_activity(uint32_t steps, uint32_t calories);

#endif
