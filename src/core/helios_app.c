#include "helios_app.h"

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "littlevgl2rtt.h"
#include "lvgl.h"
#include "helios_ui/helios_ui.h"
#include "apps/mpesa/mpesa_notifications.h"
#include "helios_ui/custom/apps/app_manager.h"
#include "helios_ui/custom/apps/notifications/notifications.h"
#ifndef _WIN32
#include "drv_lcd.h"
#endif

#include "helios_ble.h"
#include "helios_chronos.h"
#include "helios_platform.h"
#include "helios_battery.h"
#include "helios_sounds.h"
#include "helios_alarms.h"
#include "helios_ui_dispatch.h"
#include "helios_display.h"
#include "helios_power.h"
#include "helios_prefs.h"
#include "helios_time.h"
#include "helios_storage.h"
#include "helios_storage_ui.h"
#include "helios_weather_ui.h"
#include "helios_notification_ui.h"
#include "helios_user.h"

#define LOG_TAG "helios.app"
#include "log.h"

#define HELIOS_UI_STACK_SIZE (32 * 1024)
#define LCD_DEVICE_NAME "lcd"
#define HELIOS_SCREEN_TIMEOUT_DEFAULT_MS 5000U
#define HELIOS_SCREEN_IDLE_POLL_MS 250U
#define HELIOS_WRIST_VIEW_TIMEOUT_MS 5000U
#define HELIOS_SLEEP_HOME_DELAY_MS 500U
#define HELIOS_HEALTH_SENSOR_POLL_MS 20U
#define HELIOS_BUTTON_PA34 0
#define HELIOS_BUTTON_PA11 1
#define HELIOS_SCREEN_SLEEP_BUTTON HELIOS_BUTTON_PA34
#define HELIOS_PENDING_NOTIFICATIONS 4

#include "helios_features.h"

#if HELIOS_HAS_BMI270
#include "helios_bmi270.h"
#endif

#ifndef HELIOS_BOARD_NAME
#define HELIOS_BOARD_NAME "unknown"
#endif

#if HELIOS_HAS_MAX30100
#include "helios_max30100.h"
#endif

static struct rt_thread g_ui_thread;
ALIGN(RT_ALIGN_SIZE)
static uint8_t g_ui_stack[HELIOS_UI_STACK_SIZE];

static volatile uint8_t g_ble_connected;
static volatile uint8_t g_battery_percent = 0;
static volatile uint32_t g_battery_mv = 0;
static rt_err_t g_battery_read_error = -RT_ERROR;
static volatile uint8_t g_ui_ready;
static volatile uint8_t g_restart_requested;
static volatile uint32_t g_screen_timeout_ms = HELIOS_SCREEN_TIMEOUT_DEFAULT_MS;
static volatile uint8_t g_screen_awake_override;
static volatile uint8_t g_screen_on;
static uint8_t g_sleep_home_pending;
static uint32_t g_screen_off_tick;
static uint32_t g_screen_button_sequence;
static uint8_t g_screen_brightness = 30;
static lv_image_dsc_t g_nav_icon_dsc;
static uint32_t g_nav_crc = 0xFFFFFFFFu;
static uint8_t g_nav_icon_data[CHRONOS_ICON_DATA_SIZE];
static bool g_nav_active;
static bool g_nav_open_pending;
static bool g_nav_wake_locked;
static bool g_nav_unattended;
static uint32_t g_nav_button_sequence;
#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
static rt_tick_t g_notification_wrist_open_tick;
static rt_tick_t g_screen_wake_tick;
#endif
#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
static bool g_wrist_view_active;
static rt_tick_t g_wrist_view_started;
#endif
static lv_obj_t *g_call_screen;
static lv_obj_t *g_call_return_screen;

typedef struct {
    const void *icon;
    uint32_t preview_generation;
    bool received_while_awake;
    char app[CHRONOS_TEXT_SMALL_SIZE];
    char title[HELIOS_NOTIFICATION_TITLE_MAX];
    char message[HELIOS_NOTIFICATION_MESSAGE_MAX];
} helios_pending_notification_t;

static helios_pending_notification_t g_pending_notifications[HELIOS_PENDING_NOTIFICATIONS];
static uint8_t g_pending_notification_head;
static uint8_t g_pending_notification_count;
static volatile uint8_t g_pending_notification_async;
/* Tag arrivals before UI dispatch so delayed packets keep their sleep window. */
static uint32_t g_notification_preview_generation;
static bool g_capture_wrist_notifications;

static volatile uint8_t g_pending_connection_async;
static bool g_pending_connected;

static volatile uint8_t g_pending_call_async;
static bool g_pending_call_active;
static bool g_pending_call_started;
static char g_pending_caller[CHRONOS_TEXT_LARGE_SIZE];

static volatile uint8_t g_pending_music_async;
static chronos_music_info_t g_pending_music;

static volatile uint8_t g_pending_nav_async;
static chronos_navigation_t g_pending_nav;
static bool g_pending_nav_started;

static volatile uint8_t g_pending_nav_icon_async;
static uint8_t g_pending_nav_icon[CHRONOS_ICON_DATA_SIZE];
static uint32_t g_pending_nav_icon_crc;

static volatile uint8_t g_pending_weather_async;
static volatile uint8_t g_pending_contacts_async, g_pending_qr_async;
static volatile uint32_t g_pending_qr_mask;
static volatile uint8_t g_pending_user_async;
static uint32_t g_pending_user_a, g_pending_user_b;
static volatile uint8_t g_pending_phone_async;
static uint8_t g_pending_phone_mask;
static chronos_phone_info_t g_pending_phone;
static uint32_t g_activity_steps;

#if HELIOS_HAS_MAX30100
static bool new_measurement = true;
static lv_timer_t *g_health_sensor_timer;
#endif

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

#if !HELIOS_HAS_LOW_POWER
static void helios_screen_set_panel_brightness(uint8_t brightness)
{
    rt_device_t lcd = helios_lcd_device();
    if (lcd)
        rt_device_control(lcd, RTGRAPHIC_CTRL_SET_BRIGHTNESS, &brightness);
}
#endif

static uint8_t helios_screen_restore_home(void)
{
    if (!g_sleep_home_pending)
        return 0;

    lv_obj_t *home = screen_home();
    if (!home)
        return 0;
    g_sleep_home_pending = 0;
    /* Unload and delete the previous app so sensor/page cleanup still runs.
     * No transition animation is needed while the display is dark. */
    helios_screen_load_transition(home, LV_SCR_LOAD_ANIM_NONE, 0, true,
                                  HELIOS_SCREEN_TRANSITION_DEFAULT);
    return 1;
}

