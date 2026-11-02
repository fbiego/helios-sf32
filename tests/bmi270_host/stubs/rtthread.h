#ifndef TEST_RTTHREAD_H
#define TEST_RTTHREAD_H
#include <stddef.h>
#include <stdint.h>
typedef int rt_err_t;
typedef intptr_t rt_base_t;
typedef uint32_t rt_tick_t;
typedef uint16_t rt_uint16_t;
typedef uint32_t rt_uint32_t;
typedef int32_t rt_int32_t;
typedef size_t rt_size_t;
typedef void *rt_thread_t;
typedef void *rt_device_t;
struct rt_semaphore { int unused; };
#define RT_NULL NULL
#define RT_EOK 0
#define RT_ERROR 1
#define RT_EINVAL 10
#define RT_ENOMEM 5
#define RT_IPC_FLAG_FIFO 0
rt_tick_t rt_tick_get(void);
rt_tick_t rt_tick_from_millisecond(int32_t ms);
void rt_thread_mdelay(int32_t ms);
rt_thread_t rt_thread_create(const char *, void (*)(void *), void *, uint32_t, uint8_t, uint32_t);
rt_err_t rt_thread_startup(rt_thread_t thread);
rt_err_t rt_thread_delete(rt_thread_t thread);
rt_err_t rt_sem_init(struct rt_semaphore *, const char *, uint32_t, uint8_t);
rt_err_t rt_sem_detach(struct rt_semaphore *);
rt_err_t rt_sem_take(struct rt_semaphore *, int32_t);
rt_err_t rt_sem_release(struct rt_semaphore *);
#endif
