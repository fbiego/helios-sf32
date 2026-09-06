#include "helios_app.h"

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include <time.h>

#include "helios_max30100.h"
#include "littlevgl2rtt.h"
#include "lvgl.h"
#include "helios_ui/helios_ui.h"
#include "apps/mpesa/mpesa_notifications.h"
#include "helios_ui/custom/apps/notifications/notifications.h"
#ifndef _WIN32
#include "drv_lcd.h"
#endif

#include "helios_ble.h"
#include "helios_chronos.h"
#include "helios_platform.h"
#include "helios_prefs.h"

#define LOG_TAG "helios.app"
#include "log.h"

#define HELIOS_UI_STACK_SIZE (32 * 1024)
#define LCD_DEVICE_NAME "lcd"
#define HELIOS_SCREEN_TIMEOUT_DEFAULT_MS 5000U
#define HELIOS_SCREEN_IDLE_POLL_MS 250U
#define HELIOS_HEALTH_SENSOR_POLL_MS 20U
#define HELIOS_BUTTON_PA34 0
#define HELIOS_BUTTON_PA11 1
#define HELIOS_SCREEN_SLEEP_BUTTON HELIOS_BUTTON_PA34
#define HELIOS_PENDING_NOTIFICATIONS 4
#define HELIOS_WEATHER_HOURLY_DISPLAY_MAX 12

static struct rt_thread g_ui_thread;
ALIGN(RT_ALIGN_SIZE)
static uint8_t g_ui_stack[HELIOS_UI_STACK_SIZE];

static volatile uint8_t g_ble_connected;
static volatile uint8_t g_battery_percent = 0;
static volatile uint32_t g_battery_mv = 0;
static volatile uint8_t g_ui_ready;
static volatile uint32_t g_screen_timeout_ms = HELIOS_SCREEN_TIMEOUT_DEFAULT_MS;
static volatile uint8_t g_screen_awake_override;
static uint8_t g_screen_on;
static uint8_t g_screen_brightness = 30;
static lv_image_dsc_t g_nav_icon_dsc;
static uint32_t g_nav_crc = 0xFFFFFFFFu;
static uint8_t g_nav_icon_data[CHRONOS_ICON_DATA_SIZE];

typedef struct {
    const void *icon;
    char app[CHRONOS_TEXT_SMALL_SIZE];
    char title[HELIOS_NOTIFICATION_TITLE_MAX];
    char message[HELIOS_NOTIFICATION_MESSAGE_MAX];
} helios_pending_notification_t;

static helios_pending_notification_t g_pending_notifications[HELIOS_PENDING_NOTIFICATIONS];
static uint8_t g_pending_notification_head;
static uint8_t g_pending_notification_count;
static volatile uint8_t g_pending_notification_async;

static volatile uint8_t g_pending_connection_async;
static bool g_pending_connected;

static volatile uint8_t g_pending_music_async;
static chronos_music_info_t g_pending_music;

static volatile uint8_t g_pending_nav_async;
static chronos_navigation_t g_pending_nav;

static volatile uint8_t g_pending_nav_icon_async;
static uint8_t g_pending_nav_icon[CHRONOS_ICON_DATA_SIZE];
static uint32_t g_pending_nav_icon_crc;

static volatile uint8_t g_pending_weather_async;

static bool new_measurement = true;
static lv_timer_t *g_health_sensor_timer;

static uint32_t helios_chronos_time_now(void)
{
    return (uint32_t)time(RT_NULL);
}

static rt_device_t helios_lcd_device(void)
{
    return rt_device_find(LCD_DEVICE_NAME);
}

static uint8_t helios_input_active(void)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        if (lv_indev_get_state(indev) == LV_INDEV_STATE_PRESSED)
            return 1;
        indev = lv_indev_get_next(indev);
    }

    return helios_button_pressed(0) || helios_button_pressed(1);
}

static void helios_input_wait_release(void)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        lv_indev_wait_release(indev);
        indev = lv_indev_get_next(indev);
    }
}

