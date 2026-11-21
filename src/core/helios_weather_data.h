#ifndef HELIOS_WEATHER_DATA_H
#define HELIOS_WEATHER_DATA_H
#include "chronos_core.h"
#include "cJSON.h"
#include <time.h>

/* Calendar-day offset from receipt, independent of elapsed hours/DST. */
int helios_weather_day_offset(uint32_t received, time_t now);
const chronos_weather_t *helios_weather_daily_at(time_t now, int offset);
const chronos_hourly_forecast_t *helios_weather_hour_at(time_t now, int hour);
bool helios_weather_data_save(cJSON *doc);
/* Restores raw readings into Chronos. Legacy documents have no raw readings. */
bool helios_weather_data_restore(const cJSON *doc);
#endif
