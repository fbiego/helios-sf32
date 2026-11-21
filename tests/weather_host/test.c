#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/helios_weather_data.h"
#include "core/helios_weather_ui.h"
#include "helios_ui/helios_ui.h"
#include "helios_ui/custom/apps/weather/weather.h"

static uint32_t clock_now;
static unsigned weather_events;
static void config(chronos_config_t kind, uint32_t a, uint32_t b)
{ (void)a; (void)b; assert(kind == CHRONOS_CONFIG_WEATHER); weather_events++; }
static uint32_t timestamp(void) { return clock_now; }
static time_t date(int year, int month, int day, int hour, int minute)
{
    struct tm calendar = {0};
    calendar.tm_year=year-1900;calendar.tm_mon=month-1;calendar.tm_mday=day;
    calendar.tm_hour=hour;calendar.tm_min=minute;calendar.tm_isdst=-1;
    return mktime(&calendar);
}
static void send(uint8_t prefix, uint8_t command, uint8_t subcommand, const uint8_t *data, size_t size)
{
    uint8_t packet[256];size_t length=0;
    assert(chronos_core_build_packet(prefix,CHRONOS_PROTOCOL_STANDARD,command,subcommand,
        data,size,packet,sizeof(packet),&length)==CHRONOS_STATUS_OK);
    assert(chronos_core_receive_ble_packet(packet,length)==CHRONOS_STATUS_OK);
}
int main(void)
{
    chronos_core_init();chronos_core_set_time_cb(timestamp);chronos_core_set_config_cb(config);helios_weather_init();
    clock_now=date(2026,12,31,20,45);
    const uint8_t days[]={0x10,20,0x40,21,0x30,22};
    send(0xAB,0x7E,0x80,days,sizeof(days));
    const uint8_t uv[]={6,3,240};
    unsigned events=weather_events;send(0xAB,0x8A,0x80,uv,sizeof(uv));
    assert(weather_events==events+1 && chronos_core_get_weather_at(0)->uv==6);
    /* Hours are chunks, not a complete-day replacement. */
    const uint8_t hours[]={4,20,0x10,25,0,5,40,2, 0x40,26,0,5,41,3,
        0x30,27,0,5,42,4, 0x40,28,0,5,43,5};
    send(0xEA,0x7E,0x02,hours,sizeof(hours));
    helios_weather_poll(clock_now);
    assert(lv_subject_get_int(&sb_weather_temp)==25);
    assert(lv_subject_get_int(&sb_weather_humidity)==40);
    assert(helios_weather_hourly_count()==3);
    assert(!strcmp(helios_weather_hourly_get(0)->time,"21:00"));
    assert(helios_weather_daily_count()==2);
    assert(!strcmp(helios_weather_daily_get(0)->day,"Fri"));
    /* Reboot: serialize to text and restore full readings, then advance hour. */
    cJSON *doc=cJSON_CreateObject();assert(helios_weather_data_save(doc));
    char *text=cJSON_PrintUnformatted(doc);cJSON_Delete(doc);doc=cJSON_Parse(text);free(text);
    chronos_core_init();assert(helios_weather_data_restore(doc));
    time_t next=date(2026,12,31,21,0);helios_weather_poll(next);
    assert(lv_subject_get_int(&sb_weather_temp)==26 && lv_subject_get_int(&sb_weather_uv)==3);
    assert(helios_weather_hourly_count()==2 && !strcmp(helios_weather_hourly_get(0)->time,"22:00"));
    assert(chronos_core_get_weather_time()==clock_now);
    helios_weather_poll(date(2026,12,31,23,0));assert(!helios_weather_hourly_count());
    helios_weather_poll(date(2027,1,1,0,0));
    assert(!helios_weather_hourly_count() && lv_subject_get_int(&sb_weather_temp)==21);
    assert(helios_weather_daily_count()==1 && !strcmp(helios_weather_daily_get(0)->day,"Sat"));
    /* A location or partial hourly refresh must not re-date old slots/days. */
    chronos_core_set_time_cb(timestamp);clock_now=date(2027,1,1,20,0);
    const uint8_t city[]={0,'N','a','i','r','o','b','i'};
    send(0xEA,0x7E,0x01,city,sizeof(city));helios_weather_render(clock_now);
    assert(!helios_weather_hourly_count() && helios_weather_daily_count()==1);
    chronos_core_set_time_cb(timestamp);clock_now=date(2027,1,1,20,0);
    const uint8_t partial[]={1,21,0x10,30,0,0,50,1};
    send(0xEA,0x7E,0x02,partial,sizeof(partial));helios_weather_render(clock_now);
    assert(helios_weather_hourly_count()==1 && !strcmp(helios_weather_hourly_get(0)->time,"21:00"));
    assert(!helios_weather_hour_at(clock_now,22));
    helios_weather_render(date(2027,1,4,10,0));
    assert(!helios_weather_daily_count() && !helios_weather_hourly_count());
    /* Invalid raw readings fail atomically. */
    cJSON *raw=cJSON_GetObjectItem(doc,"readings");
    cJSON *row=cJSON_GetArrayItem(cJSON_GetObjectItem(raw,"hourly"),0);
    cJSON_SetNumberValue(cJSON_GetObjectItem(row,"hour"),24);
    assert(!helios_weather_data_restore(doc));
    assert(chronos_core_get_weather_time()==clock_now);cJSON_Delete(doc);
    /* Date arithmetic across leap day and DST must use calendar days. */
    assert(helios_weather_day_offset(date(2028,2,28,23,0),date(2028,3,1,0,0))==2);
    setenv("TZ","America/New_York",1);tzset();
    assert(helios_weather_day_offset(date(2026,3,7,23,30),date(2026,3,8,23,0))==1);
    puts("weather: dated persistence, reboot, hour/midnight/year rollover, partial packets, filtering and DST passed");
    return 0;
}
