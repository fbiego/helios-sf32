/**
 * @file helios_ui.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "helios_ui.h"
#include "helios_ui_gen.h"
#include "custom/apps/app_init.h"
#include "custom/apps/weather/weather.h"


#if defined (LV_SIM_BUILD)
#include <time.h>
#endif

/*********************
 *      DEFINES
 *********************/

#define LANG_TAG_COUNT (sizeof(lang_tags) / sizeof(lang_tags[0]))


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void on_language_change(lv_event_t * e);
static void language_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void screen_res_cb(lv_event_t *e);

static const char * lang_tag_get_by_index(int index);
static int lang_tag_get_index(const char * str, int def);

static void set_screen(int32_t w, int32_t h);


#if defined (LV_SIM_BUILD)
static void update_time_cb(lv_timer_t *timer);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/
static const char * lang_tags[] = {
    "en", "pt", "de", "es", "fr", "hu", "ru", "el", "th", "zh", "ja", "hi", "vi"
};

static const char * days_short[] = {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat"
};
static const char * days_long[] = {
    "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"
};

static const char * months_short[] = {
    "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"
};
static const char * months_long[] = {
    "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_ui_init(const char * asset_path)
{
    helios_ui_init_gen(asset_path);

    helios_subjects_init();
    helios_apps_init_all();


    /* Add your own custom code here if needed */

    lv_obj_add_event_cb(lv_screen_active(), on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, NULL);

    lv_subject_add_observer(&sb_language, language_observer_cb, NULL);

    helios_subject_set_music_icon((void *)icon_music);
    helios_subject_set_music_state_icon((void *)icon_music_play_32);
    

    lv_display_t *disp = lv_display_get_default();
    if (disp) {
#if defined(LV_EDITOR_PREVIEW)
        lv_display_add_event_cb(disp, screen_res_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
        lv_display_send_event(disp, LV_EVENT_RESOLUTION_CHANGED, NULL);
        
        helios_subject_set_system_connection(1);
#else
        int32_t w = lv_display_get_vertical_resolution(disp);
        int32_t h = lv_display_get_horizontal_resolution(disp);
        set_screen(w, h);
#endif
    }


#if defined(LV_EDITOR_PREVIEW) || defined (LV_SIM_BUILD)
    lv_obj_set_style_bg_color(lv_layer_top(), lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(lv_layer_top(), 0, 0);
#endif

#if defined (LV_SIM_BUILD)
    helios_subject_set_system_connection(1);
    
    lv_timer_t * timer = lv_timer_create(update_time_cb, 1000, NULL);
    
#endif


    char buf[32];
    lv_snprintf(buf, sizeof(buf), "v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    lv_subject_copy_string(&sb_lvgl_version, buf);

#if !defined(LV_EDITOR_PREVIEW)
    lv_screen_load(simulator_create());
#endif
}


int32_t get_screen_width(int32_t index)
{
    switch (index)
    {
        case 0: return 466;
        case 1: return 360;
        case 2: return 240;
    }
    return 466;

}

#if defined(LV_EDITOR_PREVIEW) || defined (LV_SIM_BUILD)
void  helios_subject_screen_brightness_change(int32_t value)
{
    lv_obj_set_style_bg_opa(lv_layer_top(), lv_map(value, 0, 100, 255, 0), 0);
}
#endif

void helios_subject_time_month_change(int32_t value)
{
    lv_subject_copy_string(&sb_time_month_short, months_short[(value - 1) % 12]);
    lv_subject_copy_string(&sb_time_month_long, months_long[(value - 1) % 12]);
}

void helios_subject_time_weekday_change(int32_t value)
{
    lv_subject_copy_string(&sb_time_weekday_short, days_short[value % 7]);
    lv_subject_copy_string(&sb_time_weekday_long, days_long[value % 7]);
}

void helios_subject_weather_code_change(int32_t value)
{
    helios_subject_set_weather_icon((void *)helios_weather_icon_get(value));
    helios_subject_set_weather_condition(helios_weather_condition_get(value));
}

void helios_subject_system_connection_change(int32_t value)
{
    helios_subject_set_system_connection_str(value ? "connected" : "disconnected");
}

void helios_subject_phone_charging_change(int32_t value)
{
    helios_subject_set_phone_charging_str(value ? "yes" : "no");
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void on_language_change(lv_event_t * e)
{
    const char * language = lv_event_get_param(e);
    LV_LOG_USER("Language changed to %s", language);

    lv_subject_set_int(&sb_language, lang_tag_get_index(language, 0));
}

static void language_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    int32_t index = lv_subject_get_int(subject);
    LV_LOG_USER("Language index %d", index);

    lv_translation_set_language(lang_tag_get_by_index(index));

}

static void set_screen(int32_t w, int32_t h)
{
    lv_subject_set_int(&sb_screen_width, w);
    lv_subject_set_int(&sb_screen_height, h);

    lv_subject_set_int(&sb_screen_type, w == h ? 0 : 1);
    lv_subject_set_int(&sb_list_circular_mode, w == h ? 1 : 0);


    char buf[32];
    lv_snprintf(buf, sizeof(buf), "%dx%d", w, h);
    lv_subject_copy_string(&sb_screen_res, buf);
    
    if (w >= 390) {
        lv_subject_set_int(&sb_screen_size, 0); // 466
    } else if (w >= 300) {
        lv_subject_set_int(&sb_screen_size, 1); // 360
    } else {
        lv_subject_set_int(&sb_screen_size, 2); // 240
    }
}


static void screen_res_cb(lv_event_t *e)
{
    lv_display_t * disp = lv_event_get_target(e);
    int32_t w = lv_display_get_original_horizontal_resolution(disp);
    int32_t h = lv_display_get_original_vertical_resolution(disp);

    set_screen(w, h);
}



static const char * lang_tag_get_by_index(int index)
{
    if(index < 0 || index >= LANG_TAG_COUNT) {
        return lang_tags[0];
    }

    return lang_tags[index];
}

static int lang_tag_get_index(const char * str, int def)
{
    if(str == NULL) {
        return 0;
    }

    for(int i = 0; i < LANG_TAG_COUNT; i++) {
        if(lv_streq(lang_tags[i], str)) {
            return i;
        }
    }

    return 0;
}


#if defined (LV_SIM_BUILD)
static void update_time_cb(lv_timer_t *timer)
{
    time_t now = time(0);
    struct tm *tm_now = localtime(&now);

    char time_now[16] = "--:--";
    lv_snprintf(time_now, sizeof(time_now), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);
    helios_subject_set_time_string(time_now);

    helios_subject_set_time_hour(tm_now->tm_hour);
    helios_subject_set_time_minute(tm_now->tm_min);
    helios_subject_set_time_seconds(tm_now->tm_sec);
    helios_subject_set_time_day(tm_now->tm_mday);
    helios_subject_set_time_month(tm_now->tm_mon + 1);
    helios_subject_set_time_year(tm_now->tm_year + 1900);
    helios_subject_set_time_weekday(tm_now->tm_wday);
}
#endif
