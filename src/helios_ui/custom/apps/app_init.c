/**
 * @file app_init.c
 * @brief Built-in app initialization
 */

/*********************
*      INCLUDES
*********************/

#include "app_init.h"
#include "app_screens.h"
#include "../events/events.h"

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/***********************
*  STATIC VARIABLES
**********************/
extern bool notifications_to_apps;

/***********************
*  STATIC PROTOTYPES
**********************/
static lv_obj_t * notifications_app_create(void);

/**********************
*   GLOBAL FUNCTIONS
**********************/

void helios_apps_init_all(void)
{
    helios_apps_init();
    helios_contacts_init();
    helios_notifications_init();
    helios_stopwatch_init();
    helios_weather_init();
    helios_watchfaces_init_all();

    helios_apps_register_transition(icon_contacts,
                                    "Contacts",
                                    "contacts",
                                    screen_contacts,
                                    LV_SCR_LOAD_ANIM_OVER_LEFT,
                                    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register(icon_folder, "Files", "files", NULL, LV_SCR_LOAD_ANIM_OVER_LEFT);
    helios_apps_register(icon_running, "Fitness", "fitness", NULL, LV_SCR_LOAD_ANIM_OVER_LEFT);
    // helios_apps_register(icon_measurement, "Health", "health", NULL, LV_SCR_LOAD_ANIM_OVER_LEFT);
    helios_apps_register_simple_transition(icon_music,
                                           "Music",
                                           "music",
                                           music_create,
                                           LV_SCR_LOAD_ANIM_OVER_LEFT,
                                           HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register_simple_transition(icon_navigation,
                                           "Navigation",
                                           "navigation",
                                           navigation_create,
                                           LV_SCR_LOAD_ANIM_OVER_LEFT,
                                           HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register_transition(icon_chat,
                                    "Notifications",
                                    "notifications",
                                    notifications_app_create,
                                    LV_SCR_LOAD_ANIM_OVER_LEFT,
                                    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register_simple_transition(icon_phone_link,
                                           "Phone Link",
                                           "phone_link",
                                           phone_create,
                                           LV_SCR_LOAD_ANIM_OVER_LEFT,
                                           HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register_transition(icon_gear,
                                    "Settings",
                                    "settings",
                                    screen_settings,
                                    LV_SCR_LOAD_ANIM_OVER_LEFT,
                                    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register(icon_sleep, "Sleep", "sleep", NULL, LV_SCR_LOAD_ANIM_OVER_LEFT);
    helios_apps_register_simple_events_transition(icon_stopwatch,
                                                  "Stopwatch",
                                                  "stopwatch",
                                                  stopwatch_create,
                                                  LV_SCR_LOAD_ANIM_OVER_LEFT,
                                                  screen_stopwatch_events_cb,
                                                  HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    helios_apps_register_simple(icon_timer, "Timer", "timer", sc_timer_create, LV_SCR_LOAD_ANIM_OVER_LEFT);
    helios_apps_register_transition(icon_weather,
                                    "Weather",
                                    "weather",
                                    screen_weather,
                                    LV_SCR_LOAD_ANIM_OVER_LEFT,
                                    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);

    helios_apps_run_initializers();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * notifications_app_create(void)
{
    notifications_to_apps = true;
    return screen_notifications();
}