static void helios_screen_apply_brightness(void)
{
    rt_device_t lcd = helios_lcd_device();
    if (lcd)
    {
        uint8_t brightness = g_screen_brightness;
        rt_device_control(lcd, RTGRAPHIC_CTRL_SET_BRIGHTNESS, &brightness);
    }
}

static void helios_screen_set_panel_brightness(uint8_t brightness)
{
    rt_device_t lcd = helios_lcd_device();
    if (lcd)
        rt_device_control(lcd, RTGRAPHIC_CTRL_SET_BRIGHTNESS, &brightness);
}

static void helios_screen_power(uint8_t on)
{
    lv_display_t *display = lv_display_get_default();

    if (on)
    {
        if (g_screen_on)
            return;

        g_screen_on = 1;
        helios_screen_apply_brightness();
        if (display)
        {
            lv_display_trigger_activity(display);
            lv_obj_invalidate(lv_screen_active());
        }
        helios_input_wait_release();
        LOG_I("screen on");
        return;
    }

    if (!g_screen_on)
        return;

    g_screen_on = 0;
    if (display)
        lv_display_trigger_activity(display);
    helios_screen_set_panel_brightness(0);
    LOG_I("screen off");
}

void helios_ui_set_ble(uint8_t connected)
{
    g_ble_connected = connected ? 1 : 0;
    helios_subject_set_system_connection(g_ble_connected);
}

void helios_ui_set_battery(uint8_t percent, uint32_t millivolts)
{
    g_battery_percent = percent;
    g_battery_mv = millivolts;
}

static void helios_nav_icon_init(void)
{
    g_nav_icon_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    g_nav_icon_dsc.header.cf = LV_COLOR_FORMAT_A1;
    g_nav_icon_dsc.header.w = 48;
    g_nav_icon_dsc.header.h = 48;
    g_nav_icon_dsc.header.stride = 6;
    g_nav_icon_dsc.header.reserved_2 = 0;
    g_nav_icon_dsc.header.flags = 0;
    g_nav_icon_dsc.data_size = 48 * 48 / 8;
    g_nav_icon_dsc.data = g_nav_icon_data;
    g_nav_icon_dsc.reserved = NULL;
}

static const void *helios_chronos_app_icon(uint8_t id)
{
    switch (id) {
    case 0x03:
        return icon_nt_chat;
    case 0x04:
        return icon_nt_mail;
    case 0x07:
        return icon_nt_qq;
    case 0x08:
        return icon_nt_skype;
    case 0x09:
        return icon_nt_wechat;
    case 0x0A:
        return icon_nt_whatsapp;
    case 0x0B:
        return icon_nt_mail;
    case 0x0E:
        return icon_nt_line;
    case 0x0F:
        return icon_nt_twitter;
    case 0x10:
        return icon_nt_facebook;
    case 0x11:
        return icon_nt_messenger;
    case 0x12:
        return icon_nt_instagram;
    case 0x13:
        return icon_nt_weibo;
    case 0x14:
        return icon_nt_kakao;
    case 0x16:
        return icon_nt_viber;
    case 0x17:
        return icon_nt_vk;
    case 0x18:
        return icon_nt_telegram;
    case 0x1B:
        return icon_nt_dingtalk;
    case 0x20:
        return icon_nt_whatsapp;
    case 0x30:
        return icon_nt_calendar;
    case 0x31:
        return icon_nt_snapchat;
    case 0x32:
        return icon_nt_tiktok;
    case 0x33:
        return icon_nt_paypal;
    default:
        return icon_nt_chat;
    }
}

static const char *helios_weather_day_name(int32_t offset)
{
    static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"};
    time_t now = time(RT_NULL);
    struct tm *tm_now = localtime(&now);
    int32_t day = tm_now ? tm_now->tm_wday : 0;

    day = (day + offset) % 7;
    if (day < 0)
        day += 7;

    return days[day];
}

static void helios_weather_format_temp(char *buf, size_t buf_size, int32_t temp)
{
    rt_snprintf(buf, buf_size, "%d°", (int)temp);
}

