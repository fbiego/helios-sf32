
/**
 * @file subjects.c
 * generated from globals.xml. DO NOT EDIT MANUALLY.
 */

/*********************
 *      INCLUDES
 *********************/
                 
#include "subjects.h"
#include "../../helios_ui.h" 

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void int_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void string_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  INIT 
 **********************/

void helios_subjects_init(void)
{
    /* Re-initialize non-supported subjects in the Editor */
    lv_subject_deinit(&sb_music_album_color);
    lv_subject_init_color(&sb_music_album_color, lv_color_hex(0xFFFFFF));

    /* Attach observers for change notifications */
    lv_subject_add_observer(&sb_app_list_mode, int_subject_observer_cb, (void *)helios_subject_app_list_mode_change);
    lv_subject_add_observer(&sb_list_circular_mode, int_subject_observer_cb, (void *)helios_subject_list_circular_mode_change);
    lv_subject_add_observer(&sb_screen_brightness, int_subject_observer_cb, (void *)helios_subject_screen_brightness_change);
    lv_subject_add_observer(&sb_screen_rotation, int_subject_observer_cb, (void *)helios_subject_screen_rotation_change);
    lv_subject_add_observer(&sb_screen_timeout, int_subject_observer_cb, (void *)helios_subject_screen_timeout_change);
    lv_subject_add_observer(&sb_screen_rtw, int_subject_observer_cb, (void *)helios_subject_screen_rtw_change);
    lv_subject_add_observer(&sb_language, int_subject_observer_cb, (void *)helios_subject_language_change);
    lv_subject_add_observer(&sb_sound_volume, int_subject_observer_cb, (void *)helios_subject_sound_volume_change);
    lv_subject_add_observer(&sb_focusable, int_subject_observer_cb, (void *)helios_subject_focusable_change);
    lv_subject_add_observer(&sb_time_month, int_subject_observer_cb, (void *)helios_subject_time_month_change);
    lv_subject_add_observer(&sb_time_weekday, int_subject_observer_cb, (void *)helios_subject_time_weekday_change);
    lv_subject_add_observer(&sb_system_connection, int_subject_observer_cb, (void *)helios_subject_system_connection_change);
    lv_subject_add_observer(&sb_music_state, int_subject_observer_cb, (void *)helios_subject_music_state_change);
    lv_subject_add_observer(&sb_music_package, string_subject_observer_cb, (void *)helios_subject_music_package_change);
    lv_subject_add_observer(&sb_phone_charging, int_subject_observer_cb, (void *)helios_subject_phone_charging_change);
    lv_subject_add_observer(&sb_weather_code, int_subject_observer_cb, (void *)helios_subject_weather_code_change);
    lv_subject_add_observer(&sb_stopwatch_state, int_subject_observer_cb, (void *)helios_subject_stopwatch_state_change);
    lv_subject_add_observer(&sb_stopwatch_button_start_state, int_subject_observer_cb, (void *)helios_subject_stopwatch_button_start_state_change);
    lv_subject_add_observer(&sb_stopwatch_button_reset_state, int_subject_observer_cb, (void *)helios_subject_stopwatch_button_reset_state_change);
    lv_subject_add_observer(&sb_mpesa_balance_hidden, int_subject_observer_cb, (void *)helios_subject_mpesa_balance_hidden_change);

}

/**********************
 * SET FUNCTIONS
 **********************/

void helios_subject_set_app_list_mode(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_app_list_mode, value);
    lv_unlock();
}

void helios_subject_set_list_circular_mode(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_list_circular_mode, value);
    lv_unlock();
}

void helios_subject_set_screen_brightness(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_screen_brightness, value);
    lv_unlock();
}

void helios_subject_set_screen_rotation(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_screen_rotation, value);
    lv_unlock();
}

void helios_subject_set_screen_timeout(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_screen_timeout, value);
    lv_unlock();
}

void helios_subject_set_screen_rtw(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_screen_rtw, value);
    lv_unlock();
}

