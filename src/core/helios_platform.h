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
void helios_platform_restart(void); /* Thread context; reboot both CPU cores. */
rt_err_t helios_battery_read(helios_battery_t *battery);
int helios_charge_detect_level(void); /* Raw 0/1, negative if unavailable. */
rt_err_t helios_buzzer_test(void); /* Short, non-blocking diagnostic tone. */
rt_err_t helios_buzzer_tone(uint32_t duration_ms); /* Thread context; non-blocking 4 kHz tone. */
void helios_buzzer_silence(void); /* Thread context; stop the current tone immediately. */
rt_err_t helios_motor_test(void);  /* Short, non-blocking diagnostic pulse. */
rt_err_t helios_motor_pulse(uint32_t duration_ms);
void helios_motor_silence(void);
rt_device_t helios_storage_open_root(void);
struct rt_i2c_bus_device *helios_i2c_bus(const char *name, uint32_t hz);
struct rt_spi_device *helios_spi_device(const char *name);
uint8_t helios_button_pressed(uint8_t id);
uint8_t helios_button_take_press(uint8_t id);
uint32_t helios_button_press_sequence(void); /* Non-consuming count across all buttons. */

#endif