static void helios_weather_format_update_time(char *buf, size_t buf_size, uint32_t timestamp)
{
    time_t update_time = timestamp ? (time_t)timestamp : time(RT_NULL);
    struct tm *tm_update = localtime(&update_time);

    if (tm_update)
        rt_snprintf(buf, buf_size, "%02d:%02d", tm_update->tm_hour, tm_update->tm_min);
    else
        rt_snprintf(buf, buf_size, "--:--");
}

static const chronos_hourly_forecast_t *helios_weather_current_hourly(void)
{
    time_t now = time(RT_NULL);
    struct tm *tm_now = localtime(&now);
    int32_t hour = tm_now ? tm_now->tm_hour : 0;

    if (chronos_core_has_forecast_hour(hour))
        return chronos_core_get_forecast_hour(hour);

    for (int32_t i = 0; i < (int32_t)CHRONOS_FORECAST_SIZE; i++) {
        if (chronos_core_has_forecast_hour(i))
            return chronos_core_get_forecast_hour(i);
    }

    return NULL;
}

static void helios_chronos_add_notification(const void *icon, const char *app, const char *title, const char *message)
{
    time_t now = time(RT_NULL);
    struct tm *tm_now = localtime(&now);
    char time_now[16] = "--:--";
    bool is_mpesa;
    if (tm_now)
        rt_snprintf(time_now, sizeof(time_now), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);

    is_mpesa = helios_mpesa_handle_notification(app, title, message);

    helios_notifications_add(is_mpesa ? icon_mpesa_watch_32 : (icon ? icon : icon_nt_chat),
                             is_mpesa ? "MPESA" : (title ? title : "Message"),
                             time_now,
                             message ? message : "");
}

static void helios_chronos_notification_async(void *user_data)
{
    (void)user_data;

    while (1) {
        helios_pending_notification_t pending;

        rt_base_t level = rt_hw_interrupt_disable();
        if (g_pending_notification_count == 0) {
            g_pending_notification_async = 0;
            rt_hw_interrupt_enable(level);
            return;
        }

        pending = g_pending_notifications[g_pending_notification_head];
        g_pending_notification_head = (uint8_t)((g_pending_notification_head + 1u) % HELIOS_PENDING_NOTIFICATIONS);
        g_pending_notification_count--;
        rt_hw_interrupt_enable(level);

        helios_chronos_add_notification(pending.icon, pending.app, pending.title, pending.message);
    }
}

static void helios_chronos_queue_notification(const void *icon, const char *app, const char *title, const char *message)
{
    if (!g_ui_ready)
        return;

    rt_base_t level = rt_hw_interrupt_disable();

    if (g_pending_notification_count >= HELIOS_PENDING_NOTIFICATIONS) {
        g_pending_notification_head = (uint8_t)((g_pending_notification_head + 1u) % HELIOS_PENDING_NOTIFICATIONS);
        g_pending_notification_count--;
    }

    uint8_t index = (uint8_t)((g_pending_notification_head + g_pending_notification_count) % HELIOS_PENDING_NOTIFICATIONS);
    g_pending_notifications[index].icon = icon ? icon : icon_nt_chat;
    rt_snprintf(g_pending_notifications[index].app,
                sizeof(g_pending_notifications[index].app),
                "%s",
                app ? app : "");
    rt_snprintf(g_pending_notifications[index].title,
                sizeof(g_pending_notifications[index].title),
                "%s",
                title ? title : "Message");
    rt_snprintf(g_pending_notifications[index].message,
                sizeof(g_pending_notifications[index].message),
                "%s",
                message ? message : "");
    g_pending_notification_count++;

    if (g_pending_notification_async) {
        rt_hw_interrupt_enable(level);
        return;
    }

    g_pending_notification_async = 1;
    rt_hw_interrupt_enable(level);

    if (lv_async_call(helios_chronos_notification_async, RT_NULL) != LV_RESULT_OK) {
        level = rt_hw_interrupt_disable();
        g_pending_notification_async = 0;
        rt_hw_interrupt_enable(level);
    }
}

