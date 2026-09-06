#ifndef HELIOS_PREFS_H
#define HELIOS_PREFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool helios_prefs_init(void);
bool helios_prefs_ready(void);

bool helios_prefs_get_i32(const char *key, int32_t *value, int32_t fallback);
bool helios_prefs_set_i32(const char *key, int32_t value);

bool helios_prefs_get_u32(const char *key, uint32_t *value, uint32_t fallback);
bool helios_prefs_set_u32(const char *key, uint32_t value);

bool helios_prefs_get_bool(const char *key, bool *value, bool fallback);
bool helios_prefs_set_bool(const char *key, bool value);

bool helios_prefs_get_str(const char *key, char *buf, size_t len, const char *fallback);
bool helios_prefs_set_str(const char *key, const char *value);
bool helios_prefs_del(const char *key);

void helios_prefs_apply_saved_ui_settings(void);
void helios_prefs_save_screen_brightness(int32_t value);
void helios_prefs_save_screen_timeout(int32_t value);

#endif
