#ifndef BLE_BATTERY_RTTHREAD_H
#define BLE_BATTERY_RTTHREAD_H
#include "../../battery_host/stubs/rtthread.h"
#include <string.h>
typedef void *rt_mailbox_t;
#define rt_strcmp strcmp
static inline void rt_enter_critical(void) {}
static inline void rt_exit_critical(void) {}
#endif
