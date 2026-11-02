#include "helios_power.h"
#include "helios_features.h"

#if HELIOS_HAS_LOW_POWER
#if !defined(BSP_USING_PM) || !defined(RT_USING_PM) || !defined(PM_DEEP_ENABLE)
#error SFWatch low power requires SDK PM with DeepSleep enabled
#endif
#include <stdbool.h>
#include <rtdevice.h>
#include <drivers/pm.h>
#include <board.h>
#include "lvgl.h"
#include "lvgl_private.h"

static uint8_t g_initialized;
static uint8_t g_display_on = 1;

void helios_power_init(void)
{
    if (!g_initialized)
    {
        rt_pm_request(PM_SLEEP_MODE_IDLE);
        g_initialized = 1;
    }
}

rt_err_t helios_power_set_display(uint8_t on)
{
    helios_power_init();
    on = !!on;
    if (g_display_on == on)
        return RT_EOK;

    rt_device_t lcd = rt_device_find("lcd");
    lv_display_t *display = lv_display_get_default();
    lv_timer_t *refresh = display ? lv_display_get_refr_timer(display) : NULL;
    if (!lcd || !refresh)
        return -RT_ERROR;

    if (!on)
    {
        /* Finish the SDK's pending pixel copy before stopping the panel. The
         * synchronous POWEROFF command also drains earlier LCD task messages. */
        if (display->flush_wait_cb)
            display->flush_wait_cb(display);
        /* LVGL normally resumes its refresh timer on invalidation. Suppress
         * invalidations too, so background subjects cannot restart drawing. */
        lv_display_enable_invalidation(display, false);
        lv_timer_pause(refresh);
        rt_err_t err = rt_device_control(lcd, RTGRAPHIC_CTRL_POWEROFF, NULL);
        if (err != RT_EOK)
        {
            lv_display_enable_invalidation(display, true);
            lv_timer_resume(refresh);
            return err;
        }
        BSP_GPIO_Set(HELIOS_DISPLAY_ENABLE_PIN, 0, 1);
        g_display_on = 0;
        rt_pm_release(PM_SLEEP_MODE_IDLE);
        return RT_EOK;
    }

    rt_pm_request(PM_SLEEP_MODE_IDLE);
    BSP_GPIO_Set(HELIOS_DISPLAY_ENABLE_PIN, 1, 1);
    rt_thread_mdelay(20);
    rt_err_t err = rt_device_control(lcd, RTGRAPHIC_CTRL_POWERON, NULL);
    if (err == RT_EOK)
    {
        /* POWERON is asynchronous. SET_BUF_FORMAT is synchronous and acts as
         * a queue barrier, and restores the LVGL port's RGB565 format. */
        uint16_t format = RTGRAPHIC_PIXEL_FORMAT_RGB565;
        err = rt_device_control(lcd, RTGRAPHIC_CTRL_SET_BUF_FORMAT, &format);
    }
    if (err != RT_EOK)
    {
        rt_device_control(lcd, RTGRAPHIC_CTRL_POWEROFF, NULL);
        BSP_GPIO_Set(HELIOS_DISPLAY_ENABLE_PIN, 0, 1);
        rt_pm_release(PM_SLEEP_MODE_IDLE);
        return err;
    }
    g_display_on = 1;
    lv_display_enable_invalidation(display, true);
    lv_timer_resume(refresh);
    lv_timer_ready(refresh);
    return RT_EOK;
}

/* The inherited devkit hooks switch PA11 as an MPI2 power supply on every
 * sleep/wake. SFWatch uses PA11 as a button: preserve its input configuration
 * and the shared PA10 sensor/touch supply across DeepSleep. */
void BSP_PowerDownCustom(int coreid, bool is_deep_sleep)
{
    (void)coreid;
    (void)is_deep_sleep;
}

void BSP_PowerUpCustom(bool is_deep_sleep)
{
    (void)is_deep_sleep;
}
#else
void helios_power_init(void) {}
rt_err_t helios_power_set_display(uint8_t on)
{
    (void)on;
    return RT_EOK;
}
#endif
