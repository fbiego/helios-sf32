#include "../../bmi270_host/stubs/rtdevice.h"
struct rt_adc_device;
struct rt_adc_ops {
    rt_err_t (*convert)(struct rt_adc_device *, rt_uint32_t, rt_uint32_t *);
};
struct rt_adc_device { const struct rt_adc_ops *ops; };
typedef struct rt_adc_device *rt_adc_device_t;
rt_device_t rt_device_find(const char *);
rt_err_t rt_adc_enable(rt_adc_device_t, rt_uint32_t);
rt_err_t rt_adc_disable(rt_adc_device_t, rt_uint32_t);
#define PIN_MODE_INPUT_PULLUP 2
int rt_pin_read(rt_base_t);
