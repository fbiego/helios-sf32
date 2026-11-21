#include "helios_weather_data.h"
#include <limits.h>
#include <string.h>

/* Gregorian civil date as an ordinal; localtime supplies the local date. */
static int ordinal(const struct tm *calendar)
{
    int y = calendar->tm_year + 1900, m = calendar->tm_mon + 1;
    y -= m <= 2;
    int era = y / 400, year = y - era * 400;
    int day = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + calendar->tm_mday - 1;
    return era * 146097 + year * 365 + year / 4 - year / 100 + day;
}

int helios_weather_day_offset(uint32_t received, time_t now)
{
    if (!received) return -1;
    time_t then = received;
    struct tm *calendar = localtime(&then);
    if (!calendar) return -1;
    int base = ordinal(calendar);
    calendar = localtime(&now);
    return calendar ? ordinal(calendar) - base : -1;
}

const chronos_weather_t *helios_weather_daily_at(time_t now, int offset)
{
    chronos_core_t *core = chronos_core_get_state();
    int day = helios_weather_day_offset(core->weather_daily_time, now);
    if (day < 0 || offset < 0 || day + offset >= core->weather_size) return NULL;
    return &core->weather[day + offset];
}

const chronos_hourly_forecast_t *helios_weather_hour_at(time_t now, int hour)
{
    chronos_core_t *core = chronos_core_get_state();
    if (hour < 0 || hour >= (int)CHRONOS_FORECAST_SIZE || !core->hourly_forecast_valid[hour] ||
        helios_weather_day_offset(core->weather_hourly_time[hour], now) != 0) return NULL;
    return &core->hourly_forecast[hour];
}

static bool number(cJSON *doc, const char *key, double value)
{ return cJSON_AddNumberToObject(doc, key, value) != NULL; }

bool helios_weather_data_save(cJSON *doc)
{
    const chronos_core_t *core = chronos_core_get_state();
    cJSON *raw = cJSON_AddObjectToObject(doc, "readings");
    if (!raw || !number(raw, "received", core->weather_time) ||
        !number(raw, "daily_received", core->weather_daily_time)) return false;
    cJSON *days = cJSON_AddArrayToObject(raw, "daily");
    cJSON *hours = cJSON_AddArrayToObject(raw, "hourly");
    if (!days || !hours) return false;
    for (int i = 0; i < core->weather_size && i < (int)CHRONOS_WEATHER_SIZE; i++) {
        const chronos_weather_t *v = &core->weather[i];
        cJSON *row = cJSON_CreateObject();
        if (!row || !cJSON_AddItemToArray(days, row)) { cJSON_Delete(row); return false; }
        if (!number(row,"icon",v->icon) || !number(row,"temp",v->temp) ||
            !number(row,"high",v->high) || !number(row,"low",v->low) ||
            !number(row,"uv",v->uv) || !number(row,"pressure",v->pressure)) return false;
    }
    for (int i = 0; i < (int)CHRONOS_FORECAST_SIZE; i++) {
        if (!core->hourly_forecast_valid[i]) continue;
        const chronos_hourly_forecast_t *v = &core->hourly_forecast[i];
        cJSON *row = cJSON_CreateObject();
        if (!row || !cJSON_AddItemToArray(hours, row)) { cJSON_Delete(row); return false; }
        if (!number(row,"received",core->weather_hourly_time[i]) || !number(row,"hour",i) ||
            !number(row,"icon",v->icon) || !number(row,"temp",v->temp) ||
            !number(row,"humidity",v->humidity) || !number(row,"uv",v->uv) ||
            !number(row,"wind",v->wind)) return false;
    }
    return true;
}

static bool integer(const cJSON *doc, const char *key, double min, double max, double *out)
{
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(doc, key);
    if (!cJSON_IsNumber(v) || !(v->valuedouble >= min && v->valuedouble <= max) ||
        v->valuedouble != (double)(int64_t)v->valuedouble) return false;
    *out = v->valuedouble;
    return true;
}

bool helios_weather_data_restore(const cJSON *doc)
{
    const cJSON *raw = cJSON_GetObjectItemCaseSensitive(doc,"readings");
    if (!raw) return true;
    const cJSON *days = cJSON_GetObjectItemCaseSensitive(raw,"daily");
    const cJSON *hours = cJSON_GetObjectItemCaseSensitive(raw,"hourly"), *row;
    double received, daily_received;
    if (!cJSON_IsObject(raw) || !cJSON_IsArray(days) || !cJSON_IsArray(hours) ||
        cJSON_GetArraySize(days) > (int)CHRONOS_WEATHER_SIZE || cJSON_GetArraySize(hours) > (int)CHRONOS_FORECAST_SIZE ||
        !integer(raw,"received",0,UINT32_MAX,&received) ||
        !integer(raw,"daily_received",0,UINT32_MAX,&daily_received)) return false;
    chronos_weather_t daily[CHRONOS_WEATHER_SIZE] = {0};
    chronos_hourly_forecast_t hourly[CHRONOS_FORECAST_SIZE] = {0};
    uint32_t stamps[CHRONOS_FORECAST_SIZE] = {0};
    bool valid[CHRONOS_FORECAST_SIZE] = {0};
    int count = 0;
#define READ(key, target, min, max) do { double n; if (!integer(row,key,min,max,&n)) return false; target = n; } while (0)
    cJSON_ArrayForEach(row,days) {
        chronos_weather_t *v = &daily[count];
        v->day = count++;
        READ("icon",v->icon,0,15); READ("temp",v->temp,-255,255);
        READ("high",v->high,-127,127); READ("low",v->low,-127,127);
        READ("uv",v->uv,0,255); READ("pressure",v->pressure,0,65535);
    }
    cJSON_ArrayForEach(row,hours) {
        int hour;
        READ("hour",hour,0,CHRONOS_FORECAST_SIZE-1);
        if (valid[hour]) return false;
        valid[hour] = true;
        chronos_hourly_forecast_t *v = &hourly[hour]; v->hour = hour;
        READ("received",stamps[hour],0,UINT32_MAX);
        READ("icon",v->icon,0,15); READ("temp",v->temp,-255,255);
        READ("humidity",v->humidity,0,255); READ("uv",v->uv,0,255); READ("wind",v->wind,0,65535);
    }
#undef READ
    chronos_core_t *core = chronos_core_get_state();
    memcpy(core->weather,daily,sizeof(daily)); core->weather_size = count;
    memcpy(core->hourly_forecast,hourly,sizeof(hourly));
    memcpy(core->hourly_forecast_valid,valid,sizeof(valid));
    memcpy(core->weather_hourly_time,stamps,sizeof(stamps));
    core->weather_time = received; core->weather_daily_time = daily_received;
    const cJSON *city = cJSON_GetObjectItemCaseSensitive(doc,"city");
    if (cJSON_IsString(city)) {
        strncpy(core->weather_city,city->valuestring,sizeof(core->weather_city)-1);
        core->weather_city[sizeof(core->weather_city)-1] = '\0';
    }
    return true;
}
