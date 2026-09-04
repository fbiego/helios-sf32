/**
 * @file app_screens.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "app_screens.h"

#include "../events/events.h"
#include "../watchfaces/watchface_manager.h"

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t * obj;
    lv_obj_t * drag_obj;
    int32_t start_y;
    int32_t panel_y;
    int32_t height;
    bool drag;
    bool open;
} control_panel_t;

typedef lv_obj_t * (*settings_item_create_cb_t)(lv_obj_t * parent);

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void screen_gesture_event_cb(lv_event_t * e);
static void lv_obj_animate_y(lv_obj_t *obj, int32_t start, int32_t end, int32_t duration);
static void scroll_y_end_listener_cb(lv_event_t *e);
static void scroll_x_end_listener_cb(lv_event_t *e);

static void home_screen_event_cb(lv_event_t * e);
static void selector_watchface_clicked_cb(lv_event_t * e);
static void on_control_widget_cb(lv_event_t * e);
static void control_widget_cb(void * var, int32_t value);
static void on_control_drag_cb(lv_event_t * e);
static void panel_events_cb(lv_event_code_t code);

static void settings_item_helper(lv_obj_t * parent, const void * icon,  const char * name, settings_item_create_cb_t function);
/**********************
 *  STATIC VARIABLES
 **********************/
static control_panel_t control_widget_home;

bool notifications_to_apps = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