static void helios_chronos_connection_async(void *user_data)
{
    (void)user_data;

    rt_base_t level = rt_hw_interrupt_disable();
    bool connected = g_pending_connected;
    g_pending_connection_async = 0;
    rt_hw_interrupt_enable(level);

    helios_ui_set_ble(connected ? 1u : 0u);
}

static void helios_chronos_music_async(void *user_data)
{
    (void)user_data;

    chronos_music_info_t music;

    rt_base_t level = rt_hw_interrupt_disable();
    music = g_pending_music;
    g_pending_music_async = 0;
    rt_hw_interrupt_enable(level);

    LOG_I("music state: %s app=%s package=%s",
          music.state ? "playing" : "paused",
          music.app_name,
          music.package_name);
    LOG_I("music track: %s - %s", music.title, music.artist);

    helios_subject_set_music_state(music.state ? 1 : 0);
    helios_subject_set_music_app(music.app_name);
    helios_subject_set_music_package(music.package_name);
    helios_subject_set_music_track(music.title);
    helios_subject_set_music_artist(music.artist);
    helios_subject_set_music_album_color(lv_color_hex(music.background_color));
}

static void helios_chronos_nav_async(void *user_data)
{
    (void)user_data;

    chronos_navigation_t nav;

    rt_base_t level = rt_hw_interrupt_disable();
    nav = g_pending_nav;
    g_pending_nav_async = 0;
    rt_hw_interrupt_enable(level);

    LOG_I("navigation state: %s", nav.active ? "active" : "inactive");
    if (nav.active) {
        LOG_I("title: %s", nav.title);
        LOG_I("directions: %s", nav.directions);
        LOG_I("distance: %s", nav.distance);
        LOG_I("duration: %s", nav.duration);
        LOG_I("eta: %s", nav.eta);
        LOG_I("speed: %s", nav.speed);
    }

    char nav_text[128];
    rt_snprintf(nav_text, sizeof(nav_text), "%s\n%s %s", nav.eta, nav.duration, nav.distance);
    helios_subject_set_nav_text(nav_text);
    helios_subject_set_nav_title(nav.title);
    helios_subject_set_nav_directions(nav.directions);

    if (nav.active && nav.has_icon)
        helios_subject_set_nav_icon((void *)&g_nav_icon_dsc);
    else
        helios_subject_set_nav_icon(NULL);
}

static void helios_chronos_nav_icon_async(void *user_data)
{
    (void)user_data;

    uint32_t icon_crc;

    rt_base_t level = rt_hw_interrupt_disable();
    rt_memcpy(g_nav_icon_data, g_pending_nav_icon, sizeof(g_nav_icon_data));
    icon_crc = g_pending_nav_icon_crc;
    g_pending_nav_icon_async = 0;
    rt_hw_interrupt_enable(level);

    g_nav_crc = icon_crc;
    g_nav_icon_dsc.data = g_nav_icon_data;
    helios_subject_set_nav_icon((void *)&g_nav_icon_dsc);
}

