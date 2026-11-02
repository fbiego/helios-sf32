#ifndef DIAGNOSTICS_TEST_RTDEVICE_H
#define DIAGNOSTICS_TEST_RTDEVICE_H
#include "../../battery_host/stubs/rtdevice.h"
#define RT_DEVICE_OFLAG_RDWR 3
#define PIN_MODE_OUTPUT 1
#define PIN_MODE_INPUT_PULLUP 2
rt_err_t rt_device_open(rt_device_t, uint16_t);
void rt_pin_write(rt_base_t, rt_base_t);
int rt_pin_read(rt_base_t);
#endif
