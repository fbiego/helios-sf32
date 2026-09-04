/**
 * @file app_manager.c
 * @brief Application registry API
 */

/*********************
 *      INCLUDES
 *********************/

#include "app_manager.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_APPS_OBSERVER_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    helios_apps_observer_cb_t cb;
    void * user_data;
} helios_apps_observer_t;

/***********************
 *  STATIC VARIABLES
 **********************/

static helios_app_t apps[HELIOS_APPS_MAX];
static helios_apps_observer_t observers[HELIOS_APPS_OBSERVER_MAX];
static helios_app_initializer_cb_t initializers[HELIOS_APP_INITIALIZERS_MAX];
static uint32_t app_count;
static uint32_t initializer_count;
static uint32_t next_app_id = 1;
static bool inited;
static bool initializers_ran;

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void notify_observers(helios_apps_event_t event, const helios_app_t * app);
static bool app_should_precede(const helios_app_t * app, const helios_app_t * other);
static int32_t app_name_cmp(const char * a, const char * b);
static char app_ascii_tolower(char c);
static bool app_register(const void * icon,
                         const char * name,
                         const char * tag,
                         helios_app_create_cb_t create_cb,
                         helios_app_launch_mode_t launch_mode,
                         lv_scr_load_anim_t anim,
                         lv_event_cb_t event_cb,
                         helios_screen_transition_t transition);
static void copy_text(char * dst, uint32_t dst_size, const char * src);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_apps_init(void)
{
    if (inited) return;
    inited = true;
}

uint32_t helios_apps_count(void)
{
    return app_count;
}

const helios_app_t * helios_apps_get(uint32_t index)
{
    if (index >= app_count) return NULL;
    return &apps[index];
}

bool helios_apps_register(const void * icon,
                          const char * name,
                          const char * tag,
                          helios_app_create_cb_t create_cb,
                          lv_scr_load_anim_t anim)
{
    return helios_apps_register_transition(icon, name, tag, create_cb, anim, HELIOS_SCREEN_TRANSITION_DEFAULT);
}

bool helios_apps_register_transition(const void * icon,
                                     const char * name,
                                     const char * tag,
                                     helios_app_create_cb_t create_cb,
                                     lv_scr_load_anim_t anim,
                                     helios_screen_transition_t transition)
{
    return app_register(icon, name, tag, create_cb, HELIOS_APP_LAUNCH_DIRECT, anim, NULL, transition);
}

bool helios_apps_register_simple(const void * icon,
                                 const char * name,
                                 const char * tag,
                                 helios_app_create_cb_t create_cb,
                                 lv_scr_load_anim_t anim)
{
    return helios_apps_register_simple_transition(icon, name, tag, create_cb, anim, HELIOS_SCREEN_TRANSITION_DEFAULT);
}

bool helios_apps_register_simple_transition(const void * icon,
                                            const char * name,
                                            const char * tag,
                                            helios_app_create_cb_t create_cb,
                                            lv_scr_load_anim_t anim,
                                            helios_screen_transition_t transition)
{
    return app_register(icon, name, tag, create_cb, HELIOS_APP_LAUNCH_SIMPLE, anim, NULL, transition);
}

bool helios_apps_register_simple_events(const void * icon,
                                        const char * name,
                                        const char * tag,
                                        helios_app_create_cb_t create_cb,
                                        lv_scr_load_anim_t anim,
                                        lv_event_cb_t event_cb)
{
    return helios_apps_register_simple_events_transition(icon,
                                                        name,
                                                        tag,
                                                        create_cb,
                                                        anim,
                                                        event_cb,
                                                        HELIOS_SCREEN_TRANSITION_DEFAULT);
}

bool helios_apps_register_simple_events_transition(const void * icon,
                                                   const char * name,
                                                   const char * tag,
                                                   helios_app_create_cb_t create_cb,
                                                   lv_scr_load_anim_t anim,
                                                   lv_event_cb_t event_cb,
                                                   helios_screen_transition_t transition)
{
    return app_register(icon, name, tag, create_cb, HELIOS_APP_LAUNCH_SIMPLE, anim, event_cb, transition);
}

bool helios_apps_initializer_add(helios_app_initializer_cb_t cb)
{
    if (!cb) return false;

    for (uint32_t i = 0; i < initializer_count; i++) {
        if (initializers[i] == cb) return true;
    }

    if (initializer_count >= HELIOS_APP_INITIALIZERS_MAX) return false;

    initializers[initializer_count++] = cb;
    return true;
}

