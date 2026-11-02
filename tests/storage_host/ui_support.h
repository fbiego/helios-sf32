#ifndef STORAGE_UI_TEST_SUPPORT_H
#define STORAGE_UI_TEST_SUPPORT_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#define HELIOS_UI_H
#define LVGL_PRO_HELIOS_UI_GEN_H
#define LV_LVGL_H_INCLUDE_SIMPLE
#define lv_strncpy strncpy
void lv_lock(void);
void lv_unlock(void);
typedef struct { int value; char text[256]; } lv_subject_t;
int lv_subject_get_int(const lv_subject_t *);
const char *lv_subject_get_string(const lv_subject_t *);
extern const void *icon_chat;
extern const void *icon_gear;
extern const void *icon_mpesa_watch_32;
extern const void *icon_music;
extern const void *icon_nt_calendar;
extern const void *icon_nt_chat;
extern const void *icon_nt_dingtalk;
extern const void *icon_nt_download;
extern const void *icon_nt_facebook;
extern const void *icon_nt_hangouts;
extern const void *icon_nt_instagram;
extern const void *icon_nt_kakao;
extern const void *icon_nt_line;
extern const void *icon_nt_mail;
extern const void *icon_nt_messenger;
extern const void *icon_nt_paypal;
extern const void *icon_nt_qq;
extern const void *icon_nt_skype;
extern const void *icon_nt_snapchat;
extern const void *icon_nt_telegram;
extern const void *icon_nt_tiktok;
extern const void *icon_nt_twitter;
extern const void *icon_nt_viber;
extern const void *icon_nt_vk;
extern const void *icon_nt_wechat;
extern const void *icon_nt_weibo;
extern const void *icon_nt_whatsapp;
extern const void *icon_weather;
extern const void *icon_wt_cloud;
extern const void *icon_wt_haze;
extern const void *icon_wt_rain;
extern const void *icon_wt_snow;
extern const void *icon_wt_sun;
extern const void *icon_wt_sun_cloud;
extern const void *icon_wt_tornado;
extern const void *icon_wt_wind;
extern lv_subject_t sb_weather_code;
void helios_subject_set_weather_code(int32_t);
extern lv_subject_t sb_weather_temp;
void helios_subject_set_weather_temp(int32_t);
extern lv_subject_t sb_weather_temp_high;
void helios_subject_set_weather_temp_high(int32_t);
extern lv_subject_t sb_weather_temp_low;
void helios_subject_set_weather_temp_low(int32_t);
extern lv_subject_t sb_weather_humidity;
void helios_subject_set_weather_humidity(int32_t);
extern lv_subject_t sb_weather_uv;
void helios_subject_set_weather_uv(int32_t);
extern lv_subject_t sb_weather_location;
void helios_subject_set_weather_location(const char *);
extern lv_subject_t sb_weather_update_time;
void helios_subject_set_weather_update_time(const char *);
extern lv_subject_t sb_weather_temp_unit;
void helios_subject_set_weather_temp_unit(const char *);
extern lv_subject_t sb_weather_condition;
void helios_subject_set_weather_condition(const char *);
void helios_subject_set_weather_icon(void *);
#endif
