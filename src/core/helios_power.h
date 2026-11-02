#ifndef HELIOS_POWER_H
#define HELIOS_POWER_H

#include <rtthread.h>
#include <stdint.h>

/* UI-thread only. Hold the CPU awake during UI work; permit automatic SDK
 * DeepSleep after the display is safely stopped. BLE remains managed by SDK. */
void helios_power_init(void);
rt_err_t helios_power_set_display(uint8_t on);

#endif
