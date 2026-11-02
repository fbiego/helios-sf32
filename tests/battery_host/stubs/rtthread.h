#include "../../bmi270_host/stubs/rtthread.h"
#define RT_ENOSYS 38
#define RT_WAITING_FOREVER (-1)
#define RT_IPC_FLAG_PRIO 1
#define INIT_PREV_EXPORT(fn)
struct rt_mutex { int unused; };
rt_err_t rt_mutex_init(struct rt_mutex *, const char *, uint8_t);
rt_err_t rt_mutex_take(struct rt_mutex *, int32_t);
rt_err_t rt_mutex_release(struct rt_mutex *);