static void helios_chronos_weather_async(void *user_data)
{
    (void)user_data;

    rt_base_t level = rt_hw_interrupt_disable();
    g_pending_weather_async = 0;
    rt_hw_interrupt_enable(level);

    int weather_count = chronos_core_get_weather_count();
    const chronos_weather_t *current = weather_count > 0 ? chronos_core_get_weather_at(0) : NULL;
    const chronos_hourly_forecast_t *current_hour = helios_weather_current_hourly();
    const chronos_weather_location_t *location = chronos_core_get_weather_location();
    const char *city = location && location->city[0] ? location->city : chronos_core_get_weather_city();
    char update_time[16];

    if (city && city[0])
        helios_subject_set_weather_location(city);

    if (current) {
        helios_subject_set_weather_code(current->icon);
        helios_subject_set_weather_icon((void *)helios_weather_icon_get(current->icon));
        helios_subject_set_weather_condition(helios_weather_condition_get(current->icon));
        helios_subject_set_weather_temp(current->temp);
        helios_subject_set_weather_temp_high(current->high);
        helios_subject_set_weather_temp_low(current->low);
        helios_subject_set_weather_uv(current->uv);
    }

    if (current_hour)
        helios_subject_set_weather_humidity(current_hour->humidity);

    helios_weather_format_update_time(update_time, sizeof(update_time), chronos_core_get_weather_time());
    helios_subject_set_weather_update_time(update_time);
    helios_subject_set_weather_temp_unit("C");

    helios_weather_daily_clear();
    for (int i = 0; i < weather_count && i < (int)HELIOS_DAILY_FORECAST_MAX; i++) {
        const chronos_weather_t *weather = chronos_core_get_weather_at(i);
        char temp[16];

        if (!weather)
            continue;

        if (weather->high != 0 || weather->low != 0)
            rt_snprintf(temp, sizeof(temp), "%d/%d°", weather->high, weather->low);
        else
            helios_weather_format_temp(temp, sizeof(temp), weather->temp);

        helios_weather_daily_add(weather->icon, helios_weather_day_name(i), temp);
    }

    helios_weather_hourly_clear();
    time_t now = time(RT_NULL);
    struct tm *tm_now = localtime(&now);
    int start_hour = tm_now ? tm_now->tm_hour : 0;
    uint32_t displayed = 0;

    for (int step = 0; step < (int)CHRONOS_FORECAST_SIZE && displayed < HELIOS_WEATHER_HOURLY_DISPLAY_MAX; step++) {
        int hour = (start_hour + step) % (int)CHRONOS_FORECAST_SIZE;
        const chronos_hourly_forecast_t *forecast;
        char time_label[16];
        char temp[16];
        char humidity[16];

        if (!chronos_core_has_forecast_hour(hour))
            continue;

        forecast = chronos_core_get_forecast_hour(hour);
        if (!forecast)
            continue;

        rt_snprintf(time_label, sizeof(time_label), "%02d:00", forecast->hour);
        helios_weather_format_temp(temp, sizeof(temp), forecast->temp);
        rt_snprintf(humidity, sizeof(humidity), "%d%%", forecast->humidity);
        helios_weather_hourly_add(forecast->icon, time_label, temp, humidity);
        displayed++;
    }

    LOG_I("weather updated: days=%d hourly=%u city=%s", weather_count, displayed, city ? city : "");
}

static void helios_chronos_schedule_async(lv_async_cb_t callback, volatile uint8_t *pending)
{
    if (!g_ui_ready)
        return;

    rt_base_t level = rt_hw_interrupt_disable();
    if (*pending) {
        rt_hw_interrupt_enable(level);
        return;
    }

    *pending = 1;
    rt_hw_interrupt_enable(level);

    if (lv_async_call(callback, RT_NULL) != LV_RESULT_OK) {
        level = rt_hw_interrupt_disable();
        *pending = 0;
        rt_hw_interrupt_enable(level);
    }
}

void helios_app_chronos_init(void)
{
    helios_nav_icon_init();
    chronos_core_set_time_cb(helios_chronos_time_now);
}

void helios_app_chronos_connection(bool connected)
{
    g_pending_connected = connected;
    helios_chronos_schedule_async(helios_chronos_connection_async, &g_pending_connection_async);
}

void helios_app_chronos_notification(const chronos_notification_t *notification)
{
    if (!notification)
        return;

    helios_chronos_queue_notification(
        helios_chronos_app_icon(notification->icon),
        notification->app,
        notification->title[0] ? notification->title : notification->app,
        notification->message);
}

void helios_app_chronos_ringer(const char *caller, bool active)
{
    if (active)
        helios_chronos_queue_notification(icon_nt_chat, "Phone", "Incoming call", caller ? caller : "");
}

void helios_app_music_update(const chronos_music_info_t *music)
{
    if (!music)
        return;

    rt_base_t level = rt_hw_interrupt_disable();
    g_pending_music = *music;
    rt_hw_interrupt_enable(level);

    helios_chronos_schedule_async(helios_chronos_music_async, &g_pending_music_async);
}

