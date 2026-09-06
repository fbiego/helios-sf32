
/**
 * @file subjects.h
 * generated from globals.xml. DO NOT EDIT MANUALLY.
 */

#ifndef SUBJECTS_H
#define SUBJECTS_H


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

                 
/**********************
 * INIT
 **********************/

/**
 * Re-initialize subjects and attach observers.
 */
void helios_subjects_init(void);


/**********************
 * SET FUNCTIONS
 **********************/

/**
 * Grid mode set function.
 * @param value    New value
 */
void helios_subject_set_app_list_mode(int32_t value);

/**
 * Circular mode set function.
 * @param value    New value
 */
void helios_subject_set_list_circular_mode(int32_t value);

/**
 * Screen brightness set function.
 * @param value    New value
 */
void helios_subject_set_screen_brightness(int32_t value);

/**
 * Screen rotation set function.
 * @param value    New value
 */
void helios_subject_set_screen_rotation(int32_t value);

/**
 * Screen timeout set function.
 * @param value    New value
 */
void helios_subject_set_screen_timeout(int32_t value);

/**
 * Raise to wake set function.
 * @param value    New value
 */
void helios_subject_set_screen_rtw(int32_t value);

/**
 * System Language set function.
 * @param value    New value
 */
void helios_subject_set_language(int32_t value);

/**
 * Music volume set function.
 * @param value    New value
 */
void helios_subject_set_sound_volume(int32_t value);

/**
 * Collapsed notifications set function.
 * @param value    New value
 */
void helios_subject_set_focusable(int32_t value);

/**
 * Time string set function.
 * @param value    New value
 */
void helios_subject_set_time_string(const char * value);

/**
 * Date string set function.
 * @param value    New value
 */
void helios_subject_set_date_string(const char * value);

/**
 * Hour set function.
 * @param value    New value
 */
void helios_subject_set_time_hour(int32_t value);

/**
 * Minute set function.
 * @param value    New value
 */
void helios_subject_set_time_minute(int32_t value);

/**
 * Seconds set function.
 * @param value    New value
 */
void helios_subject_set_time_seconds(int32_t value);

/**
 * Hour rotation set function.
 * @param value    New value
 */
void helios_subject_set_time_hour_analog(int32_t value);

/**
 * Minute rotation set function.
 * @param value    New value
 */
void helios_subject_set_time_minute_analog(int32_t value);

/**
 * Seconds rotation set function.
 * @param value    New value
 */
void helios_subject_set_time_seconds_analog(int32_t value);

/**
 * am/pm set function.
 * @param value    New value
 */
void helios_subject_set_time_am_pm(const char * value);

/**
 * -1 24hr, 0 am, 1 pm set function.
 * @param value    New value
 */
void helios_subject_set_time_am(int32_t value);

/**
 * Day set function.
 * @param value    New value
 */
void helios_subject_set_time_day(int32_t value);

/**
 * Month set function.
 * @param value    New value
 */
void helios_subject_set_time_month(int32_t value);

/**
 * Year set function.
 * @param value    New value
 */
void helios_subject_set_time_year(int32_t value);

/**
 * Weekday index set function.
 * @param value    New value
 */
void helios_subject_set_time_weekday(int32_t value);

/**
 * Step count set function.
 * @param value    New value
 */
void helios_subject_set_activity_steps(int32_t value);

/**
 *  set function.
 * @param value    New value
 */
void helios_subject_set_activity_kcal(int32_t value);

/**
 *  set function.
 * @param value    New value
 */
void helios_subject_set_activity_distance(int32_t value);

/**
 * BPM set function.
 * @param value    New value
 */
void helios_subject_set_health_bpm(int32_t value);

/**
 * SP02 set function.
 * @param value    New value
 */
void helios_subject_set_health_oxygen(int32_t value);

/**
 * System ble connection set function.
 * @param value    New value
 */
void helios_subject_set_system_connection(int32_t value);

/**
 * System ble connection string connected/disconnected set function.
 * @param value    New value
 */
void helios_subject_set_system_connection_str(const char * value);

/**
 * OS name set function.
 * @param value    New value
 */
void helios_subject_set_os_name(const char * value);

/**
 * OS version set function.
 * @param value    New value
 */
void helios_subject_set_os_version(const char * value);

/**
 * Chronos library version set function.
 * @param value    New value
 */
void helios_subject_set_chronos_esp_version(const char * value);

/**
 * Chronos app version set function.
 * @param value    New value
 */
void helios_subject_set_chronos_app_version(const char * value);

/**
 * Helios Firmware version set function.
 * @param value    New value
 */
void helios_subject_set_firmware_version(const char * value);

/**
 * Helios board OEM set function.
 * @param value    New value
 */
void helios_subject_set_board_oem(const char * value);

