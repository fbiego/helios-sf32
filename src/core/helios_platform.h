#ifndef HELIOS_PLATFORM_H
#define HELIOS_PLATFORM_H

#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>

typedef struct
{
    uint8_t percent;
    uint32_t millivolts;
    uint8_t charging;
} helios_battery_t;

void helios_platform_init(void);
rt_err_t helios_battery_read(helios_battery_t *battery);
rt_device_t helios_storage_open_root(void);
struct rt_i2c_bus_device *helios_i2c_bus(const char *name, uint32_t hz);
struct rt_spi_device *helios_spi_device(const char *name);
uint8_t helios_button_pressed(uint8_t id);
uint8_t helios_button_take_press(uint8_t id);

#endif
