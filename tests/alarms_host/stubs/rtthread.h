#ifndef ALARMS_TEST_RTTHREAD_H
#define ALARMS_TEST_RTTHREAD_H
#include <stdint.h>
#include <string.h>
#define rt_memcpy memcpy
#define rt_memset memset
typedef uint32_t rt_tick_t;
typedef intptr_t rt_base_t;
rt_tick_t rt_tick_get(void);
rt_tick_t rt_tick_from_millisecond(int32_t ms);
#endif
