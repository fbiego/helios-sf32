#ifndef HELIOS_APP_H
#define HELIOS_APP_H

#include <stdbool.h>
#include <stdint.h>

#include "chronos_core.h"

void helios_ui_set_ble(uint8_t connected);
void helios_ui_set_battery(uint8_t percent, uint32_t millivolts);

void helios_app_chronos_init(void);
void helios_app_chronos_connection(bool connected);
void helios_app_chronos_notification(const chronos_notification_t *notification);
void helios_app_chronos_ringer(const char *caller, bool active);
void helios_app_chronos_config(chronos_config_t config, uint32_t a, uint32_t b);
void helios_app_music_update(const chronos_music_info_t *music);

void helios_screen_wake(void);
void helios_screen_set_timeout_ms(uint32_t timeout_ms);
uint32_t helios_screen_get_timeout_ms(void);
void helios_screen_set_awake_override(uint8_t enabled);
uint8_t helios_screen_awake_override(void);
uint8_t helios_screen_is_on(void);

#endif
