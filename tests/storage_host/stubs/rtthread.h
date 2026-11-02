#ifndef STORAGE_TEST_RTTHREAD_H
#define STORAGE_TEST_RTTHREAD_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#define rt_memcpy memcpy
#define rt_memset memset
typedef int rt_err_t;
typedef uint32_t rt_uint32_t;
typedef unsigned char rt_uint8_t;
typedef size_t rt_size_t;
typedef long rt_off_t;
typedef void *rt_thread_t;
struct rt_mutex { int unused; };
struct rt_semaphore { int unused; };
#define RT_EOK 0
#define RT_ERROR 1
#define RT_ENOMEM 12
#define RT_WAITING_FOREVER (-1)
#define RT_IPC_FLAG_PRIO 1
#define RT_IPC_FLAG_FIFO 0
#define INIT_ENV_EXPORT(fn) static void *test_init_ref __attribute__((unused)) = (void *)&fn
#define rt_strncpy strncpy
static inline void rt_enter_critical(void) {}
static inline void rt_exit_critical(void) {}
int rt_mutex_init(struct rt_mutex *, const char *, int);
int rt_mutex_take(struct rt_mutex *, int);
int rt_mutex_release(struct rt_mutex *);
int rt_sem_init(struct rt_semaphore *, const char *, unsigned, int);
int rt_sem_take(struct rt_semaphore *, int);
int rt_sem_release(struct rt_semaphore *);
int rt_tick_from_millisecond(int);
void rt_thread_mdelay(int);
rt_thread_t rt_thread_create(const char *, void (*)(void *), void *, unsigned, unsigned, unsigned);
int rt_thread_startup(rt_thread_t);
int rt_thread_delete(rt_thread_t);
#endif
