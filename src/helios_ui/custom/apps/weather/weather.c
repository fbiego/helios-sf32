/**
 * @file weather.c
 * @brief Weather data API
 */

/*********************
 *      INCLUDES
 *********************/

#include "weather.h"
#include "../../../helios_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_WEATHER_OBSERVER_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    helios_weather_observer_cb_t cb;
    void * user_data;
} helios_weather_observer_t;

/***********************
 *  STATIC VARIABLES
 **********************/

static helios_hourly_forecast_t hourly[HELIOS_HOURLY_FORECAST_MAX];
static helios_daily_forecast_t daily[HELIOS_DAILY_FORECAST_MAX];
static helios_weather_observer_t observers[HELIOS_WEATHER_OBSERVER_MAX];
static uint32_t hourly_count;
static uint32_t daily_count;
static bool inited;

static const void ** weather_icons[] = {
    &icon_wt_sun_cloud,
    &icon_wt_sun,
    &icon_wt_snow,
    &icon_wt_rain,
    &icon_wt_cloud,
    &icon_wt_tornado,
    &icon_wt_wind,
    &icon_wt_haze,
};

static const char * weather_conditions[] = {
    "sun_cloud",
    "sunny",
    "snow",
    "rainy",
    "cloudy",
    "tornado",
    "wind",
    "haze",
};

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void notify_observers(void);
static uint32_t normalize_icon_id(int32_t icon_id);
static void copy_text(char * dst, uint32_t dst_size, const char * src);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_weather_init(void)
{
    if (inited) return;
    inited = true;

    helios_weather_hourly_add(HELIOS_WEATHER_ICON_CLOUD, "12:00", "22°", "35%");
    helios_weather_hourly_add(HELIOS_WEATHER_ICON_SUN, "13:00", "23°", "33%");
    helios_weather_hourly_add(HELIOS_WEATHER_ICON_SUN, "14:00", "23°", "34%");
    helios_weather_hourly_add(HELIOS_WEATHER_ICON_RAIN, "15:00", "21°", "39%");

    helios_weather_daily_add(HELIOS_WEATHER_ICON_SUN, "Sun", "19°");
    helios_weather_daily_add(HELIOS_WEATHER_ICON_SUN_CLOUD, "Mon", "21°");
    helios_weather_daily_add(HELIOS_WEATHER_ICON_CLOUD, "Tue", "20°");
    helios_weather_daily_add(HELIOS_WEATHER_ICON_RAIN, "Wed", "18°");
}

uint32_t helios_weather_hourly_count(void)
{
    return hourly_count;
}

const helios_hourly_forecast_t * helios_weather_hourly_get(uint32_t index)
{
    if (index >= hourly_count) return NULL;
    return &hourly[index];
}

uint32_t helios_weather_daily_count(void)
{
    return daily_count;
}

const helios_daily_forecast_t * helios_weather_daily_get(uint32_t index)
{
    if (index >= daily_count) return NULL;
    return &daily[index];
}

const void * helios_weather_icon_get(int32_t icon_id)
{
    const void * icon = *weather_icons[normalize_icon_id(icon_id)];
    return icon ? icon : icon_weather;
}

const char * helios_weather_condition_get(int32_t icon_id)
{
    return weather_conditions[normalize_icon_id(icon_id)];
}

bool helios_weather_hourly_add(int32_t icon_id, const char * time, const char * temp, const char * humidity)
{
    lv_lock();

    if (hourly_count >= HELIOS_HOURLY_FORECAST_MAX) {
        lv_unlock();
        return false;
    }

    helios_hourly_forecast_t * item = &hourly[hourly_count++];
    item->icon_id = icon_id;
    item->icon = helios_weather_icon_get(icon_id);
    copy_text(item->time, sizeof(item->time), time);
    copy_text(item->temp, sizeof(item->temp), temp);
    copy_text(item->humidity, sizeof(item->humidity), humidity);

    notify_observers();
    lv_unlock();
    return true;
}

bool helios_weather_daily_add(int32_t icon_id, const char * day, const char * temp)
{
    lv_lock();

    if (daily_count >= HELIOS_DAILY_FORECAST_MAX) {
        lv_unlock();
        return false;
    }

    helios_daily_forecast_t * item = &daily[daily_count++];
    item->icon_id = icon_id;
    item->icon = helios_weather_icon_get(icon_id);
    copy_text(item->day, sizeof(item->day), day);
    copy_text(item->temp, sizeof(item->temp), temp);

    notify_observers();
    lv_unlock();
    return true;
}

void helios_weather_hourly_clear(void)
{
    lv_lock();
    hourly_count = 0;
    notify_observers();
    lv_unlock();
}

void helios_weather_daily_clear(void)
{
    lv_lock();
    daily_count = 0;
    notify_observers();
    lv_unlock();
}

bool helios_weather_observer_add(helios_weather_observer_cb_t cb, void * user_data)
{
    lv_lock();

    if (!cb) {
        lv_unlock();
        return false;
    }

    for (uint32_t i = 0; i < HELIOS_WEATHER_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            lv_unlock();
            return true;
        }
    }

    for (uint32_t i = 0; i < HELIOS_WEATHER_OBSERVER_MAX; i++) {
        if (!observers[i].cb) {
            observers[i].cb = cb;
            observers[i].user_data = user_data;
            lv_unlock();
            return true;
        }
    }

    lv_unlock();
    return false;
}

void helios_weather_observer_remove(helios_weather_observer_cb_t cb, void * user_data)
{
    lv_lock();
    for (uint32_t i = 0; i < HELIOS_WEATHER_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            observers[i].cb = NULL;
            observers[i].user_data = NULL;
        }
    }
    lv_unlock();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void notify_observers(void)
{
    for (uint32_t i = 0; i < HELIOS_WEATHER_OBSERVER_MAX; i++) {
        if (observers[i].cb) {
            observers[i].cb(HELIOS_WEATHER_EVENT_CHANGED, observers[i].user_data);
        }
    }
}

static uint32_t normalize_icon_id(int32_t icon_id)
{
    int32_t count = HELIOS_WEATHER_ICON_COUNT;
    int32_t normalized = icon_id % count;
    if (normalized < 0) normalized += count;
    return (uint32_t)normalized;
}

static void copy_text(char * dst, uint32_t dst_size, const char * src)
{
    if (!dst || dst_size == 0) return;

    if (!src) src = "";
    lv_strncpy(dst, src, dst_size);
    dst[dst_size - 1] = '\0';
}
