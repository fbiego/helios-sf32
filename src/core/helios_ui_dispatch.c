#include "helios_ui_dispatch.h"
#include <rtthread.h>
#include <rthw.h>

/* Ten distinct bridge callbacks currently use this queue. */
#define HELIOS_UI_WORK_CAPACITY 16u
static helios_ui_work_t work[HELIOS_UI_WORK_CAPACITY];
static unsigned head;
static unsigned count;

bool helios_ui_dispatch_post(helios_ui_work_t callback)
{
    if (!callback)
        return false;
    rt_base_t level = rt_hw_interrupt_disable();
    for (unsigned i = 0; i < count; ++i) {
        if (work[(head + i) % HELIOS_UI_WORK_CAPACITY] == callback) {
            rt_hw_interrupt_enable(level);
            return true;
        }
    }
    if (count == HELIOS_UI_WORK_CAPACITY) {
        rt_hw_interrupt_enable(level);
        return false;
    }
    work[(head + count) % HELIOS_UI_WORK_CAPACITY] = callback;
    ++count;
    rt_hw_interrupt_enable(level);
    return true;
}

void helios_ui_dispatch_poll(void)
{
    rt_base_t level = rt_hw_interrupt_disable();
    unsigned budget = count;
    rt_hw_interrupt_enable(level);

    while (budget--) {
        level = rt_hw_interrupt_disable();
        helios_ui_work_t callback = work[head];
        head = (head + 1u) % HELIOS_UI_WORK_CAPACITY;
        --count;
        rt_hw_interrupt_enable(level);
        callback(RT_NULL);
    }
}
