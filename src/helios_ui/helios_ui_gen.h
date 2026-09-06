/**
 * @file helios_ui_gen.h
 */

#ifndef LVGL_PRO_HELIOS_UI_GEN_H
#define LVGL_PRO_HELIOS_UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif



/* Prototypes for target functions, needed by responsive const definitions */

void helios_ui_set_target(uint32_t target);
uint32_t helios_ui_get_target(void);
bool helios_ui_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define HELIOS_UI_TARGET_UNDEFINED    (0 << 1)
#define HELIOS_UI_TARGET_TARGET_466R  (1 << 1)
#define HELIOS_UI_TARGET_TARGET_360R  (1 << 2)
#define HELIOS_UI_TARGET_TARGET_240R  (1 << 3)
#define HELIOS_UI_TARGET_TARGET_240   (1 << 4)
#define HELIOS_UI_TARGET_TARGET_390   (1 << 5)
#define HELIOS_UI_TARGET_TARGET_410   (1 << 6)
#define HELIOS_UI_TARGET_ALL          0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef HELIOS_UI_COMPILE_TARGET
#define HELIOS_UI_COMPILE_TARGET HELIOS_UI_TARGET_ALL
#endif

#define HELIOS_UI_CHECK_COMPILE_TARGET(target) (HELIOS_UI_COMPILE_TARGET & (target) ? 1 : 0)

#define COLOR_PRIMARY lv_color_hex(0x0534ff)
#define COLOR_DANGER lv_color_hex(0xf62525)
#define COLOR_BUTTON_PRIMARY lv_color_hex(0x181146)
#define COLOR_BUTTON_DANGER lv_color_hex(0x2c0d0b)
#define UI_VERSION "v1.0.0"


