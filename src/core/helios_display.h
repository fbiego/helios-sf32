#ifndef HELIOS_DISPLAY_H
#define HELIOS_DISPLAY_H

#include <rtthread.h>

/* Apply the board's physical display orientation after littlevgl2rtt_init(),
 * before creating any screens. Touch uses the inverse LVGL display transform. */
rt_err_t helios_display_init_rotation(void);

#endif
