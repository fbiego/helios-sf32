#include "rtthread.h"
struct rt_mtd_nor_device { int unused; };
rt_size_t rt_mtd_nor_read(struct rt_mtd_nor_device *, rt_off_t, rt_uint8_t *, rt_uint32_t);