void helios_app_chronos_config(chronos_config_t config, uint32_t a, uint32_t b)
{
    switch (config) {
    case CHRONOS_CONFIG_NAV_DATA: {
        const chronos_navigation_t *nav = chronos_core_get_navigation();
        if (!nav)
            return;

        rt_base_t level = rt_hw_interrupt_disable();
        g_pending_nav = *nav;
        rt_hw_interrupt_enable(level);

        helios_chronos_schedule_async(helios_chronos_nav_async, &g_pending_nav_async);
        break;
    }
    case CHRONOS_CONFIG_NAV_ICON: {
        const chronos_navigation_t *nav = chronos_core_get_navigation();
        if (!nav)
            return;

        LOG_I("navigation icon chunk: position=%lu crc=0x%08lX",
              (unsigned long)a,
              (unsigned long)b);
        if (a == 2u && g_nav_crc != nav->icon_crc) {
            rt_base_t level = rt_hw_interrupt_disable();
            rt_memcpy(g_pending_nav_icon, nav->icon, sizeof(g_pending_nav_icon));
            g_pending_nav_icon_crc = nav->icon_crc;
            rt_hw_interrupt_enable(level);

            helios_chronos_schedule_async(helios_chronos_nav_icon_async, &g_pending_nav_icon_async);
        }
        break;
    }
    case CHRONOS_CONFIG_MUSIC:
        if (a == 2u) {
            const chronos_music_info_t *music = chronos_core_get_music_info();
            if (!music)
                return;

            helios_app_music_update(music);
        }
        break;
    case CHRONOS_CONFIG_WEATHER:
        helios_chronos_schedule_async(helios_chronos_weather_async, &g_pending_weather_async);
        break;
    case CHRONOS_CONFIG_PHONE_BATTERY: {
        const chronos_phone_info_t *phone = chronos_core_get_phone_info();
        LOG_I("Phone battery: %s\n", phone->is_charging ? "Charging" : "Not Charging");
        LOG_I("Level: %u%%\n", phone->battery_level);
        helios_subject_set_phone_battery(phone->battery_level);
        helios_subject_set_phone_charging(phone->is_charging ? 1 : 0);
        break;
    }
    case CHRONOS_CONFIG_APP: {
        const chronos_phone_info_t *phone = chronos_core_get_phone_info();
        if (b == 0u) {
            LOG_I("Chronos App; Code: %lu Version: %s\n",
                   (unsigned long)a,
                   phone->app_version);
            helios_subject_set_chronos_app_version(phone->app_version);
            helios_subject_set_chronos_app_code(phone->app_code);
        } else if (b == 1u) {
            LOG_I("Device Info; Android SDK: %lu Manufacturer: %s Model: %s\n",
                   (unsigned long)a,
                   phone->manufacturer,
                   phone->model);
            helios_subject_set_phone_manufacturer(phone->manufacturer);
            helios_subject_set_phone_model(phone->model);
            helios_subject_set_phone_sdk(phone->sdk_version);
        }
        break;
    }
    default:
        break;
    }
}

void on_music_control_cb(lv_event_t *e)
{
    const char *action = lv_event_get_user_data(e);
    LOG_D("music control action: %s", action ? action : "");
    helios_chronos_send_music_action(action);
}

void helios_screen_wake(void)
{
    helios_screen_power(1);
}

void helios_screen_set_timeout_ms(uint32_t timeout_ms)
{
    g_screen_timeout_ms = timeout_ms;
    if (timeout_ms == 0)
        helios_screen_wake();
    else
        lv_display_trigger_activity(NULL);

    LOG_I("screen timeout %u ms", timeout_ms);
}

