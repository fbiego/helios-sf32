#include "ui_support.h"
#include <stdio.h>
const void *icon_chat = (void *)(uintptr_t)1;
const void *icon_gear = (void *)(uintptr_t)2;
const void *icon_mpesa_watch_32 = (void *)(uintptr_t)3;
const void *icon_music = (void *)(uintptr_t)4;
const void *icon_nt_calendar = (void *)(uintptr_t)5;
const void *icon_nt_chat = (void *)(uintptr_t)6;
const void *icon_nt_dingtalk = (void *)(uintptr_t)7;
const void *icon_nt_download = (void *)(uintptr_t)8;
const void *icon_nt_facebook = (void *)(uintptr_t)9;
const void *icon_nt_hangouts = (void *)(uintptr_t)10;
const void *icon_nt_instagram = (void *)(uintptr_t)11;
const void *icon_nt_kakao = (void *)(uintptr_t)12;
const void *icon_nt_line = (void *)(uintptr_t)13;
const void *icon_nt_mail = (void *)(uintptr_t)14;
const void *icon_nt_messenger = (void *)(uintptr_t)15;
const void *icon_nt_paypal = (void *)(uintptr_t)16;
const void *icon_nt_qq = (void *)(uintptr_t)17;
const void *icon_nt_skype = (void *)(uintptr_t)18;
const void *icon_nt_snapchat = (void *)(uintptr_t)19;
const void *icon_nt_telegram = (void *)(uintptr_t)20;
const void *icon_nt_tiktok = (void *)(uintptr_t)21;
const void *icon_nt_twitter = (void *)(uintptr_t)22;
const void *icon_nt_viber = (void *)(uintptr_t)23;
const void *icon_nt_vk = (void *)(uintptr_t)24;
const void *icon_nt_wechat = (void *)(uintptr_t)25;
const void *icon_nt_weibo = (void *)(uintptr_t)26;
const void *icon_nt_whatsapp = (void *)(uintptr_t)27;
const void *icon_weather = (void *)(uintptr_t)28;
const void *icon_wt_cloud = (void *)(uintptr_t)29;
const void *icon_wt_haze = (void *)(uintptr_t)30;
const void *icon_wt_rain = (void *)(uintptr_t)31;
const void *icon_wt_snow = (void *)(uintptr_t)32;
const void *icon_wt_sun = (void *)(uintptr_t)33;
const void *icon_wt_sun_cloud = (void *)(uintptr_t)34;
const void *icon_wt_tornado = (void *)(uintptr_t)35;
const void *icon_wt_wind = (void *)(uintptr_t)36;
void lv_lock(void) {}
void lv_unlock(void) {}
int lv_subject_get_int(const lv_subject_t *s) { return s->value; }
const char *lv_subject_get_string(const lv_subject_t *s) { return s->text; }
lv_subject_t sb_weather_code;
void helios_subject_set_weather_code(int32_t v) { sb_weather_code.value=v; }
lv_subject_t sb_weather_temp;
void helios_subject_set_weather_temp(int32_t v) { sb_weather_temp.value=v; }
lv_subject_t sb_weather_temp_high;
void helios_subject_set_weather_temp_high(int32_t v) { sb_weather_temp_high.value=v; }
lv_subject_t sb_weather_temp_low;
void helios_subject_set_weather_temp_low(int32_t v) { sb_weather_temp_low.value=v; }
lv_subject_t sb_weather_humidity;
void helios_subject_set_weather_humidity(int32_t v) { sb_weather_humidity.value=v; }
lv_subject_t sb_weather_uv;
void helios_subject_set_weather_uv(int32_t v) { sb_weather_uv.value=v; }
lv_subject_t sb_weather_location;
void helios_subject_set_weather_location(const char *v) { snprintf(sb_weather_location.text,256,"%s",v); }
lv_subject_t sb_weather_update_time;
void helios_subject_set_weather_update_time(const char *v) { snprintf(sb_weather_update_time.text,256,"%s",v); }
lv_subject_t sb_weather_temp_unit;
void helios_subject_set_weather_temp_unit(const char *v) { snprintf(sb_weather_temp_unit.text,256,"%s",v); }
lv_subject_t sb_weather_condition;
void helios_subject_set_weather_condition(const char *v) { snprintf(sb_weather_condition.text,256,"%s",v); }
void helios_subject_set_weather_icon(void *v) { (void)v; }