#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    SCREEN_SIZE_SC_466 = 0,
    SCREEN_SIZE_SC_360 = 1,
    SCREEN_SIZE_SC_240 = 2
}screen_size_t;

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_dark;
extern lv_style_t style_primary_bg;
extern lv_style_t style_white_bg;
extern lv_style_t style_text_small_466;
extern lv_style_t style_text_small_360;
extern lv_style_t style_text_small_240;
extern lv_style_t style_text_normal_466;
extern lv_style_t style_text_normal_360;
extern lv_style_t style_text_normal_240;
extern lv_style_t style_text_large_466;
extern lv_style_t style_text_large_360;
extern lv_style_t style_text_large_240;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * NS_Medium_70;
extern lv_font_t * NS_Medium_100;
extern lv_font_t * NS_Medium_140;
extern lv_font_t * NS_Medium_14;
extern lv_font_t * NS_Medium_16;
extern lv_font_t * NS_Medium_18;
extern lv_font_t * NS_Medium_20;
extern lv_font_t * NS_Medium_30;
extern lv_font_t * NS_Medium_40;
extern lv_font_t * NS_Medium_latin_14;
extern lv_font_t * NS_Medium_latin_16;
extern lv_font_t * NS_Medium_latin_18;
extern lv_font_t * NS_Medium_latin_20;
extern lv_font_t * NS_Medium_latin_30;
extern lv_font_t * NS_Medium_latin_40;
extern lv_font_t * NS_Medium_ru_14;
extern lv_font_t * NS_Medium_ru_16;
extern lv_font_t * NS_Medium_ru_18;
extern lv_font_t * NS_Medium_ru_20;
extern lv_font_t * NS_Medium_ru_30;
extern lv_font_t * NS_Medium_ru_40;
extern lv_font_t * NS_Medium_el_14;
extern lv_font_t * NS_Medium_el_16;
extern lv_font_t * NS_Medium_el_18;
extern lv_font_t * NS_Medium_el_20;
extern lv_font_t * NS_Medium_el_30;
extern lv_font_t * NS_Medium_el_40;
extern lv_font_t * NS_Medium_hi_14;
extern lv_font_t * NS_Medium_hi_16;
extern lv_font_t * NS_Medium_hi_18;
extern lv_font_t * NS_Medium_hi_20;
extern lv_font_t * NS_Medium_hi_30;
extern lv_font_t * NS_Medium_hi_40;
extern lv_font_t * NS_Medium_th_14;
extern lv_font_t * NS_Medium_th_16;
extern lv_font_t * NS_Medium_th_18;
extern lv_font_t * NS_Medium_th_20;
extern lv_font_t * NS_Medium_th_30;
extern lv_font_t * NS_Medium_th_40;
extern lv_font_t * NS_Medium_zh_14;
extern lv_font_t * NS_Medium_zh_16;
extern lv_font_t * NS_Medium_zh_18;
extern lv_font_t * NS_Medium_zh_20;
extern lv_font_t * NS_Medium_zh_30;
extern lv_font_t * NS_Medium_zh_40;
extern lv_font_t * NS_Medium_jp_14;
extern lv_font_t * NS_Medium_jp_16;
extern lv_font_t * NS_Medium_jp_18;
extern lv_font_t * NS_Medium_jp_20;
extern lv_font_t * NS_Medium_jp_30;
extern lv_font_t * NS_Medium_jp_40;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * icon_empty;
extern const void * icon_wifi;
extern const void * icon_list;
extern const void * icon_brightness_full;
extern const void * icon_brightness_empty;
extern const void * icon_volume_high;
extern const void * icon_volume_low;
extern const void * icon_mute;
extern const void * icon_play;
extern const void * icon_back;
extern const void * icon_next;
extern const void * icon_connect;
extern const void * icon_vibration;
extern const void * icon_dnd;
extern const void * icon_qr;
extern const void * icon_arrow_up;
extern const void * icon_turn_left;
extern const void * icon_settings_harddisk;
extern const void * icon_settings_brightness;
extern const void * icon_settings_monitor;
extern const void * icon_settings_information;
extern const void * icon_settings_smartwatch;
extern const void * icon_settings_rotate;
extern const void * icon_settings_power;
extern const void * icon_settings_bell;
extern const void * icon_application;
extern const void * icon_chat;
extern const void * icon_weather;
extern const void * icon_gear;
extern const void * icon_measurement;
extern const void * icon_stopwatch;
extern const void * icon_contacts;
extern const void * icon_navigation;
extern const void * icon_timer;
extern const void * icon_music;
extern const void * icon_folder;
extern const void * icon_running;
extern const void * icon_sleep;
extern const void * icon_user;
extern const void * icon_spotify;
extern const void * icon_phone_link;
extern const void * icon_weather_humidity_icon;
extern const void * icon_weather_temp_up_icon;
extern const void * icon_weather_temp_down_icon;
extern const void * icon_weather_uv_icon;
extern const void * icon_nt_messenger;
extern const void * icon_nt_whatsapp;
extern const void * icon_nt_twitter;
extern const void * icon_nt_mail;
extern const void * icon_nt_qq;
extern const void * icon_nt_skype;
extern const void * icon_nt_line;
extern const void * icon_nt_weibo;
extern const void * icon_nt_kakao;
extern const void * icon_nt_viber;
extern const void * icon_nt_vk;
extern const void * icon_nt_wechat;
extern const void * icon_nt_paypal;
extern const void * icon_nt_chat;
extern const void * icon_nt_telegram;
extern const void * icon_nt_instagram;
extern const void * icon_nt_calendar;
extern const void * icon_nt_hangouts;
extern const void * icon_nt_download;
extern const void * icon_nt_facebook;
extern const void * icon_nt_snapchat;
extern const void * icon_nt_tiktok;
extern const void * icon_nt_dingtalk;
extern const void * icon_wt_sun_cloud;
extern const void * icon_wt_sun;
extern const void * icon_wt_rain;
extern const void * icon_wt_cloud;
extern const void * icon_wt_tornado;
extern const void * icon_wt_snow;
extern const void * icon_wt_wind;
extern const void * icon_wt_haze;
extern const void * img_music_album;
extern const void * icon_vol_down_32;
extern const void * icon_vol_up_32;
extern const void * icon_music_play_32;
extern const void * icon_music_pause_32;
extern const void * icon_music_next_32;
extern const void * icon_music_previous_32;
extern const void * icon_stop;
extern const void * icon_timer_cancel;
extern const void * icon_timer_start;
extern const void * icon_timer_pause;
extern const void * icon_lap_time;
extern const void * icon_restart;
extern const void * img_heart_beat;
extern const void * img_blood_oxygen;
extern const void * img_finger_sensor;
extern const void * icon_arrow_red;
extern const void * icon_arrow_green;
extern const void * icon_arrow_red_64;
extern const void * icon_arrow_green_64;
extern const void * icon_text_hide;
extern const void * icon_text_show;
extern const void * icon_mpesa_watch;
extern const void * icon_mpesa_watch_32;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t sb_screen_size;
extern lv_subject_t sb_screen_type;
extern lv_subject_t sb_screen_width;
extern lv_subject_t sb_screen_height;
extern lv_subject_t sb_screen_res;
extern lv_subject_t sb_placeholder;
extern lv_subject_t sb_lvgl_version;
extern lv_subject_t sb_app_list_mode;
extern lv_subject_t sb_list_circular_mode;
extern lv_subject_t sb_screen_brightness;
extern lv_subject_t sb_screen_rotation;
extern lv_subject_t sb_screen_timeout;
extern lv_subject_t sb_screen_rtw;
extern lv_subject_t sb_language;
extern lv_subject_t sb_sound_volume;
extern lv_subject_t sb_focusable;
extern lv_subject_t sb_time_string;
extern lv_subject_t sb_date_string;
extern lv_subject_t sb_time_hour;
extern lv_subject_t sb_time_minute;
extern lv_subject_t sb_time_seconds;
extern lv_subject_t sb_time_hour_analog;
extern lv_subject_t sb_time_minute_analog;
extern lv_subject_t sb_time_seconds_analog;
extern lv_subject_t sb_time_am_pm;
extern lv_subject_t sb_time_am;
extern lv_subject_t sb_time_day;
extern lv_subject_t sb_time_month;
extern lv_subject_t sb_time_year;
extern lv_subject_t sb_time_month_short;
extern lv_subject_t sb_time_month_long;
extern lv_subject_t sb_time_weekday;
extern lv_subject_t sb_time_weekday_short;
extern lv_subject_t sb_time_weekday_long;
extern lv_subject_t sb_activity_steps;
extern lv_subject_t sb_activity_kcal;
extern lv_subject_t sb_activity_distance;
extern lv_subject_t sb_health_bpm;
extern lv_subject_t sb_health_oxygen;
extern lv_subject_t sb_system_connection;
extern lv_subject_t sb_system_connection_str;
extern lv_subject_t sb_os_name;
extern lv_subject_t sb_os_version;
extern lv_subject_t sb_chronos_esp_version;
extern lv_subject_t sb_chronos_app_version;
extern lv_subject_t sb_firmware_version;
extern lv_subject_t sb_board_oem;
extern lv_subject_t sb_board_name;
extern lv_subject_t sb_board_type;
extern lv_subject_t sb_board_mac;
extern lv_subject_t sb_board_ram;
extern lv_subject_t sb_board_psram;
extern lv_subject_t sb_board_flash;
extern lv_subject_t sb_display_type;
extern lv_subject_t sb_battery_percent;
extern lv_subject_t sb_battery_charging;
extern lv_subject_t sb_battery_voltage;
extern lv_subject_t sb_battery_temp;
extern lv_subject_t sb_battery_usage;
extern lv_subject_t sb_nav_icon;
extern lv_subject_t sb_nav_text;
extern lv_subject_t sb_nav_title;
extern lv_subject_t sb_nav_directions;
extern lv_subject_t sb_music_app;
extern lv_subject_t sb_music_track;
extern lv_subject_t sb_music_artist;
extern lv_subject_t sb_music_state;
extern lv_subject_t sb_music_state_icon;
extern lv_subject_t sb_music_icon;
extern lv_subject_t sb_music_package;
extern lv_subject_t sb_music_album_color;
extern lv_subject_t sb_phone_manufacturer;
extern lv_subject_t sb_phone_model;
extern lv_subject_t sb_phone_sdk;
extern lv_subject_t sb_phone_battery;
extern lv_subject_t sb_phone_charging;
extern lv_subject_t sb_phone_charging_str;
extern lv_subject_t sb_chronos_app_code;
extern lv_subject_t sb_phone_last_sync;
extern lv_subject_t sb_weather_code;
extern lv_subject_t sb_weather_icon;
extern lv_subject_t sb_weather_temp;
extern lv_subject_t sb_weather_location;
extern lv_subject_t sb_weather_condition;
extern lv_subject_t sb_weather_update_time;
extern lv_subject_t sb_weather_temp_unit;
extern lv_subject_t sb_weather_humidity;
extern lv_subject_t sb_weather_uv;
extern lv_subject_t sb_weather_temp_high;
extern lv_subject_t sb_weather_temp_low;
extern lv_subject_t sb_stopwatch_state;
extern lv_subject_t sb_stopwatch_time;
extern lv_subject_t sb_stopwatch_button_start_state;
extern lv_subject_t sb_stopwatch_button_reset_state;
extern lv_subject_t sb_stopwatch_button_start_icon;
extern lv_subject_t sb_stopwatch_button_reset_icon;
extern lv_subject_t sb_health_finger_detect;
extern lv_subject_t sb_mpesa_balance_hidden;
extern lv_subject_t sb_mpesa_hidden_icon;
extern lv_subject_t sb_mpesa_balance_text;
extern lv_subject_t sb_mpesa_recent_text;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

