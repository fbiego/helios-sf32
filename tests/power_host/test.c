#include <assert.h>
#include <string.h>
#include "core/helios_power.c"

#if HELIOS_HAS_LOW_POWER
static int idle_locks, requests, releases, panel_enabled = 1, panel_open = 1;
static int waited, invalidation = 1, ready, no_device, no_refresh, fail_cmd;
static int pending_frame_ms, power_off_count;
static rt_tick_t ticks;
static int commands[32], command_count;
static lv_timer_t refresh;
static lv_display_t display;
static lv_event_cb_t refresh_request_cb;
static lv_event_t refresh_request_event;
static int device;

void rt_pm_request(int mode) { assert(mode == PM_SLEEP_MODE_IDLE); idle_locks++; requests++; }
void rt_pm_release(int mode) { assert(mode == PM_SLEEP_MODE_IDLE && idle_locks > 0); idle_locks--; releases++; }
rt_device_t rt_device_find(const char *name) { assert(!strcmp(name, "lcd")); return no_device ? NULL : &device; }
rt_err_t rt_device_control(rt_device_t dev, int cmd, void *args)
{
    assert(dev == &device && idle_locks == 1);
    if (cmd == RTGRAPHIC_CTRL_GET_BUSY) {
        assert(waited && refresh.paused && !invalidation && panel_open);
        if (cmd == fail_cmd) return -RT_ERROR;
        *(bool *)args = pending_frame_ms > 0;
        return RT_EOK;
    }
    commands[command_count++] = cmd;
    if (cmd == fail_cmd) return -RT_ERROR;
    if (cmd == RTGRAPHIC_CTRL_POWEROFF) {
        assert(waited && refresh.paused && !invalidation && !pending_frame_ms);
        ++power_off_count;
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
rt_tick_t rt_tick_get(void) { return ticks; }
rt_tick_t rt_tick_from_millisecond(int32_t ms) { return (rt_tick_t)ms; }
void rt_thread_mdelay(int32_t ms)
{
    assert(panel_enabled && idle_locks == 1);
    if (ms == 1) {
        assert(panel_open && refresh.paused && !invalidation && pending_frame_ms > 0);
        --pending_frame_ms;
    } else {
        assert(ms == 20);
    }
    ticks += (rt_tick_t)ms;
}
lv_display_t *lv_display_get_default(void) { return &display; }
lv_timer_t *lv_display_get_refr_timer(lv_display_t *d) { assert(d == &display); return no_refresh ? NULL : &refresh; }
void lv_display_enable_invalidation(lv_display_t *d, bool enable) { assert(d == &display); invalidation = enable; }
void lv_display_add_event_cb(lv_display_t *d, lv_event_cb_t cb, int code, void *user_data)
{
    assert(d == &display && code == LV_EVENT_REFR_REQUEST && !refresh_request_cb);
    refresh_request_cb = cb;
    refresh_request_event.user_data = user_data;
}
void *lv_event_get_user_data(lv_event_t *event) { return event->user_data; }
void lv_timer_pause(lv_timer_t *t) { assert(t == &refresh); t->paused = 1; }
void lv_timer_resume(lv_timer_t *t) { assert(t == &refresh); t->paused = 0; }
void lv_timer_ready(lv_timer_t *t) { assert(t == &refresh && panel_open && invalidation); ready++; }
static void wait_flush(lv_display_t *d)
{
    assert(d == &display && panel_open && refresh.paused && !invalidation);
    waited++;
}

static void request_layout_refresh(void)
{
    /* LVGL's layout path resumes the timer regardless of invalidation state.
     * The next timer run could draw a region invalidated before shutdown. */
    lv_timer_resume(&refresh);
    if (refresh_request_cb)
        refresh_request_cb(&refresh_request_event);
}
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
    fail_cmd = RTGRAPHIC_CTRL_GET_BUSY;
    assert(helios_power_set_display(0) == -RT_ERROR);
    assert(panel_open && invalidation && !refresh.paused && !power_off_count);
    fail_cmd = 0;
    pending_frame_ms = 300;
    assert(helios_power_set_display(0) == -RT_ETIMEOUT);
    assert(panel_open && invalidation && !refresh.paused && !power_off_count);
    pending_frame_ms = 0;
    fail_cmd = RTGRAPHIC_CTRL_POWEROFF;
    assert(helios_power_set_display(0) == -RT_ERROR);
    assert(idle_locks == 1 && invalidation && !refresh.paused && panel_enabled);
    fail_cmd = 0;
    /* The LVGL copy has completed, but the LCD still has a frame to send. */
    pending_frame_ms = 8;
    rt_tick_t before = ticks;
    assert(helios_power_set_display(0) == RT_EOK);
    assert(ticks - before == 8 && power_off_count == 1);
    assert(idle_locks == 0 && refresh.paused && !invalidation && !panel_enabled);
    request_layout_refresh();
    assert(refresh.paused);
    int previous_commands = command_count;
    assert(helios_power_set_display(0) == RT_EOK && command_count == previous_commands);
    assert(helios_power_set_display(1) == RT_EOK);
    assert(idle_locks == 1 && invalidation && !refresh.paused && panel_enabled && ready == 1);
    lv_timer_pause(&refresh);
    request_layout_refresh();
    assert(!refresh.paused);
    assert(commands[command_count - 2] == RTGRAPHIC_CTRL_POWERON);
    assert(commands[command_count - 1] == RTGRAPHIC_CTRL_SET_BUF_FORMAT);
    assert(helios_power_set_display(1) == RT_EOK && idle_locks == 1);
    assert(helios_power_set_display(0) == RT_EOK);
    fail_cmd = RTGRAPHIC_CTRL_SET_BUF_FORMAT;
    assert(helios_power_set_display(1) == -RT_ERROR);
    assert(idle_locks == 0 && refresh.paused && !invalidation && !panel_enabled);
    request_layout_refresh();
    assert(refresh.paused);
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