void helios_subject_set_language(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_language, value);
    lv_unlock();
}

void helios_subject_set_sound_volume(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_sound_volume, value);
    lv_unlock();
}

void helios_subject_set_focusable(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_focusable, value);
    lv_unlock();
}

void helios_subject_set_time_string(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_time_string, value);
    lv_unlock();
}

void helios_subject_set_date_string(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_date_string, value);
    lv_unlock();
}

void helios_subject_set_time_hour(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_hour, value);
    lv_unlock();
}

void helios_subject_set_time_minute(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_minute, value);
    lv_unlock();
}

void helios_subject_set_time_seconds(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_seconds, value);
    lv_unlock();
}

void helios_subject_set_time_hour_analog(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_hour_analog, value);
    lv_unlock();
}

void helios_subject_set_time_minute_analog(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_minute_analog, value);
    lv_unlock();
}

void helios_subject_set_time_seconds_analog(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_seconds_analog, value);
    lv_unlock();
}

void helios_subject_set_time_am_pm(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_time_am_pm, value);
    lv_unlock();
}

void helios_subject_set_time_am(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_am, value);
    lv_unlock();
}

void helios_subject_set_time_day(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_day, value);
    lv_unlock();
}

void helios_subject_set_time_month(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_month, value);
    lv_unlock();
}

void helios_subject_set_time_year(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_year, value);
    lv_unlock();
}

void helios_subject_set_time_weekday(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_time_weekday, value);
    lv_unlock();
}

void helios_subject_set_activity_steps(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_activity_steps, value);
    lv_unlock();
}

void helios_subject_set_activity_kcal(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_activity_kcal, value);
    lv_unlock();
}

void helios_subject_set_activity_distance(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_activity_distance, value);
    lv_unlock();
}

void helios_subject_set_health_bpm(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_health_bpm, value);
    lv_unlock();
}

void helios_subject_set_health_oxygen(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_health_oxygen, value);
    lv_unlock();
}

void helios_subject_set_system_connection(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_system_connection, value);
    lv_unlock();
}

void helios_subject_set_system_connection_str(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_system_connection_str, value);
    lv_unlock();
}

void helios_subject_set_chronos_esp_version(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_chronos_esp_version, value);
    lv_unlock();
}

void helios_subject_set_chronos_app_version(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_chronos_app_version, value);
    lv_unlock();
}

void helios_subject_set_firmware_version(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_firmware_version, value);
    lv_unlock();
}

void helios_subject_set_board_oem(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_oem, value);
    lv_unlock();
}

void helios_subject_set_board_name(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_name, value);
    lv_unlock();
}

void helios_subject_set_board_type(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_type, value);
    lv_unlock();
}

void helios_subject_set_board_mac(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_mac, value);
    lv_unlock();
}

void helios_subject_set_board_ram(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_ram, value);
    lv_unlock();
}

void helios_subject_set_board_psram(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_psram, value);
    lv_unlock();
}

void helios_subject_set_board_flash(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_board_flash, value);
    lv_unlock();
}

void helios_subject_set_display_type(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_display_type, value);
    lv_unlock();
}

void helios_subject_set_battery_percent(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_battery_percent, value);
    lv_unlock();
}

void helios_subject_set_battery_charging(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_battery_charging, value);
    lv_unlock();
}

void helios_subject_set_battery_voltage(float value)
{
    lv_lock();
    lv_subject_set_float(&sb_battery_voltage, value);
    lv_unlock();
}

void helios_subject_set_battery_temp(float value)
{
    lv_lock();
    lv_subject_set_float(&sb_battery_temp, value);
    lv_unlock();
}

void helios_subject_set_battery_usage(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_battery_usage, value);
    lv_unlock();
}

void helios_subject_set_nav_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_nav_icon, value);
    lv_unlock();
}

void helios_subject_set_nav_text(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_nav_text, value);
    lv_unlock();
}

void helios_subject_set_nav_title(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_nav_title, value);
    lv_unlock();
}

