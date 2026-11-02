#include "rtthread.h"
#define RTGRAPHIC_CTRL_POWEROFF 1
#define RTGRAPHIC_CTRL_POWERON 2
#define RTGRAPHIC_CTRL_SET_BUF_FORMAT 3
#define RTGRAPHIC_PIXEL_FORMAT_RGB565 16
rt_device_t rt_device_find(const char *name);
rt_err_t rt_device_control(rt_device_t device, int cmd, void *args);
