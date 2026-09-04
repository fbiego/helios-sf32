/**
 * @file app_manager.h
 * @brief Application registry API
 */

#ifndef HELIOS_APP_MANAGER_H
#define HELIOS_APP_MANAGER_H

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

#include "app_screens.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_APPS_MAX       24
#define HELIOS_APP_NAME_MAX   32
#define HELIOS_APP_TAG_MAX    32
#define HELIOS_APP_INITIALIZERS_MAX 16

#if defined(__GNUC__) || defined(__clang__)
    #define HELIOS_CONSTRUCTOR_ATTR __attribute__((constructor))
#else
    #define HELIOS_CONSTRUCTOR_ATTR
#endif

#define HELIOS_CONCAT_INNER(a, b) a##b
#define HELIOS_CONCAT(a, b) HELIOS_CONCAT_INNER(a, b)
#define HELIOS_UNIQUE_NAME(prefix) HELIOS_CONCAT(prefix, __LINE__)

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_obj_t * (*helios_app_create_cb_t)(void);
typedef void (*helios_app_initializer_cb_t)(void);

typedef enum {
    HELIOS_APP_LAUNCH_DIRECT,
    HELIOS_APP_LAUNCH_SIMPLE,
} helios_app_launch_mode_t;

typedef struct {
    uint32_t id;
    const void * icon;
    helios_app_create_cb_t create_cb;
    helios_app_launch_mode_t launch_mode;
    helios_screen_transition_t transition;
    lv_event_cb_t event_cb;
    lv_scr_load_anim_t anim;
    char name[HELIOS_APP_NAME_MAX];
    char tag[HELIOS_APP_TAG_MAX];
} helios_app_t;

typedef enum {
    HELIOS_APPS_EVENT_ADDED,
    HELIOS_APPS_EVENT_CHANGED,
    HELIOS_APPS_EVENT_CLEARED,
} helios_apps_event_t;

typedef void (*helios_apps_observer_cb_t)(helios_apps_event_t event,
                                          const helios_app_t * app,
                                          void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void helios_apps_init(void);

uint32_t helios_apps_count(void);
const helios_app_t * helios_apps_get(uint32_t index);

bool helios_apps_register(const void * icon,
                          const char * name,
                          const char * tag,
                          helios_app_create_cb_t create_cb,
                          lv_scr_load_anim_t anim);

bool helios_apps_register_transition(const void * icon,
                                     const char * name,
                                     const char * tag,
                                     helios_app_create_cb_t create_cb,
                                     lv_scr_load_anim_t anim,
                                     helios_screen_transition_t transition);

bool helios_apps_register_simple(const void * icon,
                                 const char * name,
                                 const char * tag,
                                 helios_app_create_cb_t create_cb,
                                 lv_scr_load_anim_t anim);

bool helios_apps_register_simple_transition(const void * icon,
                                            const char * name,
                                            const char * tag,
                                            helios_app_create_cb_t create_cb,
                                            lv_scr_load_anim_t anim,
                                            helios_screen_transition_t transition);

bool helios_apps_register_simple_events(const void * icon,
                                        const char * name,
                                        const char * tag,
                                        helios_app_create_cb_t create_cb,
                                        lv_scr_load_anim_t anim,
                                        lv_event_cb_t event_cb);

bool helios_apps_register_simple_events_transition(const void * icon,
                                                   const char * name,
                                                   const char * tag,
                                                   helios_app_create_cb_t create_cb,
                                                   lv_scr_load_anim_t anim,
                                                   lv_event_cb_t event_cb,
                                                   helios_screen_transition_t transition);

bool helios_apps_initializer_add(helios_app_initializer_cb_t cb);
void helios_apps_run_initializers(void);

void helios_apps_clear(void);
void helios_apps_launch(const helios_app_t * app);

bool helios_apps_observer_add(helios_apps_observer_cb_t cb, void * user_data);
void helios_apps_observer_remove(helios_apps_observer_cb_t cb, void * user_data);

#define HELIOS_REGISTER_APP(icon, name, tag, create_cb, anim) \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_app_constructor_)(void) \
    { \
        helios_apps_initializer_add(HELIOS_UNIQUE_NAME(_helios_app_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void) \
    { \
        helios_apps_register((icon), (name), (tag), (create_cb), (anim)); \
    }

#define HELIOS_REGISTER_APP_TRANSITION(icon, name, tag, create_cb, anim, transition) \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_app_constructor_)(void) \
    { \
        helios_apps_initializer_add(HELIOS_UNIQUE_NAME(_helios_app_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void) \
    { \
        helios_apps_register_transition((icon), (name), (tag), (create_cb), (anim), (transition)); \
    }

#define HELIOS_REGISTER_SIMPLE_APP(icon, name, tag, create_cb, anim) \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_app_constructor_)(void) \
    { \
        helios_apps_initializer_add(HELIOS_UNIQUE_NAME(_helios_app_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void) \
    { \
        helios_apps_register_simple((icon), (name), (tag), (create_cb), (anim)); \
    }

#define HELIOS_REGISTER_SIMPLE_APP_TRANSITION(icon, name, tag, create_cb, anim, transition) \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_app_constructor_)(void) \
    { \
        helios_apps_initializer_add(HELIOS_UNIQUE_NAME(_helios_app_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void) \
    { \
        helios_apps_register_simple_transition((icon), (name), (tag), (create_cb), (anim), (transition)); \
    }

#define HELIOS_REGISTER_SIMPLE_APP_EVENTS(icon, name, tag, create_cb, anim, event_cb) \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_app_constructor_)(void) \
    { \
        helios_apps_initializer_add(HELIOS_UNIQUE_NAME(_helios_app_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void) \
    { \
        helios_apps_register_simple_events((icon), (name), (tag), (create_cb), (anim), (event_cb)); \
    }

#define HELIOS_REGISTER_SIMPLE_APP_EVENTS_TRANSITION(icon, name, tag, create_cb, anim, event_cb, transition) \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_app_constructor_)(void) \
    { \
        helios_apps_initializer_add(HELIOS_UNIQUE_NAME(_helios_app_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_app_register_cb_)(void) \
    { \
        helios_apps_register_simple_events_transition((icon), (name), (tag), (create_cb), (anim), (event_cb), (transition)); \
    }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_APP_MANAGER_H*/
