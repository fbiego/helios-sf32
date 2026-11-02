#ifndef TEST_RTDEVICE_H
#define TEST_RTDEVICE_H
#include "rtthread.h"
struct rt_i2c_bus_device { int unused; };
struct rt_i2c_msg {
    rt_uint16_t addr, mem_addr, mem_addr_size, flags, len;
    uint8_t *buf;
};
#define RT_I2C_WR 0
#define RT_I2C_RD 1
#define RT_I2C_MEM_ACCESS 0x10
#define PIN_MODE_INPUT 0
#define PIN_IRQ_MODE_RISING 0
#define PIN_IRQ_ENABLE 1
rt_size_t rt_i2c_transfer(struct rt_i2c_bus_device *, struct rt_i2c_msg *, uint32_t);
void rt_pin_mode(rt_base_t, rt_base_t);
rt_err_t rt_pin_attach_irq(rt_int32_t, rt_uint32_t, void (*)(void *), void *);
rt_err_t rt_pin_irq_enable(rt_base_t, rt_uint32_t);
rt_err_t rt_pin_detach_irq(rt_int32_t);
#endif
