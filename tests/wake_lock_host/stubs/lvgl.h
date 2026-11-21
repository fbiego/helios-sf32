#ifndef HELIOS_WAKE_LOCK_TEST_LVGL_H
#define HELIOS_WAKE_LOCK_TEST_LVGL_H
#include <stdint.h>
#include <stddef.h>
uint32_t lv_tick_get(void);
void lv_display_trigger_activity(void *display);
#endif
