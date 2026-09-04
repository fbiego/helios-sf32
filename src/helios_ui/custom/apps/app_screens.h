/**
 * @file app_screens.h
 *
 */

#ifndef HELIOS_APP_SCREENS_H
#define HELIOS_APP_SCREENS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#include "../../helios_ui_gen.h"

/*********************
 *      DEFINES
 *********************/
#define HELIOS_SCREEN_TRANSITION_TIME 200
/**********************
 *      TYPEDEFS
 **********************/
typedef lv_obj_t * (*screen_create_cb_t)(void);

typedef enum {
    HELIOS_SCREEN_TRANSITION_DEFAULT = 0,
    HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT,
    HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT,
} helios_screen_transition_t;

typedef struct {
    lv_obj_t * (*screen_fn)(void);   // function that returns screen object
    lv_scr_load_anim_t anim;         // animation to use
    helios_screen_transition_t transition;
} gesture_action_t;

typedef struct {
    gesture_action_t left;
    gesture_action_t right;
    gesture_action_t up;
    gesture_action_t down;
} gesture_map_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * screen_home(void);

lv_obj_t * screen_applications(void);

lv_obj_t * screen_notifications(void);

lv_obj_t * screen_settings(void);

lv_obj_t * screen_widgets(void);

lv_obj_t * screen_selector(void);

lv_obj_t * screen_weather(void);

lv_obj_t * screen_contacts(void);

lv_obj_t * simple_app_screen_create(screen_create_cb_t create_cb);
lv_obj_t * simple_app_screen_create_transition(screen_create_cb_t create_cb,
                                               helios_screen_transition_t close_transition);

lv_obj_t * get_list_from_wd(lv_obj_t * parent, const char * name);

void helios_screen_load_transition(lv_obj_t * screen,
                                   lv_scr_load_anim_t fallback_anim,
                                   uint32_t duration,
                                   bool auto_del,
                                   helios_screen_transition_t transition);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_APP_SCREENS_H*/