/**
 * Helios board name set function.
 * @param value    New value
 */
void helios_subject_set_board_name(const char * value);

/**
 * Helios board type set function.
 * @param value    New value
 */
void helios_subject_set_board_type(const char * value);

/**
 * Helios board MAC address set function.
 * @param value    New value
 */
void helios_subject_set_board_mac(const char * value);

/**
 * Helios board RAM set function.
 * @param value    New value
 */
void helios_subject_set_board_ram(const char * value);

/**
 * Helios board PSRAM set function.
 * @param value    New value
 */
void helios_subject_set_board_psram(const char * value);

/**
 * Helios board Flash set function.
 * @param value    New value
 */
void helios_subject_set_board_flash(const char * value);

/**
 * Helios display type set function.
 * @param value    New value
 */
void helios_subject_set_display_type(const char * value);

/**
 * Battery percentage set function.
 * @param value    New value
 */
void helios_subject_set_battery_percent(int32_t value);

/**
 * Battery Charging set function.
 * @param value    New value
 */
void helios_subject_set_battery_charging(int32_t value);

/**
 * Battery Voltage set function.
 * @param value    New value
 */
void helios_subject_set_battery_voltage(float value);

/**
 * Battery Temperature set function.
 * @param value    New value
 */
void helios_subject_set_battery_temp(float value);

/**
 * Battery Usage set function.
 * @param value    New value
 */
void helios_subject_set_battery_usage(const char * value);

/**
 * Current navigation icon set function.
 * @param value    New value
 */
void helios_subject_set_nav_icon(void * value);

/**
 * Navigation text (eta\nduration distance) set function.
 * @param value    New value
 */
void helios_subject_set_nav_text(const char * value);

/**
 * Navigation title set function.
 * @param value    New value
 */
void helios_subject_set_nav_title(const char * value);

/**
 * Navigation directions set function.
 * @param value    New value
 */
void helios_subject_set_nav_directions(const char * value);

/**
 * Music app set function.
 * @param value    New value
 */
void helios_subject_set_music_app(const char * value);

/**
 * Music track set function.
 * @param value    New value
 */
void helios_subject_set_music_track(const char * value);

/**
 * Music artist set function.
 * @param value    New value
 */
void helios_subject_set_music_artist(const char * value);

/**
 * Music state set function.
 * @param value    New value
 */
void helios_subject_set_music_state(int32_t value);

/**
 * Music play/pause icon set function.
 * @param value    New value
 */
void helios_subject_set_music_state_icon(void * value);

/**
 * Music icon set function.
 * @param value    New value
 */
void helios_subject_set_music_icon(void * value);

/**
 * Music app package set function.
 * @param value    New value
 */
void helios_subject_set_music_package(const char * value);

/**
 * Music album color set function.
 * @param value    New value
 */
void helios_subject_set_music_album_color(lv_color_t value);

/**
 * Phone manufacturer set function.
 * @param value    New value
 */
void helios_subject_set_phone_manufacturer(const char * value);

/**
 * Phone model set function.
 * @param value    New value
 */
void helios_subject_set_phone_model(const char * value);

/**
 * Phone SDK version set function.
 * @param value    New value
 */
void helios_subject_set_phone_sdk(int32_t value);

/**
 * Phone battery set function.
 * @param value    New value
 */
void helios_subject_set_phone_battery(int32_t value);

/**
 * Phone charging set function.
 * @param value    New value
 */
void helios_subject_set_phone_charging(int32_t value);

/**
 * Phone charging string yes/no set function.
 * @param value    New value
 */
void helios_subject_set_phone_charging_str(const char * value);

/**
 * Chronos App code set function.
 * @param value    New value
 */
void helios_subject_set_chronos_app_code(int32_t value);

/**
 * Phone last sync set function.
 * @param value    New value
 */
void helios_subject_set_phone_last_sync(const char * value);

/**
 * Weather condition code set function.
 * @param value    New value
 */
void helios_subject_set_weather_code(int32_t value);

/**
 * Weather icon set function.
 * @param value    New value
 */
void helios_subject_set_weather_icon(void * value);

/**
 * Weather temperature set function.
 * @param value    New value
 */
void helios_subject_set_weather_temp(int32_t value);

/**
 * Weather location set function.
 * @param value    New value
 */
void helios_subject_set_weather_location(const char * value);

/**
 * Weather condition set function.
 * @param value    New value
 */
void helios_subject_set_weather_condition(const char * value);

/**
 * Weather update time set function.
 * @param value    New value
 */
void helios_subject_set_weather_update_time(const char * value);

/**
 * Weather update date set function.
 * @param value    New value
 */
void helios_subject_set_weather_temp_unit(const char * value);

/**
 * Weather humidity set function.
 * @param value    New value
 */
