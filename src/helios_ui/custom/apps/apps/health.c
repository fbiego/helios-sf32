

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#include "../../../helios_ui.h"

#include "../app_manager.h"

void __attribute__((weak)) health_events_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOADED)
    {
        LV_LOG_USER("Health screen opened");
    }

    if (code == LV_EVENT_SCREEN_UNLOAD_START)
    {
        LV_LOG_USER("Health screen closing");
    }
}

lv_obj_t *my_health_app_create(void)
{
    lv_obj_t *screen = sc_health_create();

    lv_obj_add_event_cb(screen, health_events_cb, LV_EVENT_ALL, NULL);

    return screen;
}

HELIOS_REGISTER_SIMPLE_APP_TRANSITION(
    icon_measurement,
    "Heart",
    "health",
    my_health_app_create,
    LV_SCR_LOAD_ANIM_OVER_LEFT,
    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT
);