void helios_subject_set_nav_directions(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_nav_directions, value);
    lv_unlock();
}

void helios_subject_set_music_app(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_music_app, value);
    lv_unlock();
}

void helios_subject_set_music_track(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_music_track, value);
    lv_unlock();
}

void helios_subject_set_music_artist(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_music_artist, value);
    lv_unlock();
}

void helios_subject_set_music_state(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_music_state, value);
    lv_unlock();
}

void helios_subject_set_music_state_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_music_state_icon, value);
    lv_unlock();
}

void helios_subject_set_music_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_music_icon, value);
    lv_unlock();
}

void helios_subject_set_music_package(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_music_package, value);
    lv_unlock();
}

void helios_subject_set_music_album_color(lv_color_t value)
{
    lv_lock();
    lv_subject_set_color(&sb_music_album_color, value);
    lv_unlock();
}

void helios_subject_set_phone_manufacturer(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_phone_manufacturer, value);
    lv_unlock();
}

void helios_subject_set_phone_model(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_phone_model, value);
    lv_unlock();
}

void helios_subject_set_phone_sdk(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_phone_sdk, value);
    lv_unlock();
}

void helios_subject_set_phone_battery(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_phone_battery, value);
    lv_unlock();
}

void helios_subject_set_phone_charging(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_phone_charging, value);
    lv_unlock();
}

void helios_subject_set_phone_charging_str(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_phone_charging_str, value);
    lv_unlock();
}

void helios_subject_set_chronos_app_code(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_chronos_app_code, value);
    lv_unlock();
}

void helios_subject_set_phone_last_sync(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_phone_last_sync, value);
    lv_unlock();
}

void helios_subject_set_weather_code(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_weather_code, value);
    lv_unlock();
}

void helios_subject_set_weather_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_weather_icon, value);
    lv_unlock();
}

void helios_subject_set_weather_temp(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_weather_temp, value);
    lv_unlock();
}

void helios_subject_set_weather_location(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_weather_location, value);
    lv_unlock();
}

void helios_subject_set_weather_condition(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_weather_condition, value);
    lv_unlock();
}

void helios_subject_set_weather_update_time(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_weather_update_time, value);
    lv_unlock();
}

void helios_subject_set_weather_temp_unit(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_weather_temp_unit, value);
    lv_unlock();
}

void helios_subject_set_weather_humidity(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_weather_humidity, value);
    lv_unlock();
}

void helios_subject_set_weather_uv(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_weather_uv, value);
    lv_unlock();
}

void helios_subject_set_weather_temp_high(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_weather_temp_high, value);
    lv_unlock();
}

void helios_subject_set_weather_temp_low(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_weather_temp_low, value);
    lv_unlock();
}

void helios_subject_set_stopwatch_state(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_stopwatch_state, value);
    lv_unlock();
}

void helios_subject_set_stopwatch_time(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_stopwatch_time, value);
    lv_unlock();
}

void helios_subject_set_stopwatch_button_start_state(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_stopwatch_button_start_state, value);
    lv_unlock();
}

void helios_subject_set_stopwatch_button_reset_state(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_stopwatch_button_reset_state, value);
    lv_unlock();
}

void helios_subject_set_stopwatch_button_start_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_stopwatch_button_start_icon, value);
    lv_unlock();
}

void helios_subject_set_stopwatch_button_reset_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_stopwatch_button_reset_icon, value);
    lv_unlock();
}

void helios_subject_set_health_finger_detect(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_health_finger_detect, value);
    lv_unlock();
}

void helios_subject_set_mpesa_balance_hidden(int32_t value)
{
    lv_lock();
    lv_subject_set_int(&sb_mpesa_balance_hidden, value);
    lv_unlock();
}

void helios_subject_set_mpesa_hidden_icon(void * value)
{
    lv_lock();
    lv_subject_set_pointer(&sb_mpesa_hidden_icon, value);
    lv_unlock();
}