typedef enum {
    HELIOS_SCREEN_WAKE,
    HELIOS_SCREEN_WRIST_WAKE,
    HELIOS_SCREEN_SLEEP,
    HELIOS_SCREEN_AUTO_SLEEP,
} helios_screen_action_t;

static void helios_chronos_notification_async(void *user_data);

static void helios_screen_update_notification_window(helios_screen_action_t action)
{
    bool keep_preview = action == HELIOS_SCREEN_WRIST_WAKE;
    rt_base_t level = rt_hw_interrupt_disable();
    if (!keep_preview && ++g_notification_preview_generation == 0)
        ++g_notification_preview_generation;
    g_capture_wrist_notifications = action == HELIOS_SCREEN_AUTO_SLEEP;
    if (action == HELIOS_SCREEN_SLEEP || action == HELIOS_SCREEN_AUTO_SLEEP)
        g_screen_on = 0;
    rt_hw_interrupt_enable(level);

    if (!keep_preview)
        helios_notification_preview_reset();
}

static bool helios_show_notification_preview(void)
{
    if (!helios_notification_preview_show())
        return false;

#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
    g_notification_wrist_open_tick = rt_tick_get();
#endif
    return true;
}

#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
static void helios_wrist_view_start(void)
{
    g_wrist_view_active = true;
    g_wrist_view_started = rt_tick_get();
}
#endif

