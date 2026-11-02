#include <assert.h>
#include <string.h>
#define HELIOS_BUZZER_PIN 41
#define HELIOS_BUZZER_PAD PAD_PA41
#define HELIOS_BUZZER_GPIO GPIO_A41
#define HELIOS_BUZZER_PWM_FUNCTION GPTIM2_CH2
#define HELIOS_BUZZER_PWM_DEVICE "pwmt2"
#define HELIOS_BUZZER_PWM_CHANNEL 2
#define HELIOS_BUZZER_FREQUENCY_HZ 2000
#define HELIOS_MOTOR_PIN 20
#define HELIOS_MOTOR_PAD PAD_PA20
#define HELIOS_MOTOR_GPIO GPIO_A20
#define HELIOS_MOTOR_ACTIVE_HIGH 1
#define HELIOS_CHARGE_DETECT_PIN 25
#define HELIOS_CHARGE_DETECT_PAD 25
#define HELIOS_CHARGE_DETECT_GPIO 25
#define PIN_PULLUP 1
#include "core/helios_platform.c"

void drv_reboot(void) {}

#if HELIOS_HAS_BUZZER
static struct rt_device_pwm pwm;
static int present = 1, open_error, pwm_error, timer_error;
static int opens, running, levels[42];
rt_tick_t rt_tick_from_millisecond(int32_t ms) { return (rt_tick_t)ms; }
rt_device_t rt_device_find(const char *name)
{ assert(strcmp(name, "pwmt2") == 0); return present ? &pwm : NULL; }
rt_err_t rt_device_open(rt_device_t device, uint16_t flags)
{ assert(device == &pwm && flags == RT_DEVICE_OFLAG_RDWR); opens++; return open_error; }
rt_err_t rt_pwm_set(struct rt_device_pwm *device, int channel, uint32_t period, uint32_t pulse)
{ assert(device == &pwm && channel == 2 && period == 500000 && pulse == 250000); return pwm_error; }
rt_err_t rt_pwm_enable(struct rt_device_pwm *device, int channel)
{ assert(device == &pwm && channel == 2); running = 1; return RT_EOK; }
rt_err_t rt_pwm_disable(struct rt_device_pwm *device, int channel)
{ assert(device == &pwm && channel == 2); running = 0; return RT_EOK; }
void HAL_PIN_Set(int pad, int function, int pull, int cpu)
{ (void)function; (void)pull; assert((pad == 20 || pad == 41 || pad == 25) && cpu == 1); }
void BSP_GPIO_Set(int pin, int value, int cpu) { assert(cpu == 1); levels[pin] = value; }
void rt_pin_write(rt_base_t pin, rt_base_t value) { levels[pin] = (int)value; }
void rt_pin_mode(rt_base_t pin, rt_base_t mode) { (void)pin; (void)mode; }
int rt_pin_read(rt_base_t pin) { return levels[pin]; }
void rt_timer_init(struct rt_timer *timer, const char *name, void (*cb)(void *), void *p,
                   rt_tick_t ticks, uint8_t flags)
{
    (void)name;
    assert(flags & RT_TIMER_FLAG_SOFT_TIMER);
    timer->callback = cb; timer->parameter = p; timer->ticks = ticks; timer->armed = 0;
}
rt_err_t rt_timer_start(struct rt_timer *timer)
{ timer->armed = !timer_error; return timer_error; }
rt_err_t rt_timer_stop(struct rt_timer *timer) { timer->armed = 0; return RT_EOK; }
static void expire(struct rt_timer *timer)
{ assert(timer->armed); timer->armed = 0; timer->callback(timer->parameter); }
#endif

int main(void)
{
#if HELIOS_HAS_BUZZER
    present = 0;
    assert(helios_buzzer_test() == -RT_ERROR && !running);
    present = 1; open_error = -5;
    assert(helios_buzzer_test() == -5 && !running);
    open_error = 0;
    assert(helios_buzzer_test() == RT_EOK && running && g_buzzer_timer.ticks == 200);
    int previous_opens = opens;
    assert(helios_buzzer_test() == RT_EOK && opens == previous_opens);
    expire(&g_buzzer_timer);
    assert(!running && levels[41] == 0);
    pwm_error = -6;
    assert(helios_buzzer_test() == -6 && !running && levels[41] == 0);
    pwm_error = 0; timer_error = -7;
    assert(helios_buzzer_test() == -7 && !running && levels[41] == 0);
    timer_error = 0;
    assert(helios_motor_test() == RT_EOK && levels[20] == 1 && g_motor_timer.ticks == 250);
    assert(helios_motor_test() == RT_EOK && levels[20] == 1);
    expire(&g_motor_timer);
    assert(levels[20] == 0);
    timer_error = -8;
    assert(helios_motor_test() == -8 && levels[20] == 0);
    levels[25] = 0; assert(helios_charge_detect_level() == 0);
    levels[25] = 1; assert(helios_charge_detect_level() == 1);
#else
    assert(helios_buzzer_test() == -RT_ENOSYS);
    assert(helios_motor_test() == -RT_ENOSYS);
    assert(helios_charge_detect_level() == -RT_ENOSYS);
#endif
    return 0;
}