void helios_subject_set_mpesa_balance_text(const char * value)
{
    lv_lock();
    lv_subject_copy_string(&sb_mpesa_balance_text, value);
    lv_unlock();
}

/**********************
 * GET FUNCTIONS
 **********************/

/**********************
 * WEAK FUNCTIONS
 **********************/

void __attribute__((weak)) helios_subject_app_list_mode_change(int32_t value)
{
    LV_LOG_USER("Subject app_list_mode changed %d", value);
}

void __attribute__((weak)) helios_subject_list_circular_mode_change(int32_t value)
{
    LV_LOG_USER("Subject list_circular_mode changed %d", value);
}

void __attribute__((weak)) helios_subject_screen_brightness_change(int32_t value)
{
    LV_LOG_USER("Subject screen_brightness changed %d", value);
}

void __attribute__((weak)) helios_subject_screen_rotation_change(int32_t value)
{
    LV_LOG_USER("Subject screen_rotation changed %d", value);
}

void __attribute__((weak)) helios_subject_screen_timeout_change(int32_t value)
{
    LV_LOG_USER("Subject screen_timeout changed %d", value);
}

void __attribute__((weak)) helios_subject_screen_rtw_change(int32_t value)
{
    LV_LOG_USER("Subject screen_rtw changed %d", value);
}

void __attribute__((weak)) helios_subject_language_change(int32_t value)
{
    LV_LOG_USER("Subject language changed %d", value);
}

void __attribute__((weak)) helios_subject_sound_volume_change(int32_t value)
{
    LV_LOG_USER("Subject sound_volume changed %d", value);
}

void __attribute__((weak)) helios_subject_focusable_change(int32_t value)
{
    LV_LOG_USER("Subject focusable changed %d", value);
}

void __attribute__((weak)) helios_subject_time_month_change(int32_t value)
{
    LV_LOG_USER("Subject time_month changed %d", value);
}

void __attribute__((weak)) helios_subject_time_weekday_change(int32_t value)
{
    LV_LOG_USER("Subject time_weekday changed %d", value);
}

void __attribute__((weak)) helios_subject_system_connection_change(int32_t value)
{
    LV_LOG_USER("Subject system_connection changed %d", value);
}

void __attribute__((weak)) helios_subject_music_state_change(int32_t value)
{
    LV_LOG_USER("Subject music_state changed %d", value);
}

void __attribute__((weak)) helios_subject_music_package_change(const char * value)
{
    LV_LOG_USER("Subject music_package changed %s", value);
}

void __attribute__((weak)) helios_subject_phone_charging_change(int32_t value)
{
    LV_LOG_USER("Subject phone_charging changed %d", value);
}

void __attribute__((weak)) helios_subject_weather_code_change(int32_t value)
{
    LV_LOG_USER("Subject weather_code changed %d", value);
}

void __attribute__((weak)) helios_subject_stopwatch_state_change(int32_t value)
{
    LV_LOG_USER("Subject stopwatch_state changed %d", value);
}

void __attribute__((weak)) helios_subject_stopwatch_button_start_state_change(int32_t value)
{
    LV_LOG_USER("Subject stopwatch_button_start_state changed %d", value);
}

void __attribute__((weak)) helios_subject_stopwatch_button_reset_state_change(int32_t value)
{
    LV_LOG_USER("Subject stopwatch_button_reset_state changed %d", value);
}

void __attribute__((weak)) helios_subject_mpesa_balance_hidden_change(int32_t value)
{
    LV_LOG_USER("Subject mpesa_balance_hidden changed %d", value);
}

/**********************
 * STATIC FUNCTIONS
 **********************/

static void int_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    int32_t value = lv_subject_get_int(subject);
    void (*fn)(int32_t) = lv_observer_get_user_data(observer);
    if(fn) fn(value);
}

static void string_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const char * value = lv_subject_get_string(subject);
    void (*fn)(const char *) = lv_observer_get_user_data(observer);
    if(fn) fn(value);
}
