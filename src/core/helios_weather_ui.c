#include "helios_weather_ui.h"
#include "helios_weather_data.h"
#include "helios_ui/helios_ui.h"
#include "helios_ui/custom/apps/weather/weather.h"
#include <stdio.h>

static const char *helios_weather_day_name(time_t now, int32_t offset)
{
    static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"};
    struct tm *tm_now = localtime(&now);
    int32_t day = tm_now ? tm_now->tm_wday : 0;

    day = (day + offset) % 7;
    if (day < 0)
        day += 7;

    return days[day];
}

static void helios_weather_format_temp(char *buf, size_t buf_size, int32_t temp)
{
    snprintf(buf, buf_size, "%d°", (int)temp);
}

static void helios_weather_format_update_time(char *buf, size_t buf_size, uint32_t timestamp)
{
    if (!timestamp) { snprintf(buf, buf_size, "--:--"); return; }
    time_t update_time = (time_t)timestamp;
    struct tm *tm_update = localtime(&update_time);

    if (tm_update)
        snprintf(buf, buf_size, "%02d:%02d", tm_update->tm_hour, tm_update->tm_min);
    else
        snprintf(buf, buf_size, "--:--");
}

void helios_weather_render(time_t now)
{
    struct tm *calendar = localtime(&now);
    int start_hour = calendar ? calendar->tm_hour : 23;
    const chronos_weather_t *current = helios_weather_daily_at(now, 0);
    const chronos_hourly_forecast_t *current_hour = helios_weather_hour_at(now, start_hour);
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

    if (current_hour) {
        helios_subject_set_weather_code(current_hour->icon);
        helios_subject_set_weather_icon((void *)helios_weather_icon_get(current_hour->icon));
        helios_subject_set_weather_condition(helios_weather_condition_get(current_hour->icon));
        helios_subject_set_weather_temp(current_hour->temp);
        helios_subject_set_weather_uv(current_hour->uv);
    }
    helios_subject_set_weather_humidity(current_hour ? current_hour->humidity : 0);

    helios_weather_format_update_time(update_time, sizeof(update_time), chronos_core_get_weather_time());
    helios_subject_set_weather_update_time(update_time);
    helios_subject_set_weather_temp_unit("C");

    helios_weather_daily_clear();
    for (int i = 1; i <= (int)HELIOS_DAILY_FORECAST_MAX; i++) {
        const chronos_weather_t *weather = helios_weather_daily_at(now, i);
        char temp[16];

        if (!weather)
            continue;

        if (weather->high != 0 || weather->low != 0)
            snprintf(temp, sizeof(temp), "%d/%d°", weather->high, weather->low);
        else
            helios_weather_format_temp(temp, sizeof(temp), weather->temp);

        helios_weather_daily_add(weather->icon, helios_weather_day_name(now, i), temp);
    }

    helios_weather_hourly_clear();
    uint32_t displayed = 0;

    for (int hour = start_hour + 1; hour < (int)CHRONOS_FORECAST_SIZE && displayed < HELIOS_HOURLY_FORECAST_MAX; hour++) {
        const chronos_hourly_forecast_t *forecast;
        char time_label[16];
        char temp[16];
        char humidity[16];

        forecast = helios_weather_hour_at(now, hour);
        if (!forecast)
            continue;

        snprintf(time_label, sizeof(time_label), "%02d:00", forecast->hour);
        helios_weather_format_temp(temp, sizeof(temp), forecast->temp);
        snprintf(humidity, sizeof(humidity), "%d%%", forecast->humidity);
        helios_weather_hourly_add(forecast->icon, time_label, temp, humidity);
        displayed++;
    }

}

void helios_weather_poll(time_t now)
{
    static int year = -1, day = -1, hour = -1;
    struct tm *calendar = localtime(&now);
    if (!calendar || (year == calendar->tm_year && day == calendar->tm_yday && hour == calendar->tm_hour)) return;
    year = calendar->tm_year; day = calendar->tm_yday; hour = calendar->tm_hour;
    helios_weather_render(now);
}
