#include "helios_chronos.h"

#include <rtthread.h>
#include <drivers/rtc.h>

#include "chronos_core.h"
#include "helios_app.h"
#include "helios_ble.h"
#include "helios_ios.h"
#include "helios_platform.h"

#define LOG_TAG "helios.proto"
#include "log.h"

static uint8_t g_chronos_ready;

void helios_chronos_update_battery(uint8_t percent, uint8_t charging)
{
    if (!g_chronos_ready || !helios_ble_can_notify())
        return;

    chronos_status_t status = chronos_core_send_battery(percent, charging != 0);
    if (status != CHRONOS_STATUS_OK)
        LOG_W("battery report failed status=%d", status);
}

static void helios_chronos_send_watch_battery(void)
{
    if (!helios_ble_can_notify())
        return;

    helios_battery_t battery;
    rt_err_t err = helios_battery_read(&battery);
    if (err == RT_EOK)
        helios_chronos_update_battery(battery.percent, battery.charging);
    else
        LOG_W("battery report skipped: ADC error=%d", err);
}

void helios_chronos_update_activity(uint32_t steps, uint32_t calories)
{
    if (!g_chronos_ready || !helios_ble_can_notify()) return;
    chronos_status_t status = chronos_core_send_realtime_steps(steps, calories);
    if (status != CHRONOS_STATUS_OK) LOG_W("activity report failed status=%d", status);
}

static void helios_chronos_tx(uint8_t *data, size_t length)
{
    int ret = helios_ble_send(data, (uint16_t)length);
    if (ret <= 0)
        LOG_W("chronos tx failed ret=%d len=%u", ret, (unsigned int)length);
}

static void helios_chronos_connection(bool connected)
{
    LOG_I("chronos %s", connected ? "connected" : "disconnected");
    helios_app_chronos_connection(connected);
}

static void helios_chronos_notification(chronos_notification_t *notification)
{
    if (!notification)
        return;

    helios_app_chronos_notification(notification);
    LOG_D("notification from %s:  %s: %s", notification->app, notification->title, notification->message);
}

static void helios_chronos_ringer(const char *caller, bool active)
{
    helios_app_chronos_ringer(caller, active);
    LOG_D("ringer %s %s", active ? "start" : "stop", caller ? caller : "");
}

static void helios_chronos_config(chronos_config_t config, uint32_t a, uint32_t b)
{
    switch (config)
    {
        case CHRONOS_CONFIG_TIME:
        {
            const chronos_date_time_t *date_time = chronos_core_get_date_time();
            if (!date_time)
                return;

            if (date_time->year < 2020 || date_time->month < 1 || date_time->month > 12 ||
                date_time->day < 1 || date_time->day > 31 ||
                date_time->hour > 23 || date_time->minute > 59 || date_time->second > 59)
            {
                LOG_W("invalid time %u-%u-%u %u:%u:%u",
                    (unsigned int)date_time->year,
                    date_time->month,
                    date_time->day,
                    date_time->hour,
                    date_time->minute,
                    date_time->second);
                return;
            }

            set_date(date_time->year, date_time->month, date_time->day);
            set_time(date_time->hour, date_time->minute, date_time->second);
            LOG_I("time synced %04u-%02u-%02u %02u:%02u:%02u",
                (unsigned int)date_time->year,
                date_time->month,
                date_time->day,
                date_time->hour,
                date_time->minute,
                date_time->second);
        }
        break;
        case CHRONOS_CONFIG_DEVICE_INFO_REQUEST: 
        {
            LOG_I("device info request received");
            chronos_status_t status = chronos_core_send_info(0x88);
            LOG_I("info request status=%d", status);

            status = chronos_core_send_notify_battery(true);
            LOG_I("notify battery request status=%d", status);

            helios_chronos_send_watch_battery();
        }

        break;
        default:
            helios_app_chronos_config(config, a, b);
            break;
    }
}

static void helios_chronos_init_once(void)
{
    /* Both BLE callbacks and UI startup can prepare the protocol now that
     * stored contacts/QR are restored before the first connection. */
    rt_enter_critical();
    if (g_chronos_ready) {
        rt_exit_critical();
        return;
    }

    chronos_core_init();
    chronos_core_set_tx_cb(helios_chronos_tx);
    chronos_core_set_connection_cb(helios_chronos_connection);
    chronos_core_set_notification_cb(helios_chronos_notification);
    chronos_core_set_ringer_cb(helios_chronos_ringer);
    chronos_core_set_config_cb(helios_chronos_config);
    helios_app_chronos_init();

    g_chronos_ready = 1;
    rt_exit_critical();
}

void helios_chronos_on_connected(void)
{
    helios_chronos_init_once();
    chronos_core_reset_rx(chronos_core_get_state());
    chronos_core_set_connection_status(true, false);
    LOG_I("Chronos connected");
}

void helios_chronos_prepare(void)
{
    helios_chronos_init_once();
}

void helios_chronos_on_disconnected(void)
{
    helios_chronos_init_once();
    chronos_core_reset_rx(chronos_core_get_state());
    chronos_core_set_connection_status(false, false);
    LOG_I("Chronos disconnected");
}

void helios_chronos_on_subscribed(void)
{
    helios_chronos_init_once();
    chronos_core_set_connection_status(helios_ble_is_connected() != 0, true);

    chronos_status_t status = chronos_core_send_sync_request();
    LOG_I("sync request status=%d", status);

    status = chronos_core_send_info(0x88);
    LOG_I("info request status=%d", status);

    status = chronos_core_send_notify_battery(true);
    LOG_I("notify battery request status=%d", status);

    helios_chronos_send_watch_battery();
}

void helios_chronos_on_rx(const uint8_t *data, uint16_t len)
{
    if (!data || len == 0)
        return;

    helios_chronos_init_once();

    chronos_status_t status = chronos_core_receive_ble_packet((uint8_t *)data, len);
    if (status != CHRONOS_STATUS_OK && status != CHRONOS_STATUS_PENDING)
        LOG_W("chronos rx status=%d len=%u", status, len);
}

void helios_chronos_send_music_action(const char *action)
{
    uint16_t command;

    LOG_I("Send music action: %s", action ? action : "");

    helios_chronos_init_once();

    if (!action)
        return;

    if (helios_ios_music_command(action))
        return;

    if (rt_strcmp(action, "previous") == 0)
        command = 0x9D02u;
    else if (rt_strcmp(action, "toggle") == 0)
        command = 0x9900u;
    else if (rt_strcmp(action, "next") == 0)
        command = 0x9D03u;
    else {
        LOG_W("unknown music action %s", action);
        return;
    }

    chronos_status_t status = chronos_core_send_music_control(command);
    if (status != CHRONOS_STATUS_OK)
        LOG_W("music action %s failed status=%d", action, status);
}

void helios_subject_sound_volume_change(int32_t value)
{
    chronos_status_t status = chronos_core_send_volume(value);
    if (status != CHRONOS_STATUS_OK)
        LOG_W("sound volume change failed status=%d", status);
}