void helios_max30100_sample(const helios_max30100_sample_t *sample,
                                            void *user_data)
{
    (void)user_data;

    // LOG_I("Finger %u, cached %u, beat %u, BPM %d, SpO2 %u",
    //       sample->finger_detected, sample->cached, sample->beat_detected,
    //       sample->heart_rate_bpm, sample->spo2_percent);
    if (!sample->finger_detected)
        new_measurement = true;

    helios_subject_set_health_finger_detect(sample->finger_detected ? 1 : 0);

    if (sample->beat_detected || sample->cached || (!sample->finger_detected && !sample->cached))
        helios_subject_set_health_bpm(sample->heart_rate_bpm);

    if (sample->spo2_percent > 0 || (!sample->finger_detected && !sample->cached))
        helios_subject_set_health_oxygen(sample->spo2_percent);

    if (sample->finger_detected && new_measurement) {
        new_measurement = false;
        helios_subject_set_health_bpm(0);
        helios_subject_set_health_oxygen(0);
    }
}

static void helios_health_sensor_poll(lv_timer_t *timer)
{
    helios_max30100_sample_t sample;

    (void)timer;

    if (helios_max30100_update(&sample))
        helios_max30100_sample(&sample, NULL);
}

void health_events_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOADED)
    {
        rt_bool_t state = helios_max30100_begin(NULL);
        if (state) {
            rt_err_t err;

            helios_max30100_set_cache_timeout_ms(5000);
            err = helios_max30100_start_measuring(NULL, NULL);
            if (err == RT_EOK) {
                g_health_sensor_timer = lv_timer_create(helios_health_sensor_poll,
                                                        HELIOS_HEALTH_SENSOR_POLL_MS,
                                                        NULL);
                LOG_I("Starting max30100");

                helios_subject_set_health_bpm(0);
                helios_subject_set_health_oxygen(0);
                new_measurement = true;
            } else {
                LOG_E("Failed to start max30100 measurement, err=%d", err);
                helios_max30100_close();
            }
        } else {
            LOG_E("Failed to start max30100");
        }
        
    }

    if (code == LV_EVENT_SCREEN_UNLOAD_START)
    {
        LOG_I("Closing max30100");
        if (g_health_sensor_timer) {
            lv_timer_delete(g_health_sensor_timer);
            g_health_sensor_timer = NULL;
        }
        helios_max30100_stop_measuring();
        helios_max30100_close();
        
    }
}

uint32_t helios_screen_get_timeout_ms(void)
{
    return g_screen_timeout_ms;
}

void helios_screen_set_awake_override(uint8_t enabled)
{
    g_screen_awake_override = enabled ? 1 : 0;
    if (g_screen_awake_override)
        helios_screen_wake();
    else
        lv_display_trigger_activity(NULL);

    LOG_I("screen awake override %u", g_screen_awake_override);
}

uint8_t helios_screen_awake_override(void)
{
    return g_screen_awake_override;
}

uint8_t helios_screen_is_on(void)
{
    return g_screen_on;
}

void helios_subject_screen_brightness_change(int32_t value)
{
    if (value < 1)
        value = 1;
    if (value > 100)
        value = 100;

    g_screen_brightness = (uint8_t)value;
    if (g_screen_on)
        helios_screen_apply_brightness();

    helios_prefs_save_screen_brightness(value);
}

void helios_subject_screen_timeout_change(int32_t value)
{
    static const uint32_t timeouts_ms[] = {
        5000U,
        10000U,
        20000U,
        30000U,
        0U,
    };

    if (value < 0)
        value = 0;
    if (value >= (int32_t)(sizeof(timeouts_ms) / sizeof(timeouts_ms[0])))
        value = (int32_t)(sizeof(timeouts_ms) / sizeof(timeouts_ms[0])) - 1;

    helios_screen_set_timeout_ms(timeouts_ms[value]);
    helios_prefs_save_screen_timeout(value);
}

