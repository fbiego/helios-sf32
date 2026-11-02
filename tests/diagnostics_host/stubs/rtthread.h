#ifndef DIAGNOSTICS_TEST_RTTHREAD_H
#define DIAGNOSTICS_TEST_RTTHREAD_H
#include "../../battery_host/stubs/rtthread.h"
#define RT_TIMER_FLAG_ONE_SHOT 0
#define RT_TIMER_FLAG_SOFT_TIMER 4
struct rt_timer {
    void (*callback)(void *);
    void *parameter;
    rt_tick_t ticks;
    int armed;
};
void rt_timer_init(struct rt_timer *, const char *, void (*)(void *), void *, rt_tick_t, uint8_t);
rt_err_t rt_timer_start(struct rt_timer *);
rt_err_t rt_timer_stop(struct rt_timer *);
#endif