void on_music_control_cb(lv_event_t * e);
void on_hs_info_cb(lv_event_t * e);
void on_hs_title_pill_cb(lv_event_t * e);
void on_simulator_event_cb(lv_event_t * e);

/**
 * Initialize the component library
 */

void helios_ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/apps/mpesa_details/mpesa_details_gen.h"
#include "components/apps/mpesa_item/mpesa_item_gen.h"
#include "components/control/control_bar/control_bar_gen.h"
#include "components/control/control_button/control_button_gen.h"
#include "components/control/control_group/control_group_gen.h"
#include "components/control/control_music/control_music_gen.h"
#include "components/control/control_panel/control_panel_gen.h"
#include "components/control/control_slider/control_slider_gen.h"
#include "components/control/control_switch/control_switch_gen.h"
#include "components/dialogs/info_dialog/info_dialog_gen.h"
#include "components/generic/hs_bar/hs_bar_gen.h"
#include "components/generic/hs_button_icon/hs_button_icon_gen.h"
#include "components/generic/hs_button_text/hs_button_text_gen.h"
#include "components/generic/hs_button/hs_button_gen.h"
#include "components/generic/hs_card/hs_card_gen.h"
#include "components/generic/hs_column/hs_column_gen.h"
#include "components/generic/hs_container/hs_container_gen.h"
#include "components/generic/hs_dropdown_item/hs_dropdown_item_gen.h"
#include "components/generic/hs_dropdown/hs_dropdown_gen.h"
#include "components/generic/hs_info_sb_sb/hs_info_sb_sb_gen.h"
#include "components/generic/hs_info_sb/hs_info_sb_gen.h"
#include "components/generic/hs_info/hs_info_gen.h"
#include "components/generic/hs_lap_item/hs_lap_item_gen.h"
#include "components/generic/hs_line/hs_line_gen.h"
#include "components/generic/hs_roller/hs_roller_gen.h"
#include "components/generic/hs_row/hs_row_gen.h"
#include "components/generic/hs_slider/hs_slider_gen.h"
#include "components/generic/hs_space/hs_space_gen.h"
#include "components/generic/hs_switch/hs_switch_gen.h"
#include "components/generic/hs_text_icon/hs_text_icon_normal/hs_text_icon_normal_gen.h"
#include "components/generic/hs_text/hs_tag_normal/hs_tag_normal_gen.h"
#include "components/generic/hs_text/hs_tag_small/hs_tag_small_gen.h"
#include "components/generic/hs_text/hs_text_large/hs_text_large_gen.h"
#include "components/generic/hs_text/hs_text_normal/hs_text_normal_gen.h"
#include "components/generic/hs_text/hs_text_small/hs_text_small_gen.h"
#include "components/generic/hs_title_pill/hs_title_pill_gen.h"
#include "components/generic/hs_title/hs_title_gen.h"
#include "components/items/app_item/app_item_gen.h"
#include "components/items/contact_item/contact_item_gen.h"
#include "components/items/notification_item/notification_item_gen.h"
#include "components/items/panel_dot_item/panel_dot_item_gen.h"
#include "components/music/music_info/music_info_gen.h"
#include "components/music/music_widget/music_widget_gen.h"
#include "components/settings/settings_about/settings_about_gen.h"
#include "components/settings/settings_alert/settings_alert_gen.h"
#include "components/settings/settings_battery/settings_battery_gen.h"
#include "components/settings/settings_display/settings_display_gen.h"
#include "components/settings/settings_storage/settings_storage_gen.h"
#include "components/settings/settings_system/settings_system_gen.h"
#include "components/simple/simple_bar/simple_bar_gen.h"
#include "components/timers/timer_countdown/timer_countdown_gen.h"
#include "components/timers/timer_list/timer_list_gen.h"
#include "components/watchfaces/face_preview/face_preview_gen.h"
#include "components/watchfaces/watchface_default/watchface_default_gen.h"
#include "components/weather/daily_forecast/daily_forecast_gen.h"
#include "components/weather/hourly_forecast/hourly_forecast_gen.h"
#include "components/weather/weather_widget/weather_widget_gen.h"
#include "screens/apps/sc_mpesa/sc_mpesa_gen.h"
#include "screens/core/applications/applications_gen.h"
#include "screens/core/boot/boot_gen.h"
#include "screens/core/contacts/contacts_gen.h"
#include "screens/core/home/home_gen.h"
#include "screens/core/music/music_gen.h"
#include "screens/core/navigation/navigation_gen.h"
#include "screens/core/notifications/notifications_gen.h"
#include "screens/core/phone/phone_gen.h"
#include "screens/core/sc_health/sc_health_gen.h"
#include "screens/core/sc_timer/sc_timer_gen.h"
#include "screens/core/selector/selector_gen.h"
#include "screens/core/settings/settings_gen.h"
#include "screens/core/stopwatch/stopwatch_gen.h"
#include "screens/core/weather/weather_gen.h"
#include "screens/core/widgets/widgets_gen.h"
#include "screens/simulator/simulator_gen.h"
#include "widgets/wd_arcoiris/wd_arcoiris_gen.h"
#include "widgets/wd_dropdown/wd_dropdown_gen.h"
#include "widgets/wd_image/wd_image_gen.h"
#include "widgets/wd_label/wd_label_gen.h"
#include "widgets/wd_list/wd_list_gen.h"
#include "widgets/wd_obj/wd_obj_gen.h"
#include "widgets/wd_panel/wd_panel_gen.h"
#include "widgets/wd_segment/wd_segment_gen.h"
#include "widgets/wf_analog/wf_analog_gen.h"
#include "widgets/wf_item/wf_item_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_HELIOS_UI_GEN_H*/