static void helios_screen_power(helios_screen_action_t action)
{
    lv_display_t *display = lv_display_get_default();

    if (action == HELIOS_SCREEN_WAKE || action == HELIOS_SCREEN_WRIST_WAKE)
    {
        if (g_screen_on)
            return;

        g_screen_button_sequence = helios_button_press_sequence();
#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
        g_screen_wake_tick = rt_tick_get();
#endif
        if (helios_power_set_display(1) != RT_EOK)
        {
            LOG_E("display wake failed");
            return;
        }

#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
        helios_bmi270_set_gesture_polling(helios_subject_get_gesture_navigation() != 0);
#endif
        helios_screen_update_notification_window(action);
        /* A quick wake replaces the previous app even before the delayed reset. */
        uint8_t restored_home = helios_screen_restore_home();
        bool preview = false;
        if (action == HELIOS_SCREEN_WRIST_WAKE) {
            /* Include any messages received while dark that are still queued. */
            helios_chronos_notification_async(NULL);
            preview = helios_show_notification_preview();
        }
#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
        if (action == HELIOS_SCREEN_WRIST_WAKE && g_screen_timeout_ms != 0 && !g_screen_awake_override)
            helios_wrist_view_start();
#endif
        if (display && (restored_home || preview || HELIOS_HAS_LOW_POWER))
        {
            lv_obj_invalidate(lv_screen_active());
            lv_refr_now(display);
        }
        rt_base_t level = rt_hw_interrupt_disable();
        g_screen_on = 1;
        rt_hw_interrupt_enable(level);
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

    if (helios_power_set_display(0) != RT_EOK)
    {
        LOG_E("display sleep failed");
        return;
    }

    helios_screen_update_notification_window(action);
#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
    g_wrist_view_active = false;
#endif
#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
    helios_bmi270_set_gesture_polling(0);
#endif
    helios_sounds_ringtone(false);
    g_nav_unattended = false;
    g_sleep_home_pending = 1;
    g_screen_off_tick = lv_tick_get();
    if (display)
        lv_display_trigger_activity(display);
#if !HELIOS_HAS_LOW_POWER
    helios_screen_set_panel_brightness(0);
#endif
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

static bool helios_screen_has_name(const char *expected)
{
    if (lv_display_get_screen_loading(NULL))
        return false;

    lv_obj_t *screen = lv_screen_active();
    const char *name = screen ? lv_obj_get_name(screen) : NULL;
    return name && lv_streq(name, expected);
}

static void helios_notification_added(const helios_pending_notification_t *pending)
{
    if (!pending->preview_generation || pending->preview_generation != g_notification_preview_generation)
        return;

    uint32_t count = helios_notifications_count();
    const helios_notification_t *notification = count ? helios_notifications_get(count - 1) : NULL;
    if (!notification)
        return;

    if (!pending->received_while_awake) {
        helios_notification_preview_add(notification->id);
    } else if (g_screen_on && helios_screen_has_name("home_#") && !helios_input_active()) {
        helios_notification_preview_reset();
        helios_notification_preview_add(notification->id);
        helios_show_notification_preview();
    }
}

static void helios_chronos_add_notification(const helios_pending_notification_t *pending)
{
    time_t now = time(RT_NULL);
    struct tm *tm_now = localtime(&now);
    char time_now[16] = "--:--";
    bool is_mpesa;
    if (tm_now)
        rt_snprintf(time_now, sizeof(time_now), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);

    is_mpesa = helios_mpesa_handle_notification(pending->app, pending->title, pending->message);

    if (helios_notifications_add(is_mpesa ? icon_mpesa_watch_32 : pending->icon,
                                 is_mpesa ? "MPESA" : pending->title, time_now, pending->message))
        helios_notification_added(pending);
    helios_sounds_notification();
}

static void helios_chronos_notification_async(void *user_data)
{
    (void)user_data;

    unsigned budget = HELIOS_PENDING_NOTIFICATIONS;
    while (budget--) {
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

        helios_chronos_add_notification(&pending);
    }
    /* A sustained stream must still give timers, touch and other work time. */
    if (!helios_ui_dispatch_post(helios_chronos_notification_async)) {
        rt_base_t level = rt_hw_interrupt_disable();
        g_pending_notification_async = 0;
        rt_hw_interrupt_enable(level);
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
    g_pending_notifications[index].received_while_awake = g_screen_on;
    g_pending_notifications[index].preview_generation = (g_screen_on || g_capture_wrist_notifications)
                                                        ? g_notification_preview_generation : 0;
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

    if (!helios_ui_dispatch_post(helios_chronos_notification_async)) {
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

static void helios_call_dismiss(void)
{
    if (!g_call_screen || lv_screen_active() != g_call_screen)
        return;

    lv_obj_t *screen = g_call_return_screen;
    if (!screen || !lv_obj_is_valid(screen))
        screen = screen_home();
    if (screen) {
        g_call_return_screen = NULL;
        helios_screen_load_transition(screen, LV_SCR_LOAD_ANIM_NONE, 0, true,
                                      HELIOS_SCREEN_TRANSITION_DEFAULT);
    }
}

static void helios_call_screen_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_SCREEN_LOADED) {
        helios_sounds_ringtone(true);
    } else if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        helios_sounds_ringtone(false);
    } else if (code == LV_EVENT_DELETE && lv_event_get_target(event) == g_call_screen) {
        helios_sounds_ringtone(false);
        helios_screen_wake_lock_release(g_call_screen);
        g_call_screen = NULL;
        if (g_call_return_screen && lv_obj_is_valid(g_call_return_screen))
            lv_obj_delete_async(g_call_return_screen);
        g_call_return_screen = NULL;
    } else if (code == LV_EVENT_GESTURE) {
        lv_indev_t *indev = lv_indev_active();
        if (!indev)
            return;
        lv_dir_t direction = lv_indev_get_gesture_dir(indev);
        if (direction == LV_DIR_LEFT || direction == LV_DIR_RIGHT) {
            lv_indev_wait_release(indev);
            helios_call_dismiss();
        }
    }
}

static void helios_call_async(void *unused)
{
    (void)unused;
    char caller[sizeof(g_pending_caller)];
    rt_base_t level = rt_hw_interrupt_disable();
    bool active = g_pending_call_active;
    bool started = g_pending_call_started;
    rt_memcpy(caller, g_pending_caller, sizeof(caller));
    g_pending_call_started = false;
    g_pending_call_async = 0;
    rt_hw_interrupt_enable(level);

    if (!active) {
        helios_sounds_ringtone(false);
        helios_call_dismiss();
        return;
    }

    helios_subject_set_caller_name(caller);
    if (!started || g_call_screen)
        return;

    lv_obj_t *screen = call_create();
    if (!screen)
        return;

    helios_screen_wake();
    g_call_return_screen = lv_display_get_screen_loading(NULL);
    if (!g_call_return_screen)
        g_call_return_screen = lv_screen_active();
    g_call_screen = screen;
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(screen, helios_call_screen_event, LV_EVENT_ALL, NULL);
    helios_screen_load_transition(screen, LV_SCR_LOAD_ANIM_NONE, 0, false,
                                  HELIOS_SCREEN_TRANSITION_DEFAULT);
    helios_screen_wake_lock_acquire(screen, 0);
}

static bool helios_navigation_is_open(void)
{
    lv_obj_t *screen = lv_display_get_screen_loading(NULL);
    if (!screen)
        screen = lv_screen_active();

    /* Both the launcher and automatic opening use the generated screen. */
    const char *name = screen ? lv_obj_get_name(screen) : NULL;
    return name && strcmp(name, "navigation_#") == 0;
}

static void helios_navigation_update_wake_lock(void)
{
    bool hold = g_screen_on && g_nav_active && helios_navigation_is_open();
    if (!hold)
        g_nav_unattended = false;

    if (hold && !g_nav_wake_locked) {
        g_nav_wake_locked = helios_screen_wake_lock_acquire(&g_nav_wake_locked, 0);
    } else if (!hold && g_nav_wake_locked) {
        helios_screen_wake_lock_release(&g_nav_wake_locked);
        g_nav_wake_locked = false;
    }
}

static void helios_screen_note_interaction(void)
{
    g_nav_unattended = false;
#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
    g_wrist_view_active = false;
#endif
    helios_notification_preview_interact();
}

static void helios_screen_input_pressed(lv_event_t *event)
{
    (void)event;
    helios_screen_note_interaction();
}

static bool helios_navigation_open(bool automatic)
{
    uint32_t button_sequence = helios_button_press_sequence();
    bool input_active = helios_input_active();
    helios_screen_wake();
    if (helios_navigation_is_open())
        return true;

    for (uint32_t i = 0; i < helios_apps_count(); ++i) {
        const helios_app_t *app = helios_apps_get(i);
        if (strcmp(app->tag, "navigation") == 0) {
            /* Only an automatic opening can put the watch to sleep at route end. */
            g_nav_button_sequence = button_sequence;
            g_nav_unattended = automatic && !input_active;
            helios_apps_launch(app);
            return helios_navigation_is_open();
        }
    }
    return false;
}

static void helios_chronos_nav_async(void *user_data)
{
    (void)user_data;

    chronos_navigation_t nav;

    rt_base_t level = rt_hw_interrupt_disable();
    nav = g_pending_nav;
    bool started = g_pending_nav_started;
    g_pending_nav_started = false;
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

    bool active = nav.active && nav.is_navigation;
    bool sleep = !active && g_nav_active && g_nav_unattended &&
                 g_screen_on && helios_navigation_is_open() &&
                 !helios_input_active() && helios_button_press_sequence() == g_nav_button_sequence;

    g_nav_active = active;
    g_nav_open_pending = active && (started || g_nav_open_pending);
    helios_navigation_update_wake_lock();

    if (sleep && g_screen_timeout_ms != 0 && !g_screen_awake_override &&
        !helios_screen_wake_lock_active())
        helios_screen_power(HELIOS_SCREEN_SLEEP);
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
    helios_weather_render(time(NULL));
}

/* Producers only publish work; LVGL itself is confined to the UI thread. */
static void helios_chronos_schedule_async(helios_ui_work_t callback, volatile uint8_t *pending)
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

    if (!helios_ui_dispatch_post(callback)) {
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
    rt_base_t level = rt_hw_interrupt_disable();
    g_pending_connected = connected;
    bool clear_nav = !connected && g_pending_nav.active;
    if (clear_nav) {
        g_pending_nav = (chronos_navigation_t){
            .eta = "Navigation",
            .title = "Chronos",
            .duration = "Disconnected",
        };
        g_pending_nav_started = false;
    }
    rt_hw_interrupt_enable(level);

    helios_chronos_schedule_async(helios_chronos_connection_async, &g_pending_connection_async);
    if (clear_nav)
        helios_chronos_schedule_async(helios_chronos_nav_async, &g_pending_nav_async);
    if (!connected)
        helios_app_call_update(NULL, false);
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

void helios_app_call_update(const char *caller, bool active)
{
    rt_base_t level = rt_hw_interrupt_disable();
    if (active && !g_pending_call_active) {
        g_pending_call_started = true;
        rt_snprintf(g_pending_caller, sizeof(g_pending_caller), "%s", "Unknown caller");
    }
    if (active && caller && caller[0])
        rt_snprintf(g_pending_caller, sizeof(g_pending_caller), "%s", caller);
    if (!active)
        g_pending_call_started = false;
    g_pending_call_active = active;
    rt_hw_interrupt_enable(level);

    helios_chronos_schedule_async(helios_call_async, &g_pending_call_async);
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

static void helios_chronos_contacts_async(void *unused)
{
    (void)unused;
    g_pending_contacts_async = 0;
    helios_storage_ui_sync_contacts();
}

static void helios_chronos_qr_async(void *unused)
{
    (void)unused;
    rt_base_t level = rt_hw_interrupt_disable();
    uint32_t mask = g_pending_qr_mask;
    g_pending_qr_mask = 0;
    g_pending_qr_async = 0;
    rt_hw_interrupt_enable(level);
    for (uint32_t i = 0; i < CHRONOS_QR_SIZE; i++)
        if (mask & (1UL << i)) helios_storage_ui_sync_qr(i);
}

static void helios_activity_progress_refresh(void)
{
    uint32_t target = helios_user_get().target_steps;
    uint64_t progress = target ? (uint64_t)g_activity_steps * 100 / target : 0;
    helios_subject_set_activity_progress(progress > 100 ? 100 : (int32_t)progress);
}

static void helios_chronos_user_async(void *unused)
{
    (void)unused;
    rt_base_t level = rt_hw_interrupt_disable();
    uint32_t a = g_pending_user_a, b = g_pending_user_b;
    g_pending_user_async = 0;
    rt_hw_interrupt_enable(level);
    helios_storage_ui_sync_user(a, b);
    helios_activity_progress_refresh();
    uint32_t kcal = helios_user_calories(g_activity_steps);
    helios_subject_set_activity_kcal(kcal > INT32_MAX ? INT32_MAX : (int32_t)kcal);
}

static void helios_chronos_phone_async(void *unused)
{
    (void)unused;
    rt_base_t level = rt_hw_interrupt_disable();
    chronos_phone_info_t phone = g_pending_phone;
    uint8_t mask = g_pending_phone_mask;
    g_pending_phone_mask = 0;
    g_pending_phone_async = 0;
    rt_hw_interrupt_enable(level);
    if (mask & 1u) {
        helios_subject_set_phone_battery(phone.battery_level);
        helios_subject_set_phone_charging(phone.is_charging ? 1 : 0);
    }
    if (mask & 2u) {
        helios_subject_set_chronos_app_version(phone.app_version);
        helios_subject_set_chronos_app_code(phone.app_code);
    }
    if (mask & 4u) {
        helios_subject_set_phone_manufacturer(phone.manufacturer);
        helios_subject_set_phone_model(phone.model);
        helios_subject_set_phone_sdk(phone.sdk_version);
    }
}

void helios_app_chronos_config(chronos_config_t config, uint32_t a, uint32_t b)
{
    switch (config) {
    case CHRONOS_CONFIG_HOURLY:
        helios_sounds_set_hourly(b != 0);
        break;
    case CHRONOS_CONFIG_SLEEP: {
        unsigned start_hour = (b >> 24) & 0xffu, start_minute = (b >> 16) & 0xffu;
        unsigned end_hour = (b >> 8) & 0xffu, end_minute = b & 0xffu;
        if (start_hour < 24 && end_hour < 24 && start_minute < 60 && end_minute < 60)
            helios_sounds_set_sleep(a != 0, start_hour * 60 + start_minute,
                                   end_hour * 60 + end_minute);
        break;
    }
    case CHRONOS_CONFIG_ALARM:
        helios_alarms_request(a, b);
        break;
    case CHRONOS_CONFIG_24_HOUR:
        helios_time_request_24_hour(b != 0);
        break;
    case CHRONOS_CONFIG_RESET:
        /* No allocation or reset in the BLE callback. The UI thread snapshots
         * saved state and lets storage finish writing before restarting. */
        g_restart_requested = 1;
        break;
    case CHRONOS_CONFIG_USER: {
        rt_base_t level = rt_hw_interrupt_disable();
        g_pending_user_a = a;
        g_pending_user_b = b;
        rt_hw_interrupt_enable(level);
        helios_chronos_schedule_async(helios_chronos_user_async, &g_pending_user_async);
        break;
    }
    case CHRONOS_CONFIG_CONTACT:
        if (a == 1u || (a == 0u && (b & 0xffu) == 0u))
            helios_chronos_schedule_async(helios_chronos_contacts_async, &g_pending_contacts_async);
        break;
    case CHRONOS_CONFIG_QR:
        if (a == 0u && b < CHRONOS_QR_SIZE) {
            rt_base_t level = rt_hw_interrupt_disable();
            g_pending_qr_mask |= 1UL << b;
            rt_hw_interrupt_enable(level);
            helios_chronos_schedule_async(helios_chronos_qr_async, &g_pending_qr_async);
        }
        break;
    case CHRONOS_CONFIG_NAV_DATA: {
        const chronos_navigation_t *nav = chronos_core_get_navigation();
        if (!nav)
            return;

        rt_base_t level = rt_hw_interrupt_disable();
        /* Latch a new route even if multiple packets arrive before UI dispatch. */
        if (!nav->active || !nav->is_navigation)
            g_pending_nav_started = false;
        else if (!g_pending_nav.active || !g_pending_nav.is_navigation)
            g_pending_nav_started = true;
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
    case CHRONOS_CONFIG_PHONE_BATTERY:
    case CHRONOS_CONFIG_APP: {
        const chronos_phone_info_t *phone = chronos_core_get_phone_info();
        uint8_t mask = config == CHRONOS_CONFIG_PHONE_BATTERY ? 1u :
                       b == 0u ? 2u : b == 1u ? 4u : 0u;
        if (!phone || !mask)
            break;
        rt_base_t level = rt_hw_interrupt_disable();
        g_pending_phone = *phone;
        g_pending_phone_mask |= mask;
        rt_hw_interrupt_enable(level);
        helios_chronos_schedule_async(helios_chronos_phone_async, &g_pending_phone_async);
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
    helios_screen_power(HELIOS_SCREEN_WAKE);
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

#if HELIOS_HAS_MAX30100
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
#endif

void health_events_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

#if HELIOS_HAS_MAX30100
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
#else
    if (code == LV_EVENT_SCREEN_LOADED)
    {
        helios_subject_set_health_finger_detect(0);
        helios_subject_set_health_bpm(0);
        helios_subject_set_health_oxygen(0);
        LOG_W("MAX30100 disabled for board %s", HELIOS_BOARD_NAME);
    }

    (void)code;
#endif
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
    lv_display_trigger_activity(NULL);
}

void on_slider_release(lv_event_t *e)
{
    if (!e || lv_event_get_code(e) != LV_EVENT_RELEASED)
        return;

    const char *data = lv_event_get_user_data(e);
    if (data && lv_streq("brightness", data))
        helios_prefs_save_screen_brightness(g_screen_brightness);
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

static void helios_diagnostic_battery_refresh(void)
{
    char text[UI_SUBJECT_STRING_LENGTH];
    char adc[40];
    char charge[48];
    int level = helios_charge_detect_level();
    if (g_battery_read_error == RT_EOK)
        rt_snprintf(adc, sizeof(adc), "ADC: OK");
    else
        rt_snprintf(adc, sizeof(adc), "ADC error: %d", g_battery_read_error);
#if HELIOS_HAS_CHARGE_DETECT
    if (level >= 0)
        rt_snprintf(charge, sizeof(charge), "Charge detect PA%d: %s (%d)",
                    HELIOS_CHARGE_DETECT_PIN, level ? "HIGH" : "LOW", level);
    else
#endif
        rt_snprintf(charge, sizeof(charge), "Charge detect: unavailable (%d)", level);

    if (g_battery_mv)
        rt_snprintf(text, sizeof(text), "Voltage: %lu mV\nLevel: %u%%\n%s\n%s",
                    (unsigned long)g_battery_mv, g_battery_percent, adc, charge);
    else
        rt_snprintf(text, sizeof(text), "Voltage: unavailable\nLevel: unavailable\n%s\n%s", adc, charge);
    helios_subject_set_diagnostic_battery(text);
}

static lv_obj_t *g_fitness_steps_chart;

static void helios_fitness_steps_update(lv_obj_t *chart, time_t now)
{
    lv_chart_series_t *series = lv_chart_get_series_next(chart, NULL);
    if (!series)
        return;

    uint32_t bins[HELIOS_STEPS_HOURS_PER_DAY];
    int32_t values[HELIOS_STEPS_HOURS_PER_DAY];
    helios_storage_hourly_steps(now, bins);
    /* LVGL multiplies bar values by the chart height in a signed integer. */
    int32_t display_height = lv_display_get_vertical_resolution(lv_obj_get_display(chart));
    uint32_t limit = INT32_MAX / LV_MAX(1, display_height);
    int32_t peak = 100;
    for (unsigned i = 0; i < HELIOS_STEPS_HOURS_PER_DAY; ++i) {
        values[i] = (int32_t)LV_MIN(bins[i], limit);
        peak = LV_MAX(peak, values[i]);
    }

    lv_chart_set_point_count(chart, HELIOS_STEPS_HOURS_PER_DAY);
    int32_t *points = lv_chart_get_series_y_array(chart, series);
    if (points && memcmp(points, values, sizeof(values)) != 0) {
        memcpy(points, values, sizeof(values));
        lv_chart_set_x_start_point(chart, series, 0);
        lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, peak);
        lv_chart_refresh(chart);
    }
}

static void helios_fitness_chart_deleted(lv_event_t *event)
{
    if (lv_event_get_target(event) == g_fitness_steps_chart)
        g_fitness_steps_chart = NULL;
}

static void helios_fitness_before_draw(lv_event_t *event)
{
    lv_obj_t *screen = lv_display_get_screen_active(lv_event_get_target(event));
    const char *name = screen ? lv_obj_get_name(screen) : NULL;
    if (!name || !lv_streq(name, "fitness_#"))
        return;

    lv_obj_t *chart = lv_obj_find_by_name(screen, "steps_chart");
    if (!chart || chart == g_fitness_steps_chart)
        return;

    g_fitness_steps_chart = chart;
    lv_obj_add_event_cb(chart, helios_fitness_chart_deleted, LV_EVENT_DELETE, NULL);
    helios_fitness_steps_update(chart, time(NULL));
}

static void helios_ui_refresh(lv_timer_t *timer)
{
    helios_storage_ui_poll();
    (void)timer;

    time_t now = time(RT_NULL);
    if (g_fitness_steps_chart && lv_obj_get_screen(g_fitness_steps_chart) == lv_screen_active())
        helios_fitness_steps_update(g_fitness_steps_chart, now);
    helios_weather_poll(now);
    struct tm *tm_now = localtime(&now);
    helios_time_publish(tm_now);
    helios_sounds_poll(now, tm_now);
    if (tm_now)
    {
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
    helios_diagnostic_battery_refresh();
}

static bool helios_screen_can_auto_sleep(void)
{
    return !g_screen_awake_override && g_screen_timeout_ms != 0 && !helios_screen_wake_lock_active();
}

#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
static bool helios_wrist_notifications(void)
{
    bool opening = helios_screen_has_name("home_#");
    if (!opening && !helios_screen_has_name("notifications_#"))
        return false;

    lv_obj_t *screen = opening ? screen_notifications() : lv_screen_active();
    if (!screen)
        return false;

    if (opening)
        helios_screen_load_transition(screen, LV_SCR_LOAD_ANIM_NONE, 0, true,
                                      HELIOS_SCREEN_TRANSITION_DEFAULT);

    lv_obj_t *list = get_list_from_wd(screen, "notifications_list");
    if (!list)
        return opening;

    lv_obj_update_layout(list);
    if (opening) {
        lv_obj_scroll_to_y(list, 0, LV_ANIM_OFF);
    } else {
        int32_t page = lv_obj_get_content_height(list);
        int32_t remaining = lv_obj_get_scroll_bottom(list);
        if (page > 0 && remaining > 0)
            lv_obj_scroll_to_y(list, lv_obj_get_scroll_y(list) + LV_MIN(page, remaining), LV_ANIM_OFF);
    }
    return true;
}

static lv_obj_t *helios_widgets_container(lv_obj_t *screen)
{
    lv_obj_t *panel = lv_obj_get_child(screen, 0);
    return panel ? wd_panel_get_container(panel) : NULL;
}

static bool helios_wrist_open_widgets(void)
{
    lv_obj_t *screen = screen_widgets();
    lv_obj_t *container = screen ? helios_widgets_container(screen) : NULL;
    lv_obj_t *first = container ? lv_obj_get_child(container, 0) : NULL;
    if (!first) {
        if (screen)
            lv_obj_delete(screen);
        return false;
    }

    lv_obj_scroll_to_view(first, LV_ANIM_OFF);
    helios_screen_load_transition(screen, LV_SCR_LOAD_ANIM_NONE, 0, true,
                                  HELIOS_SCREEN_TRANSITION_DEFAULT);
    return true;
}

static bool helios_wrist_next_widget(void)
{
    if (!helios_screen_has_name("widgets_#"))
        return false;

    lv_obj_t *container = helios_widgets_container(lv_screen_active());
    if (!container || lv_obj_is_scrolling(container))
        return false;

    /* Use the centered page so a preceding touch swipe sets the same position. */
    lv_obj_update_layout(container);
    lv_area_t viewport;
    lv_obj_get_content_coords(container, &viewport);
    int32_t center = (viewport.x1 + viewport.x2) / 2;
    int32_t closest = INT32_MAX;
    uint32_t current = 0;
    uint32_t count = lv_obj_get_child_count(container);
    if (count < 2)
        return false;

    for (uint32_t i = 0; i < count; ++i) {
        lv_area_t area;
        lv_obj_get_coords(lv_obj_get_child(container, (int32_t)i), &area);
        int32_t distance = LV_ABS((area.x1 + area.x2) / 2 - center);
        if (distance < closest) {
            closest = distance;
            current = i;
        }
    }

    uint32_t next = (current + 1) % count;
    lv_obj_scroll_to_view(lv_obj_get_child(container, (int32_t)next), LV_ANIM_OFF);
    return true;
}

static bool helios_wrist_pivot_up(void)
{
    if (lv_display_get_screen_loading(NULL))
        return false;

    if (helios_notification_preview_is_open()) {
        helios_notification_preview_dismiss();
        return true;
    }

    if (helios_screen_has_name("home_#"))
        return g_nav_active ? helios_navigation_open(false) : helios_wrist_open_widgets();

    if (g_nav_active && helios_navigation_is_open())
        return helios_wrist_open_widgets();

    lv_obj_t *home = screen_home();
    if (!home)
        return false;

    helios_screen_load_transition(home, LV_SCR_LOAD_ANIM_NONE, 0, true,
                                  HELIOS_SCREEN_TRANSITION_DEFAULT);
    return true;
}

static void helios_screen_wrist_poll(void)
{
    static uint32_t last_gesture_events;
    static uint32_t last_pivot_up_events;
    helios_bmi270_sample_t sample;
    if (helios_bmi270_read(&sample) != RT_EOK)
        return;

    bool new_gesture = sample.wrist_gesture_events != last_gesture_events;
    bool new_pivot_up = sample.wrist_pivot_up_events != last_pivot_up_events;
    last_gesture_events = sample.wrist_gesture_events;
    last_pivot_up_events = sample.wrist_pivot_up_events;
    if ((!new_gesture && !new_pivot_up) || !helios_subject_get_gesture_navigation() || helios_input_active())
        return;

    rt_tick_t now = rt_tick_get();
    rt_tick_t wake_age = now - g_screen_wake_tick;
    rt_tick_t max_age = rt_tick_from_millisecond(750);
    rt_tick_t pivot_up_age = now - sample.wrist_pivot_up_tick;
    rt_tick_t age = now - sample.wrist_gesture_tick;

    if (new_pivot_up && pivot_up_age <= max_age && pivot_up_age < wake_age) {
        if (!helios_wrist_pivot_up())
            return;
    } else if (!new_gesture || age > max_age || age >= wake_age) {
        return;
    } else if (!helios_notification_preview_is_open()) {
        if (sample.wrist_gesture != HELIOS_BMI270_GESTURE_FLICK_IN ||
            (!helios_wrist_next_widget() && !helios_wrist_notifications()))
            return;
    } else if (helios_subject_get_screen_rtw() == 0 ||
               age >= (rt_tick_t)(now - g_notification_wrist_open_tick)) {
        return;
    } else if (sample.wrist_gesture == HELIOS_BMI270_GESTURE_FLICK_IN) {
        helios_notification_preview_next();
    } else if (sample.wrist_gesture == HELIOS_BMI270_GESTURE_FLICK_OUT) {
        helios_notification_preview_dismiss();
    } else {
        return;
    }

    helios_navigation_update_wake_lock();
    lv_display_trigger_activity(NULL);
#if HELIOS_IMU_RAISE_TO_WAKE
    if (g_wrist_view_active)
        helios_wrist_view_start();
#endif
}
#endif

static void helios_screen_idle_poll(lv_timer_t *timer)
{
    (void)timer;

#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
    /* Always consume, including while awake, so gestures cannot queue up. */
    uint8_t wrist_wake = helios_bmi270_take_wake();
#else
    uint8_t wrist_wake = 0;
#endif
    helios_navigation_update_wake_lock();
    if (!g_screen_on)
    {
        if (g_sleep_home_pending && lv_tick_elaps(g_screen_off_tick) >= HELIOS_SLEEP_HOME_DELAY_MS)
            helios_screen_restore_home();
        if (helios_button_take_press(HELIOS_BUTTON_PA34) ||
            helios_button_take_press(HELIOS_BUTTON_PA11) ||
            helios_input_active())
            helios_screen_wake();
        else if (wrist_wake && helios_subject_get_screen_rtw() != 0)
            helios_screen_power(HELIOS_SCREEN_WRIST_WAKE);
        return;
    }

    if (helios_button_take_press(HELIOS_SCREEN_SLEEP_BUTTON))
    {
        helios_screen_power(HELIOS_SCREEN_SLEEP);
        return;
    }

    uint32_t button_sequence = helios_button_press_sequence();
    if (helios_button_take_press(HELIOS_BUTTON_PA11) || helios_input_active() ||
        button_sequence != g_screen_button_sequence) {
        helios_screen_note_interaction();
        lv_display_trigger_activity(NULL);
    } else {
        helios_notification_list_mark_visible_read();
    }
    g_screen_button_sequence = button_sequence;

#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
    helios_screen_wrist_poll();
#endif

    bool preview_waiting = helios_notification_preview_poll();
#if HELIOS_HAS_BMI270 && HELIOS_IMU_RAISE_TO_WAKE
    if (g_wrist_view_active) {
        if (g_nav_active && helios_navigation_is_open())
            return;

        if (!helios_screen_can_auto_sleep() ||
            (!helios_notification_preview_is_open() && !helios_screen_has_name("home_#") &&
             !helios_screen_has_name("widgets_#") && !helios_screen_has_name("notifications_#"))) {
            g_wrist_view_active = false;
        } else {
            if ((rt_tick_t)(rt_tick_get() - g_wrist_view_started) >=
                rt_tick_from_millisecond(HELIOS_WRIST_VIEW_TIMEOUT_MS))
                helios_screen_power(HELIOS_SCREEN_AUTO_SLEEP);
            return;
        }
    }
#endif

    if (preview_waiting || !helios_screen_can_auto_sleep())
        return;

    if (lv_display_get_inactive_time(NULL) >= g_screen_timeout_ms)
        helios_screen_power(HELIOS_SCREEN_AUTO_SLEEP);
}

#if HELIOS_HAS_BMI270
static void helios_motion_poll(lv_timer_t *timer)
{
    helios_bmi270_sample_t sample;
    (void)timer;
    rt_err_t err = helios_bmi270_read(&sample);
    if (err == RT_EOK)
    {
        char text[UI_SUBJECT_STRING_LENGTH];
        char gyro[64];
        char tilt[64] = "";
        char activity[32], gesture[32];
        if (sample.temperature_valid)
            helios_subject_set_battery_temp(sample.temperature_c);
        if (HELIOS_IMU_STEPS)
        {
            helios_storage_record_steps(time(NULL), sample.steps);
            uint32_t total = helios_storage_ready() ? helios_storage_daily_steps() : sample.steps;
            g_activity_steps = total;
            helios_activity_progress_refresh();
            helios_subject_set_activity_steps(total > INT32_MAX ? INT32_MAX : (int32_t)total);
            uint32_t kcal = helios_user_calories(total);
            helios_subject_set_activity_kcal(kcal > INT32_MAX ? INT32_MAX : (int32_t)kcal);
        }
        if (sample.gyro_error)
            rt_snprintf(gyro, sizeof(gyro), "G error: %d", sample.gyro_error);
        else if (sample.gyro_enabled)
            rt_snprintf(gyro, sizeof(gyro), "G: %d,%d,%d", sample.gyro[0], sample.gyro[1], sample.gyro[2]);
        else
            rt_snprintf(gyro, sizeof(gyro), "G: off / starting");
        if (sample.activity_error)
            rt_snprintf(activity, sizeof(activity), "read error %d", sample.activity_error);
        else
            rt_snprintf(activity, sizeof(activity), "%s", !HELIOS_IMU_ACTIVITY ? "disabled" :
                !sample.activity_valid ? "starting" : helios_bmi270_activity_name(sample.activity));
        if (sample.wrist_gesture_error)
            rt_snprintf(gesture, sizeof(gesture), "read error %d", sample.wrist_gesture_error);
        else
            rt_snprintf(gesture, sizeof(gesture), "%s", HELIOS_IMU_WRIST_GESTURES ?
                helios_bmi270_gesture_name(sample.wrist_gesture) : "disabled");
#if HELIOS_IMU_DEBUG_ORIENTATION
        int16_t roll, pitch;
        if (helios_bmi270_orientation(&sample, &roll, &pitch) == RT_EOK)
            rt_snprintf(tilt, sizeof(tilt), "\nTilt: %d,%d (0.1 deg)", roll, pitch);
#endif
        rt_snprintf(text, sizeof(text),
                    "Wrist position (est.): %s\nAct: %s <- %s [%lu]\nNav: %s [%lu]\nPivot ups: %lu\nA: %d,%d,%d\n%s\nSteps: %lu%s (%lu events)\nWake: %lu%s%s",
                    helios_bmi270_focus_name(&sample), activity, helios_bmi270_activity_name(sample.previous_activity),
                    (unsigned long)sample.activity_changes, gesture, (unsigned long)sample.wrist_gesture_events,
                    (unsigned long)sample.wrist_pivot_up_events,
                    sample.accel[0], sample.accel[1], sample.accel[2], gyro,
                    (unsigned long)sample.steps, HELIOS_IMU_STEPS ? "" : " (off)",
                    (unsigned long)sample.step_events, (unsigned long)sample.wrist_wake_events,
                    HELIOS_IMU_RAISE_TO_WAKE ? "" : " (disabled)", tilt);
        helios_subject_set_diagnostic_bmi270(text);
    }
    else
    {
        char text[UI_SUBJECT_STRING_LENGTH];
        char probes[2][28];
        helios_bmi270_status_t status;
        helios_bmi270_get_status(&status);
        for (int i = 0; i < 2; i++)
        {
            if (!status.probe_attempted[i])
                rt_snprintf(probes[i], sizeof(probes[i]), "not probed");
            else if (status.probe_ack[i])
                rt_snprintf(probes[i], sizeof(probes[i]), "ID 0x%02x", status.probe_id[i]);
            else
                rt_snprintf(probes[i], sizeof(probes[i]), "no response");
        }
        rt_snprintf(text, sizeof(text),
                    "BMI270: %s\nStage: %s\nError: %d\n%s: %s, %lu Hz\n0x68: %s\n0x69: %s\nI2C %s 0x%02x n=%lu: %d\nAttempts: %lu",
                    status.error ? "unavailable (retrying)" : "initializing",
                    status.stage, status.error, HELIOS_BMI270_I2C_BUS,
                    status.bus_ready ? "found" : "missing", (unsigned long)status.bus_hz,
                    probes[0], probes[1], status.io_read ? "read" : "write", status.io_register,
                    (unsigned long)status.io_length, status.io_result, (unsigned long)status.attempts);
        helios_subject_set_diagnostic_bmi270(text);
    }
}
#endif

#if HELIOS_HAS_BMI270 && HELIOS_IMU_STEPS
static void helios_activity_sync(lv_timer_t *timer)
{
    (void)timer;
    helios_chronos_update_activity(g_activity_steps, helios_user_calories(g_activity_steps));
}
#endif

static void helios_battery_usage_refresh(void)
{
    helios_battery_runtime_t runtime;
    const char *value = "--";
    char text[32];
    if (helios_battery_estimate_remaining(time(NULL), &runtime))
    {
        switch (runtime.status)
        {
        case HELIOS_BATTERY_RUNTIME_AVAILABLE: {
            uint32_t hours = runtime.remaining_seconds / 3600;
            rt_snprintf(text, sizeof(text), "%lud %luh",
                        (unsigned long)(hours / 24), (unsigned long)(hours % 24));
            value = text;
            break;
        }
        case HELIOS_BATTERY_RUNTIME_LEARNING:
            value = "Estimating...";
            break;
        case HELIOS_BATTERY_RUNTIME_CHARGING:
            value = "Charging";
            break;
        default:
            break;
        }
    }
    helios_subject_set_battery_usage(value);
}

static void helios_battery_poll(lv_timer_t *timer)
{
    static rt_err_t last_error = RT_EOK;
    static uint8_t have_reading;
    (void)timer;

    helios_battery_t battery;
    rt_err_t err = helios_battery_read(&battery);
    g_battery_read_error = err;
    if (err == RT_EOK)
    {
        helios_battery_observe(time(NULL), &battery);
        helios_ui_set_battery(battery.percent, battery.millivolts);
        helios_subject_set_battery_percent(battery.percent);
        helios_subject_set_battery_voltage(battery.millivolts / 1000.0f);
        helios_subject_set_battery_charging(battery.charging);
        helios_chronos_update_battery(battery.percent, battery.charging);
        helios_storage_record_battery(time(NULL), battery.millivolts, battery.percent, battery.charging);
        if (!have_reading || last_error != RT_EOK)
            LOG_I("battery: %lu mV (%u%%)", (unsigned long)battery.millivolts, battery.percent);
        have_reading = 1;
    }
    else if (err != last_error)
    {
        LOG_W("battery read failed: %d; run battery_status for diagnostics", err);
    }
    last_error = err;
    helios_battery_usage_refresh();
    helios_diagnostic_battery_refresh();
}

void helios_diagnostic_screen_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *screen = lv_event_get_current_target(e);
    if (code == LV_EVENT_SCREEN_LOADED)
    {
        if (!helios_screen_wake_lock_acquire(screen, 0))
            LOG_W("Diagnostics could not acquire screen wake lock");
#if HELIOS_HAS_BMI270
        helios_bmi270_set_diagnostics(1);
        helios_motion_poll(NULL);
#endif
    }
    else if (code == LV_EVENT_SCREEN_UNLOAD_START || code == LV_EVENT_DELETE)
    {
        helios_screen_wake_lock_release(screen);
#if HELIOS_HAS_BMI270
        helios_bmi270_set_diagnostics(0);
#endif
    }
}

void on_diagnostic_click(lv_event_t *e)
{
    if (!e || lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    const char *data = lv_event_get_user_data(e);
    if (!data)
        return;
    rt_err_t err;
    if (lv_streq("vibration", data))
        err = helios_motor_test();
    else if (lv_streq("buzzer", data))
        err = helios_buzzer_test();
    else
        return;
    if (err != RT_EOK)
        LOG_W("diagnostic %s failed: %d", data, err);
}

static void helios_rtc_init_if_unset(void)
{
    rt_device_t rtc = rt_device_find("rtc");
    time_t now = 0;
    if (!rtc || rt_device_control(rtc, RT_DEVICE_CTRL_RTC_GET_TIME, &now) != RT_EOK)
    {
        LOG_W("RTC unavailable; leaving time unchanged");
        return;
    }

    struct tm calendar;
    /* Accept a plausible calendar in the RTC's supported 2020-2099 range.
     * The SDK already preserves the RTC across normal resets; do not rewrite
     * a retained calendar merely because the application has restarted. */
    if (localtime_r(&now, &calendar) && calendar.tm_year >= 120 && calendar.tm_year <= 199)
        return;

    struct tm fallback = {0};
    fallback.tm_year = 126;
    fallback.tm_mon = 0;
    fallback.tm_mday = 1;
    fallback.tm_hour = 12;
    fallback.tm_isdst = -1;
    time_t initial = mktime(&fallback);
    if (initial == (time_t)-1 ||
        rt_device_control(rtc, RT_DEVICE_CTRL_RTC_SET_TIME, &initial) != RT_EOK)
        LOG_W("RTC fallback initialization failed");
    else
        LOG_I("RTC was unset; initialized fallback time pending phone sync");
}

static void helios_ui_entry(void *parameter)
{
    (void)parameter;

    helios_power_init();

    helios_rtc_init_if_unset();

    rt_err_t err = littlevgl2rtt_init(LCD_DEVICE_NAME);
    RT_ASSERT(err == RT_EOK);

    for (lv_indev_t *indev = lv_indev_get_next(NULL); indev; indev = lv_indev_get_next(indev))
        lv_indev_add_event_cb(indev, helios_screen_input_pressed, LV_EVENT_PRESSED, NULL);

    err = helios_display_init_rotation();
    RT_ASSERT(err == RT_EOK);

    helios_ui_init("");
    /* Replace the preview value before the battery view binds to this subject. */
    lv_subject_init_float(&sb_battery_temp, NAN);
    lv_display_add_event_cb(lv_display_get_default(), helios_fitness_before_draw, LV_EVENT_REFR_START, NULL);
    helios_chronos_prepare();
    helios_storage_ui_init();
    helios_alarms_init();
    helios_subject_set_activity_steps(0);
    helios_subject_set_activity_kcal(0);
    helios_activity_progress_refresh();
    g_ui_ready = 1;
    lv_timer_create(helios_ui_refresh, 1000, RT_NULL);
    lv_timer_create(helios_screen_idle_poll, HELIOS_SCREEN_IDLE_POLL_MS, RT_NULL);
    lv_timer_create(helios_battery_poll, 10000, RT_NULL);
    helios_subject_set_battery_voltage(0.0f);
    helios_subject_set_battery_percent(0);
    helios_battery_poll(NULL);
    helios_subject_set_diagnostic_bmi270("BMI270: not fitted on this board");
#if HELIOS_HAS_BMI270
    helios_subject_set_activity_steps(0);
    lv_timer_create(helios_motion_poll, 1000, RT_NULL);
    helios_motion_poll(NULL);
#if HELIOS_IMU_STEPS
    lv_timer_create(helios_activity_sync, 10000, RT_NULL);
#endif
#endif

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
    helios_time_restore_format();
    helios_ui_refresh(NULL);

    helios_screen_wake();

    helios_sounds_startup();

    while (1)
    {
        helios_ui_dispatch_poll();
        if (g_nav_open_pending && !g_call_screen) {
            helios_navigation_open(true);
            helios_navigation_update_wake_lock();
            g_nav_open_pending = false;
        }
        time_t alarm_now = time(RT_NULL);
        struct tm alarm_calendar;
        const struct tm *alarm_time = localtime_r(&alarm_now, &alarm_calendar);
        if (helios_alarms_poll(alarm_time, helios_button_press_sequence())) {
            /* The dismissal press belongs to the alarm, not navigation. */
            while (helios_button_take_press(HELIOS_BUTTON_PA34)) {}
            while (helios_button_take_press(HELIOS_BUTTON_PA11)) {}
        }
        helios_sounds_ringtone_poll(lv_tick_get());
        int delay_ms = lv_timer_handler();
        if (g_restart_requested) {
            g_restart_requested = 0;
            LOG_I("BLE restart requested; saving data");
            helios_storage_ui_poll();
            if (helios_storage_flush()) {
                /* Allow the BLE receive callback and log output to complete. */
                rt_thread_mdelay(100);
                helios_platform_restart();
            } else {
                LOG_E("restart cancelled: storage flush failed");
            }
        }
#if HELIOS_HAS_LOW_POWER
        /* Service queued BLE/UI work, touch and wake detection at 4 Hz while
         * dark. A 5-33 ms LVGL refresh/animation deadline otherwise prevents
         * the SDK's >=30 ms DeepSleep policy from taking effect. */
        if (!g_screen_on)
            delay_ms = HELIOS_SCREEN_IDLE_POLL_MS;
#endif
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
