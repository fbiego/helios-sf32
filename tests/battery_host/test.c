#include <assert.h>
#include <string.h>
#include "core/helios_platform.c"

static unsigned reboots;
void drv_reboot(void) { reboots++; }

#if HELIOS_HAS_CHARGE_DETECT
static int charge_level = 1;
int rt_pin_read(rt_base_t pin) { assert(pin == 25); return charge_level; }
void rt_pin_mode(rt_base_t pin, rt_base_t mode) { (void)pin; (void)mode; }
void HAL_PIN_Set(int pad, int gpio, int pull, int enabled)
{ (void)pad; (void)gpio; (void)pull; (void)enabled; }
#endif

#ifdef RT_USING_ADC
static int present = 1, locked, enables, converts, disables;
static rt_err_t enable_error, convert_error, disable_error;
static uint32_t measured;
static struct rt_adc_device adc;

rt_err_t rt_mutex_init(struct rt_mutex *mutex, const char *name, uint8_t flag)
{ (void)mutex; assert(strcmp(name, "battery") == 0 && flag == RT_IPC_FLAG_PRIO); return RT_EOK; }
rt_err_t rt_mutex_take(struct rt_mutex *mutex, int32_t timeout)
{ (void)mutex; assert(timeout == RT_WAITING_FOREVER && !locked); locked = 1; return RT_EOK; }
rt_err_t rt_mutex_release(struct rt_mutex *mutex)
{ (void)mutex; assert(locked); locked = 0; return RT_EOK; }
rt_device_t rt_device_find(const char *name)
{ assert(strcmp(name, "bat1") == 0); return present ? &adc : RT_NULL; }
rt_err_t rt_adc_enable(rt_adc_device_t device, rt_uint32_t channel)
{ assert(locked && device == &adc && channel == 7); enables++; return enable_error; }
rt_err_t rt_adc_disable(rt_adc_device_t device, rt_uint32_t channel)
{ assert(locked && device == &adc && channel == 7); disables++; return disable_error; }
static rt_err_t convert(rt_adc_device_t device, rt_uint32_t channel, rt_uint32_t *value)
{
    assert(locked && device == &adc && channel == 7);
    converts++;
    *value = measured;
    return convert_error;
}
static const struct rt_adc_ops ops = {.convert = convert};
#endif

int main(void)
{
    helios_platform_restart();
    assert(reboots == 1);
    helios_battery_t battery;
    assert(helios_battery_read(NULL) == -RT_EINVAL);
#ifdef RT_USING_ADC
    assert(helios_battery_lock_init() == RT_EOK);
    adc.ops = &ops;
    present = 0;
    memset(&battery, 0xff, sizeof(battery));
    assert(helios_battery_read(&battery) == -RT_ERROR);
    assert(!locked && !battery.millivolts && !battery.percent);
    present = 1;

    const struct { uint32_t raw, mv; uint8_t percent; } cases[] = {
        {33000, 3300, 0}, {39000, 3900, 66}, {42000, 4200, 100},
        {31000, 3100, 0}, {43500, 4350, 100}, {35555, 3555, 28},
    };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        measured = cases[i].raw;
        assert(helios_battery_read(&battery) == RT_EOK);
        assert(battery.millivolts == cases[i].mv && battery.percent == cases[i].percent);
        assert(!locked && enables == converts && enables == disables);
    }

#if HELIOS_HAS_CHARGE_DETECT
    measured = 39000;
    for (charge_level = 0; charge_level <= 1; charge_level++) {
        assert(helios_battery_read(&battery) == RT_EOK);
        assert(battery.charging == (charge_level == HELIOS_CHARGE_DETECT_ACTIVE_LEVEL));
    }
#else
    assert(battery.charging == 0);
#endif

    /* A conversion error must not publish the driver's invalid voltage value. */
    convert_error = -6;
    measured = 50000;
    assert(helios_battery_read(&battery) == -6);
    assert(!locked && !battery.millivolts && !battery.percent && converts == disables);
    convert_error = RT_EOK;
    assert(helios_battery_read(&battery) == -RT_ERROR); /* 5 V invalid sentinel. */
    measured = 0;
    assert(helios_battery_read(&battery) == -RT_ERROR);
    measured = UINT32_MAX;
    assert(helios_battery_read(&battery) == -RT_ERROR);

    int previous_converts = converts, previous_disables = disables;
    enable_error = -7;
    assert(helios_battery_read(&battery) == -7);
    assert(!locked && converts == previous_converts && disables == previous_disables);
    enable_error = RT_EOK;
    measured = 39000;
    disable_error = -8;
    assert(helios_battery_read(&battery) == -8 && !locked && !battery.millivolts);
    disable_error = RT_EOK;
    adc.ops = NULL;
    assert(helios_battery_read(&battery) == -RT_ENOSYS && !locked);
#else
    assert(helios_battery_read(&battery) == -RT_ENOSYS);
    assert(!battery.millivolts && !battery.percent);
#endif
    return 0;
}
