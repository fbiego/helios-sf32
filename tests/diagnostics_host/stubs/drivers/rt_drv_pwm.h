#include <rtdevice.h>
struct rt_device_pwm { int unused; };
rt_err_t rt_pwm_set(struct rt_device_pwm *, int, uint32_t, uint32_t);
rt_err_t rt_pwm_enable(struct rt_device_pwm *, int);
rt_err_t rt_pwm_disable(struct rt_device_pwm *, int);