void helios_apps_run_initializers(void)
{
    if (initializers_ran) return;
    initializers_ran = true;

    for (uint32_t i = 0; i < initializer_count; i++) {
        if (initializers[i]) initializers[i]();
    }
}

void helios_apps_clear(void)
{
    lv_lock();
    app_count = 0;
    notify_observers(HELIOS_APPS_EVENT_CLEARED, NULL);
    lv_unlock();
}

void helios_apps_launch(const helios_app_t * app)
{
    if (!app || !app->create_cb) return;

    lv_obj_t * screen = NULL;
    if (app->launch_mode == HELIOS_APP_LAUNCH_SIMPLE) {
        screen = simple_app_screen_create_transition(app->create_cb,
                                                     app->transition == HELIOS_SCREEN_TRANSITION_DEFAULT
                                                         ? HELIOS_SCREEN_TRANSITION_DEFAULT
                                                         : HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT);
    } else {
        screen = app->create_cb();
    }

    if (!screen) return;

    if (app->event_cb) {
        lv_obj_add_event_cb(screen, app->event_cb, LV_EVENT_ALL, NULL);
    }

    helios_screen_load_transition(screen, app->anim, HELIOS_SCREEN_TRANSITION_TIME, true, app->transition);
}

bool helios_apps_observer_add(helios_apps_observer_cb_t cb, void * user_data)
{
    lv_lock();

    if (!cb) {
        lv_unlock();
        return false;
    }

    for (uint32_t i = 0; i < HELIOS_APPS_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            lv_unlock();
            return true;
        }
    }

    for (uint32_t i = 0; i < HELIOS_APPS_OBSERVER_MAX; i++) {
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

void helios_apps_observer_remove(helios_apps_observer_cb_t cb, void * user_data)
{
    lv_lock();
    for (uint32_t i = 0; i < HELIOS_APPS_OBSERVER_MAX; i++) {
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

static bool app_register(const void * icon,
                         const char * name,
                         const char * tag,
                         helios_app_create_cb_t create_cb,
                         helios_app_launch_mode_t launch_mode,
                         lv_scr_load_anim_t anim,
                         lv_event_cb_t event_cb,
                         helios_screen_transition_t transition)
{
    lv_lock();

    if (app_count >= HELIOS_APPS_MAX || !name) {
        lv_unlock();
        return false;
    }

    helios_app_t app;
    app.id = next_app_id++;
    app.icon = icon;
    app.create_cb = create_cb;
    app.launch_mode = launch_mode;
    app.transition = transition;
    app.event_cb = event_cb;
    app.anim = anim;
    copy_text(app.name, sizeof(app.name), name);
    copy_text(app.tag, sizeof(app.tag), tag);

    uint32_t insert_at = app_count;
    while (insert_at > 0 && app_should_precede(&app, &apps[insert_at - 1])) {
        apps[insert_at] = apps[insert_at - 1];
        insert_at--;
    }

    apps[insert_at] = app;
    app_count++;

    notify_observers(HELIOS_APPS_EVENT_ADDED, &apps[insert_at]);
    lv_unlock();
    return true;
}

static bool app_should_precede(const helios_app_t * app, const helios_app_t * other)
{
    int32_t cmp = app_name_cmp(app->name, other->name);
    if (cmp != 0) return cmp < 0;

    return app->id < other->id;
}

static int32_t app_name_cmp(const char * a, const char * b)
{
    if (!a) a = "";
    if (!b) b = "";

    while (*a && *b) {
        char ac = app_ascii_tolower(*a);
        char bc = app_ascii_tolower(*b);

        if (ac != bc) return (int32_t)(unsigned char)ac - (int32_t)(unsigned char)bc;

        a++;
        b++;
    }

    return (int32_t)(unsigned char)*a - (int32_t)(unsigned char)*b;
}

static char app_ascii_tolower(char c)
{
    if (c >= 'A' && c <= 'Z') return (char)(c - 'A' + 'a');
    return c;
}

static void notify_observers(helios_apps_event_t event, const helios_app_t * app)
{
    for (uint32_t i = 0; i < HELIOS_APPS_OBSERVER_MAX; i++) {
        if (observers[i].cb) {
            observers[i].cb(event, app, observers[i].user_data);
        }
    }
}

static void copy_text(char * dst, uint32_t dst_size, const char * src)
{
    if (!dst || dst_size == 0) return;

    if (!src) src = "";
    lv_strncpy(dst, src, dst_size);
    dst[dst_size - 1] = '\0';
}