void helios_subject_set_weather_humidity(int32_t value);

/**
 * Weather UV index set function.
 * @param value    New value
 */
void helios_subject_set_weather_uv(int32_t value);

/**
 * Weather high temperature set function.
 * @param value    New value
 */
void helios_subject_set_weather_temp_high(int32_t value);

/**
 * Weather low temperature set function.
 * @param value    New value
 */
void helios_subject_set_weather_temp_low(int32_t value);

/**
 * Stopwatch state set function.
 * @param value    New value
 */
void helios_subject_set_stopwatch_state(int32_t value);

/**
 * Stopwatch time set function.
 * @param value    New value
 */
void helios_subject_set_stopwatch_time(const char * value);

/**
 * Stopwatch pause button state set function.
 * @param value    New value
 */
void helios_subject_set_stopwatch_button_start_state(int32_t value);

/**
 * Stopwatch reset button state set function.
 * @param value    New value
 */
void helios_subject_set_stopwatch_button_reset_state(int32_t value);

/**
 * Stopwatch start button icon set function.
 * @param value    New value
 */
void helios_subject_set_stopwatch_button_start_icon(void * value);

/**
 * Stopwatch reset button icon set function.
 * @param value    New value
 */
void helios_subject_set_stopwatch_button_reset_icon(void * value);

/**
 * Finger detect on sensor set function.
 * @param value    New value
 */
void helios_subject_set_health_finger_detect(int32_t value);

/**
 * Mpesa Balance hidden set function.
 * @param value    New value
 */
void helios_subject_set_mpesa_balance_hidden(int32_t value);

/**
 * hide icon set function.
 * @param value    New value
 */
void helios_subject_set_mpesa_hidden_icon(void * value);

/**
 * Mpesa balance text set function.
 * @param value    New value
 */
void helios_subject_set_mpesa_balance_text(const char * value);

/**
 * Mpesa recent text set function.
 * @param value    New value
 */
void helios_subject_set_mpesa_recent_text(const char * value);


/**********************
 * GET FUNCTIONS
 **********************/


/**********************
 * CHANGE FUNCTIONS
 **********************/

/**
 * Grid mode change callback.
 * @param value    Updated value
 */
void helios_subject_app_list_mode_change(int32_t value);

/**
 * Circular mode change callback.
 * @param value    Updated value
 */
void helios_subject_list_circular_mode_change(int32_t value);

/**
 * Screen brightness change callback.
 * @param value    Updated value
 */
void helios_subject_screen_brightness_change(int32_t value);

/**
 * Screen rotation change callback.
 * @param value    Updated value
 */
void helios_subject_screen_rotation_change(int32_t value);

/**
 * Screen timeout change callback.
 * @param value    Updated value
 */
void helios_subject_screen_timeout_change(int32_t value);

/**
 * Raise to wake change callback.
 * @param value    Updated value
 */
void helios_subject_screen_rtw_change(int32_t value);

/**
 * System Language change callback.
 * @param value    Updated value
 */
void helios_subject_language_change(int32_t value);

/**
 * Music volume change callback.
 * @param value    Updated value
 */
void helios_subject_sound_volume_change(int32_t value);

/**
 * Collapsed notifications change callback.
 * @param value    Updated value
 */
void helios_subject_focusable_change(int32_t value);

/**
 * Month change callback.
 * @param value    Updated value
 */
void helios_subject_time_month_change(int32_t value);

/**
 * Weekday index change callback.
 * @param value    Updated value
 */
void helios_subject_time_weekday_change(int32_t value);

/**
 * System ble connection change callback.
 * @param value    Updated value
 */
void helios_subject_system_connection_change(int32_t value);

/**
 * Music state change callback.
 * @param value    Updated value
 */
void helios_subject_music_state_change(int32_t value);

/**
 * Music app package change callback.
 * @param value    Updated value
 */
void helios_subject_music_package_change(const char * value);

/**
 * Phone charging change callback.
 * @param value    Updated value
 */
void helios_subject_phone_charging_change(int32_t value);

/**
 * Weather condition code change callback.
 * @param value    Updated value
 */
void helios_subject_weather_code_change(int32_t value);

/**
 * Stopwatch state change callback.
 * @param value    Updated value
 */
void helios_subject_stopwatch_state_change(int32_t value);

/**
 * Stopwatch pause button state change callback.
 * @param value    Updated value
 */
void helios_subject_stopwatch_button_start_state_change(int32_t value);

/**
 * Stopwatch reset button state change callback.
 * @param value    Updated value
 */
void helios_subject_stopwatch_button_reset_state_change(int32_t value);

/**
 * Mpesa Balance hidden change callback.
 * @param value    Updated value
 */
void helios_subject_mpesa_balance_hidden_change(int32_t value);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SUBJECTS_H*/
