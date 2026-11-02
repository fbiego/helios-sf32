#include <assert.h>
#include <string.h>
#include "core/helios_power.c"

#if HELIOS_HAS_LOW_POWER
static int idle_locks, requests, releases, panel_enabled = 1, panel_open = 1;
static int waited, invalidation = 1, ready, no_device, no_refresh, fail_cmd;
static int commands[32], command_count;
static lv_timer_t refresh;
static lv_display_t display;
static int device;

void rt_pm_request(int mode) { assert(mode == PM_SLEEP_MODE_IDLE); idle_locks++; requests++; }
void rt_pm_release(int mode) { assert(mode == PM_SLEEP_MODE_IDLE && idle_locks > 0); idle_locks--; releases++; }
rt_device_t rt_device_find(const char *name) { assert(!strcmp(name, "lcd")); return no_device ? NULL : &device; }
rt_err_t rt_device_control(rt_device_t dev, int cmd, void *args)
{
    assert(dev == &device && idle_locks == 1);
    commands[command_count++] = cmd;
    if (cmd == fail_cmd) return -RT_ERROR;
    if (cmd == RTGRAPHIC_CTRL_POWEROFF) {
        assert(waited && refresh.paused && !invalidation);
        panel_open = 0;
    } else if (cmd == RTGRAPHIC_CTRL_POWERON) {
        assert(panel_enabled && refresh.paused && !invalidation);
        panel_open = 1;
    } else {
        assert(cmd == RTGRAPHIC_CTRL_SET_BUF_FORMAT && panel_open);
        assert(*(uint16_t *)args == RTGRAPHIC_PIXEL_FORMAT_RGB565);
    }
    return RT_EOK;
}
void BSP_GPIO_Set(int pin, int value, int porta)
{
    assert(pin == 26 && porta == 1); /* Never change PA10 or PA11. */
    if (!value) assert(!panel_open);
    panel_enabled = value;
}
void rt_thread_mdelay(int32_t ms) { assert(ms == 20 && panel_enabled && idle_locks == 1); }
lv_display_t *lv_display_get_default(void) { return &display; }
lv_timer_t *lv_display_get_refr_timer(lv_display_t *d) { assert(d == &display); return no_refresh ? NULL : &refresh; }
void lv_display_enable_invalidation(lv_display_t *d, bool enable) { assert(d == &display); invalidation = enable; }
void lv_timer_pause(lv_timer_t *t) { assert(t == &refresh); t->paused = 1; }
void lv_timer_resume(lv_timer_t *t) { assert(t == &refresh); t->paused = 0; }
void lv_timer_ready(lv_timer_t *t) { assert(t == &refresh && panel_open && invalidation); ready++; }
static void wait_flush(lv_display_t *d) { assert(d == &display && panel_open); waited++; }
#endif

int main(void)
{
    helios_power_init();
    helios_power_init();
#if HELIOS_HAS_LOW_POWER
    display.flush_wait_cb = wait_flush;
    assert(idle_locks == 1 && requests == 1);
    no_device = 1;
    assert(helios_power_set_display(0) == -RT_ERROR && idle_locks == 1);
    no_device = 0;
    no_refresh = 1;
    assert(helios_power_set_display(0) == -RT_ERROR && idle_locks == 1);
    no_refresh = 0;
    fail_cmd = RTGRAPHIC_CTRL_POWEROFF;
    assert(helios_power_set_display(0) == -RT_ERROR);
    assert(idle_locks == 1 && invalidation && !refresh.paused && panel_enabled);
    fail_cmd = 0;
    assert(helios_power_set_display(0) == RT_EOK);
    assert(idle_locks == 0 && refresh.paused && !invalidation && !panel_enabled);
    int previous_commands = command_count;
    assert(helios_power_set_display(0) == RT_EOK && command_count == previous_commands);
    assert(helios_power_set_display(1) == RT_EOK);
    assert(idle_locks == 1 && invalidation && !refresh.paused && panel_enabled && ready == 1);
    assert(commands[command_count - 2] == RTGRAPHIC_CTRL_POWERON);
    assert(commands[command_count - 1] == RTGRAPHIC_CTRL_SET_BUF_FORMAT);
    assert(helios_power_set_display(1) == RT_EOK && idle_locks == 1);
    assert(helios_power_set_display(0) == RT_EOK);
    fail_cmd = RTGRAPHIC_CTRL_SET_BUF_FORMAT;
    assert(helios_power_set_display(1) == -RT_ERROR);
    assert(idle_locks == 0 && refresh.paused && !invalidation && !panel_enabled);
    fail_cmd = RTGRAPHIC_CTRL_POWERON;
    assert(helios_power_set_display(1) == -RT_ERROR && idle_locks == 0);
    fail_cmd = 0;
    assert(helios_power_set_display(1) == RT_EOK && idle_locks == 1);
    previous_commands = command_count;
    BSP_PowerDownCustom(0, true);
    BSP_PowerUpCustom(true);
    assert(command_count == previous_commands && panel_enabled);
    assert(requests == releases + 1);
#else
    assert(helios_power_set_display(0) == RT_EOK);
    assert(helios_power_set_display(1) == RT_EOK);
#endif
    return 0;
}
