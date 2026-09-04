#ifndef HELIOS_IOS_H
#define HELIOS_IOS_H

#include <stdint.h>

void helios_ios_on_connected(uint8_t conn_idx);
void helios_ios_on_disconnected(uint8_t conn_idx);
void helios_ios_on_bonded(uint8_t conn_idx);
uint8_t helios_ios_music_command(const char *action);
uint8_t helios_ios_ams_ready(void);
uint8_t helios_ios_ancs_ready(void);

#endif