lv_obj_t * screen_home(void)
{
    lv_obj_t * home = home_create();

    lv_obj_t * watchface_cont = lv_obj_find_by_name(home, "watchface_cont");
    if (watchface_cont) {
        lv_obj_clean(watchface_cont);
        helios_watchface_create_active(watchface_cont);
    }

    static gesture_map_t gestures = {
        .left  = { screen_applications, LV_SCR_LOAD_ANIM_OVER_LEFT },
        .right = { screen_notifications, LV_SCR_LOAD_ANIM_OVER_RIGHT },
        .up    = { screen_widgets, LV_SCR_LOAD_ANIM_OVER_TOP },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(home, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    control_widget_home.obj = control_panel_create(home);

    control_widget_home.drag_obj = lv_obj_find_by_name(control_widget_home.obj, "drag_control");
    if (control_widget_home.drag_obj) {
        lv_obj_add_event_cb(control_widget_home.drag_obj, on_control_drag_cb, LV_EVENT_ALL, home);
    }

    static int scroll_pos = 0;
    lv_obj_t * list = get_list_from_wd(control_widget_home.obj, "control_list");
    if (list) {
        lv_obj_scroll_to_y(list, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(list, scroll_y_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
        lv_obj_set_flag(list, LV_OBJ_FLAG_EVENT_BUBBLE, false);
    }

    lv_obj_set_align(control_widget_home.obj, LV_ALIGN_TOP_MID);

    control_widget_home.height = lv_obj_get_height(home);
    lv_obj_set_y(control_widget_home.obj, -control_widget_home.height);

    lv_obj_add_event_cb(home, home_screen_event_cb, LV_EVENT_ALL, NULL);

    return home;

}

lv_obj_t * screen_selector(void)
{
    lv_obj_t * selector = selector_create();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_home, LV_SCR_LOAD_ANIM_OUT_RIGHT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(selector, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    lv_obj_t * selector_panel = lv_obj_find_by_name(selector, "selector_cont");
    lv_obj_t * selector_cont = selector_panel ? wd_panel_get_container(selector_panel) : NULL;
    lv_obj_t * active_item = NULL;

    if (selector_cont) {
        lv_obj_clean(selector_cont);

        uint32_t count = helios_watchfaces_count();
        uint32_t active = helios_watchfaces_active_index();

        for (uint32_t i = 0; i < count; i++) {
            const helios_watchface_t * watchface = helios_watchfaces_get(i);
            if (!watchface) continue;

            lv_obj_t * item = face_preview_create(selector_cont,
                                                  helios_watchface_get_preview(watchface),
                                                  watchface->name);
            lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(item,
                                selector_watchface_clicked_cb,
                                LV_EVENT_CLICKED,
                                (void *)(uintptr_t)i);

            if (i == active) {
                lv_obj_add_state(item, LV_STATE_CHECKED);
                active_item = item;
            }
        }

        if (active_item) {
            lv_obj_update_layout(selector_cont);
            lv_obj_scroll_to_view(active_item, LV_ANIM_OFF);
        }
    }

    return selector;
}


lv_obj_t * screen_applications(void)
{

    lv_obj_t * apps = applications_create();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_home, LV_SCR_LOAD_ANIM_OUT_RIGHT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(apps, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);
    lv_obj_add_event_cb(apps, screen_applications_events_cb, LV_EVENT_ALL, NULL);

    static int scroll_pos = 0;
    lv_obj_t * app_list = get_list_from_wd(apps, "app_list");
    if (app_list) {
        lv_obj_scroll_to_y(app_list, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(app_list, scroll_y_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
    }

    return apps;
}

lv_obj_t * screen_notifications(void)
{

    lv_obj_t * notifications = notifications_create();

    static gesture_map_t gestures = {
        .left  = { screen_home, LV_SCR_LOAD_ANIM_OUT_LEFT },
        .right = { NULL, 0 },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    if (notifications_to_apps) {
        gesture_action_t apps = {
            .screen_fn = screen_applications, 
            .anim = LV_SCR_LOAD_ANIM_OUT_RIGHT,
            .transition = HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT
        };
        gestures.left.screen_fn = NULL;
        gestures.right = apps;
        notifications_to_apps = false;
    } else {
        gesture_action_t home = {
            .screen_fn = screen_home, 
            .anim = LV_SCR_LOAD_ANIM_OUT_LEFT
        };
        gestures.right.screen_fn = NULL;
        gestures.left = home;
    }

    lv_obj_add_event_cb(notifications, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    lv_obj_add_event_cb(notifications, screen_notifications_events_cb, LV_EVENT_ALL, NULL);

    static int scroll_pos = 0;
    lv_obj_t * list = get_list_from_wd(notifications, "notifications_list");
    if (list) {
        lv_obj_scroll_to_y(list, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(list, scroll_y_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
    }

    return notifications;
}

lv_obj_t * screen_widgets(void)
{

    lv_obj_t * widgets = widgets_create();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { NULL, 0 },
        .up    = { NULL, 0 },
        .down  = { screen_home, LV_SCR_LOAD_ANIM_OUT_BOTTOM },
    };

    lv_obj_add_event_cb(widgets, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    static int scroll_pos = 0;
    lv_obj_t * panel = lv_obj_get_child(widgets, 0);
    lv_obj_t * panel_container = panel ? wd_panel_get_container(panel) : NULL;
    if (panel_container) {
        lv_obj_update_layout(panel_container);
        lv_obj_scroll_to_x(panel_container, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(panel_container, scroll_x_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
    }

    return widgets;
}

lv_obj_t * screen_settings(void)
{

    lv_obj_t * settings = settings_create();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_applications, LV_SCR_LOAD_ANIM_OUT_RIGHT, HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(settings, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    static int scroll_pos = 0;
    lv_obj_t * list = get_list_from_wd(settings, "main_settings_list");
    if (list) {

        lv_obj_clean(list);

        settings_item_helper(list, icon_settings_monitor, "display", settings_display_create);
        settings_item_helper(list, icon_settings_smartwatch, "system", settings_system_create);
        settings_item_helper(list, icon_settings_bell, "alerts", settings_alert_create);
        settings_item_helper(list, icon_settings_power, "battery", settings_battery_create);
        settings_item_helper(list, icon_settings_harddisk, "storage", settings_storage_create);
        settings_item_helper(list, icon_settings_information, "about", settings_about_create);

        lv_obj_scroll_to_y(list, 1, LV_ANIM_OFF); /* Circular scroll trigger fix */
        lv_obj_scroll_to_y(list, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(list, scroll_y_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
    }

    return settings;
}

lv_obj_t * screen_weather(void)
{

    lv_obj_t * weather = weather_create();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_applications, LV_SCR_LOAD_ANIM_OUT_RIGHT, HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(weather, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);
    lv_obj_add_event_cb(weather, screen_weather_events_cb, LV_EVENT_ALL, NULL);

    static int scroll_pos = 0;
    lv_obj_t * list = get_list_from_wd(weather, "weather_list");
    if (list) {

        lv_obj_scroll_to_y(list, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(list, scroll_y_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
    }

    return weather;
}

lv_obj_t * screen_contacts(void)
{
    lv_obj_t * contacts = contacts_create();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_applications, LV_SCR_LOAD_ANIM_OUT_RIGHT, HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(contacts, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);
    lv_obj_add_event_cb(contacts, screen_contacts_events_cb, LV_EVENT_ALL, NULL);

    static int scroll_pos = 0;
    lv_obj_t * list = get_list_from_wd(contacts, "contacts_list");
    if (list) {

        lv_obj_scroll_to_y(list, scroll_pos, LV_ANIM_OFF);
        lv_obj_add_event_cb(list, scroll_y_end_listener_cb, LV_EVENT_SCROLL_END, &scroll_pos);
    }

    return contacts;
}

lv_obj_t * get_list_from_wd(lv_obj_t * parent, const char * name)
{
    lv_obj_t * list = NULL;
    lv_obj_t * wd_list = lv_obj_find_by_name(parent, name);
    if (wd_list) {
        list = wd_list_get_container(wd_list);
    }
    return list;
}

lv_obj_t * simple_app_screen_create(screen_create_cb_t create_cb)
{
    return simple_app_screen_create_transition(create_cb, HELIOS_SCREEN_TRANSITION_DEFAULT);
}

lv_obj_t * simple_app_screen_create_transition(screen_create_cb_t create_cb,
                                               helios_screen_transition_t close_transition)
{
    if (!create_cb) return NULL;

    lv_obj_t * screen = create_cb();

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_applications, LV_SCR_LOAD_ANIM_OUT_RIGHT, HELIOS_SCREEN_TRANSITION_DEFAULT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    gestures.right.transition = close_transition;

    lv_obj_add_event_cb(screen, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    return screen;
}

void helios_screen_load_transition(lv_obj_t * screen,
                                   lv_scr_load_anim_t fallback_anim,
                                   uint32_t duration,
                                   bool auto_del,
                                   helios_screen_transition_t transition)
{
    if (!screen) return;

    LV_UNUSED(transition);
    lv_screen_load_anim(screen, fallback_anim, duration, 0, auto_del);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void control_widget_cb(void * var, int32_t value)
{

    int32_t opa = lv_map(value, -control_widget_home.height, 0, 55, 255);
    lv_obj_set_y(var, value);
    lv_obj_set_style_bg_opa(var, opa, 0);

}

static void on_control_drag_cb(lv_event_t * e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * home = lv_event_get_user_data(e);

    if (code == LV_EVENT_PRESSED) {
        panel_events_cb(LV_EVENT_PRESSED);
    }

    if (code == LV_EVENT_PRESSING) {
        panel_events_cb(LV_EVENT_PRESSING);
    }

    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        panel_events_cb(LV_EVENT_RELEASED);
    }

}
 static void panel_events_cb(lv_event_code_t code)
{

    lv_indev_t * indev = lv_indev_active();

    if (code == LV_EVENT_PRESSED) {
        if(indev == NULL)  return;
        lv_point_t p;
        lv_indev_get_point(indev, &p);

        control_widget_home.start_y = p.y;
        control_widget_home.panel_y = lv_obj_get_y_aligned(control_widget_home.obj);
        control_widget_home.drag = true;
    }

    if (code == LV_EVENT_PRESSING && control_widget_home.drag) {

        if(indev == NULL)  return;
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);

        int32_t new_y = lv_obj_get_y_aligned(control_widget_home.obj) + vect.y;

        if (new_y > 0) new_y = 0;
        if (new_y < -control_widget_home.height) new_y = -control_widget_home.height;

        int32_t opa = lv_map(new_y, -control_widget_home.height, 0, 55, 255);
        lv_obj_set_style_bg_opa(control_widget_home.obj, opa, 0);

        lv_obj_set_y(control_widget_home.obj, new_y);
    }

    if (code == LV_EVENT_RELEASED && control_widget_home.drag) {

        control_widget_home.drag = false;

        int32_t y = lv_obj_get_y_aligned(control_widget_home.obj);
        int32_t threshold = control_widget_home.height / 5;

        /* Determine if drag exceeded threshold */
        if (LV_ABS(y - control_widget_home.panel_y) > threshold) {
            control_widget_home.open = !control_widget_home.open;
        }

        /* Animate to final state */
        int32_t target_y = control_widget_home.open ? 0 : -control_widget_home.height;
        lv_obj_animate_y(control_widget_home.obj, y, target_y, 200);

    }
}

static void home_screen_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_LONG_PRESSED) {
        helios_screen_load_transition(screen_selector(),
                                      LV_SCR_LOAD_ANIM_FADE_ON,
                                      HELIOS_SCREEN_TRANSITION_TIME,
                                      true,
                                      HELIOS_SCREEN_TRANSITION_DEFAULT);
        return;
    }
    
    if (code == LV_EVENT_PRESSED) {
        panel_events_cb(LV_EVENT_PRESSED);
    }

    if (code == LV_EVENT_PRESSING) {
        panel_events_cb(LV_EVENT_PRESSING);
    }

    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        panel_events_cb(LV_EVENT_RELEASED);
    }

}

static void selector_watchface_clicked_cb(lv_event_t * e)
{
    uint32_t index = (uint32_t)(uintptr_t)lv_event_get_user_data(e);

    if (!helios_watchfaces_set_active(index)) return;

    helios_screen_load_transition(screen_home(),
                                  LV_SCR_LOAD_ANIM_FADE_OUT,
                                  HELIOS_SCREEN_TRANSITION_TIME,
                                  true,
                                  HELIOS_SCREEN_TRANSITION_DEFAULT);
}

static void lv_obj_animate_y(lv_obj_t *obj, int32_t start, int32_t end, int32_t duration)
{
    lv_anim_t   anim;
    lv_anim_init(&anim);

    /* Set the "animator" function */
    lv_anim_set_exec_cb(&anim, control_widget_cb);
    /* Set target of the Animation */
    lv_anim_set_var(&anim, obj);
    /* Length of the Animation [ms] */
    lv_anim_set_duration(&anim, duration);
    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&anim, start, end);
    lv_anim_start(&anim);   /* Start the Animation */

}

static void screen_gesture_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_GESTURE) return;

    gesture_map_t * data = lv_event_get_user_data(e);
    if (!data) return;   // protect user_data

    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    gesture_action_t * action = NULL;

    switch (dir) {
        case LV_DIR_LEFT:   action = &data->left;  break;
        case LV_DIR_RIGHT:  action = &data->right; break;
        case LV_DIR_TOP:    action = &data->up;    break;
        case LV_DIR_BOTTOM: action = &data->down;  break;
        default: return;
    }

    if (!action) return;

    if (!action->screen_fn) return;

    if (action->screen_fn) {
        helios_screen_load_transition(
            action->screen_fn(),
            action->anim,
            HELIOS_SCREEN_TRANSITION_TIME,
            true,
            action->transition
        );
    }
}

static void scroll_y_end_listener_cb(lv_event_t *e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t * scroll_pos = lv_event_get_user_data(e);
    *scroll_pos = lv_obj_get_scroll_y(obj);
}

static void scroll_x_end_listener_cb(lv_event_t *e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t * scroll_pos = lv_event_get_user_data(e);
    *scroll_pos = lv_obj_get_scroll_x(obj);
}

static void settings_item_clicked_cb(lv_event_t * e)
{
    settings_item_create_cb_t function = (settings_item_create_cb_t)lv_event_get_user_data(e);
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(screen, 255, 0);

    function(screen);

    static gesture_map_t gestures = {
        .left  = { NULL, 0 },
        .right = { screen_settings, LV_SCR_LOAD_ANIM_OUT_RIGHT, HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT },
        .up    = { NULL, 0 },
        .down  = { NULL, 0 },
    };

    lv_obj_add_event_cb(screen, screen_gesture_event_cb, LV_EVENT_GESTURE, &gestures);

    helios_screen_load_transition(screen,
                                  LV_SCR_LOAD_ANIM_OVER_LEFT,
                                  HELIOS_SCREEN_TRANSITION_TIME,
                                  true,
                                  HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
    
}

static void settings_item_helper(lv_obj_t * parent, const void * icon,  const char * name, settings_item_create_cb_t function)
{
    lv_obj_t * item = app_item_create(parent, icon, name, name);
    lv_obj_add_event_cb(item, settings_item_clicked_cb, LV_EVENT_CLICKED, function);
}