static void helios_ui_refresh(lv_timer_t *timer)
{
    (void)timer;

    time_t now = time(RT_NULL);
    struct tm *tm_now = localtime(&now);
    char time_now[16] = "--:--";
    if (tm_now)
    {
        rt_snprintf(time_now, sizeof(time_now), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);
        helios_subject_set_time_string(time_now);

        helios_subject_set_time_hour(tm_now->tm_hour);
        helios_subject_set_time_minute(tm_now->tm_min);
        helios_subject_set_time_seconds(tm_now->tm_sec);
        helios_subject_set_time_day(tm_now->tm_mday);
        helios_subject_set_time_month(tm_now->tm_mon + 1);
        helios_subject_set_time_year(tm_now->tm_year + 1900);
        helios_subject_set_time_weekday(tm_now->tm_wday);
    }

    helios_subject_set_system_connection(g_ble_connected);

    char battery_text[32];
    helios_subject_set_battery_percent(g_battery_percent);
    if (g_battery_mv)
        helios_subject_set_battery_voltage(g_battery_mv / 1000.0f);
    else
        rt_snprintf(battery_text, sizeof(battery_text), "Battery --%%");
    (void)battery_text;
}

static void helios_screen_idle_poll(lv_timer_t *timer)
{
    (void)timer;

    if (!g_screen_on)
    {
        if (helios_button_take_press(HELIOS_BUTTON_PA34) ||
            helios_button_take_press(HELIOS_BUTTON_PA11) ||
            helios_input_active())
            helios_screen_wake();
        return;
    }

    if (helios_button_take_press(HELIOS_SCREEN_SLEEP_BUTTON))
    {
        helios_screen_power(0);
        return;
    }

    if (helios_input_active())
        lv_display_trigger_activity(NULL);

    if (g_screen_awake_override || g_screen_timeout_ms == 0)
        return;

    if (lv_display_get_inactive_time(NULL) >= g_screen_timeout_ms)
        helios_screen_power(0);
}

static void helios_battery_poll(lv_timer_t *timer)
{
    (void)timer;

    helios_battery_t battery;
    if (helios_battery_read(&battery) == RT_EOK)
        helios_ui_set_battery(battery.percent, battery.millivolts);
}

static void helios_ui_entry(void *parameter)
{
    (void)parameter;

    set_date(2026, 1, 1);
    set_time(12, 0, 0);

    rt_err_t err = littlevgl2rtt_init(LCD_DEVICE_NAME);
    RT_ASSERT(err == RT_EOK);

    helios_ui_init("");
    g_ui_ready = 1;
    lv_timer_create(helios_ui_refresh, 1000, RT_NULL);
    lv_timer_create(helios_screen_idle_poll, HELIOS_SCREEN_IDLE_POLL_MS, RT_NULL);
    lv_timer_create(helios_battery_poll, 10000, RT_NULL);

    helios_notifications_clear();

    helios_subject_set_screen_timeout(4);
    helios_subject_set_screen_brightness(30);

    // helios_subject_set_chronos_esp_version("1.0.0");
    // helios_subject_set_chronos_app_version("1.0.0");
    helios_subject_set_firmware_version("v1.0.0");
    helios_subject_set_board_oem("SIFLI");
    helios_subject_set_board_name("SF32");
    helios_subject_set_board_type("SF32LB52");
    helios_subject_set_board_ram("576KB");
    helios_subject_set_board_psram("8MB");
    helios_subject_set_board_flash("16MB");
    helios_subject_set_os_name("RT-Thread");
    char os_version[16];
    rt_snprintf(os_version,
                sizeof(os_version),
                "v%ld.%ld.%ld",
                (long)RTT_VERSION,
                (long)RTT_SUBVERSION,
                (long)RTT_REVISION);
    helios_subject_set_os_version(os_version);

    helios_prefs_init();
    helios_prefs_apply_saved_ui_settings();

    helios_screen_wake();

    while (1)
    {
        int delay_ms = lv_timer_handler();
        rt_thread_mdelay(delay_ms > 0 ? delay_ms : 5);
    }
}

static int helios_ui_thread_init(void)
{
    rt_err_t err = rt_thread_init(&g_ui_thread, "helios_ui", helios_ui_entry, RT_NULL,
                                  g_ui_stack, sizeof(g_ui_stack),
                                  RT_THREAD_PRIORITY_MIDDLE, RT_THREAD_TICK_DEFAULT);
    if (err != RT_EOK)
        return RT_ERROR;

    rt_thread_startup(&g_ui_thread);
    return RT_EOK;
}
INIT_APP_EXPORT(helios_ui_thread_init);
