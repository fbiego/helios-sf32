/**
 * @file watchface_manager.c
 * @brief Watchface registry API
 */

/*********************
 *      INCLUDES
 *********************/

#include "watchface_manager.h"

#include "../../helios_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_WATCHFACES_OBSERVER_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    helios_watchfaces_observer_cb_t cb;
    void * user_data;
} helios_watchfaces_observer_t;

/***********************
 *  STATIC VARIABLES
 **********************/

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) const void * img_preview_240 = NULL;
__attribute__((weak)) const void * img_preview_280 = NULL;
__attribute__((weak)) const void * img_preview_360 = NULL;
__attribute__((weak)) const void * img_preview_390 = NULL;
__attribute__((weak)) const void * img_preview_410 = NULL;
__attribute__((weak)) const void * img_preview_466 = NULL;
#else
const void * img_preview_240 = NULL;
const void * img_preview_280 = NULL;
const void * img_preview_360 = NULL;
const void * img_preview_390 = NULL;
const void * img_preview_410 = NULL;
const void * img_preview_466 = NULL;
#endif

static helios_watchface_t watchfaces[HELIOS_WATCHFACES_MAX];
static helios_watchfaces_observer_t observers[HELIOS_WATCHFACES_OBSERVER_MAX];
static helios_watchface_initializer_cb_t initializers[HELIOS_WATCHFACE_INITIALIZERS_MAX];
static uint32_t watchface_count;
static uint32_t initializer_count;
static uint32_t active_index;
static uint32_t next_watchface_id = 1;
static bool inited;
static bool initializers_ran;

/***********************
 *  STATIC PROTOTYPES
 **********************/

static lv_obj_t * default_watchface_create(lv_obj_t * parent);
static void notify_observers(helios_watchfaces_event_t event, const helios_watchface_t * watchface);
static helios_watchface_t * find_watchface_by_tag(const char * tag);
static void watchface_set_previews(helios_watchface_t * watchface,
                                   const helios_watchface_preview_t * previews,
                                   uint32_t preview_count);
static void copy_text(char * dst, uint32_t dst_size, const char * src);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_watchfaces_init(void)
{
    if (inited) return;
    inited = true;
}

void helios_watchfaces_init_all(void)
{
    static bool builtins_registered = false;

    helios_watchfaces_init();
    if (builtins_registered) return;
    builtins_registered = true;

    helios_watchfaces_register("Default", "default", default_watchface_create, NULL);

    helios_watchfaces_run_initializers();
}

uint32_t helios_watchfaces_count(void)
{
    return watchface_count;
}

const helios_watchface_t * helios_watchfaces_get(uint32_t index)
{
    if (index >= watchface_count) return NULL;
    return &watchfaces[index];
}

bool helios_watchfaces_register(const char * name,
                                const char * tag,
                                helios_watchface_create_cb_t create_cb,
                                const void * preview)
{
    return helios_watchfaces_register_preview_res(name,
                                                 tag,
                                                 create_cb,
                                                 preview,
                                                 lv_subject_get_int(&sb_screen_width),
                                                 lv_subject_get_int(&sb_screen_height));
}

bool helios_watchfaces_register_previews(const char * name,
                                         const char * tag,
                                         helios_watchface_create_cb_t create_cb,
                                         const helios_watchface_preview_t * previews,
                                         uint32_t preview_count)
{
    return helios_watchfaces_register_full(name, tag, create_cb, previews, preview_count);
}

bool helios_watchfaces_register_preview_res(const char * name,
                                            const char * tag,
                                            helios_watchface_create_cb_t create_cb,
                                            const void * preview,
                                            int32_t preview_width,
                                            int32_t preview_height)
{
    const helios_watchface_preview_t previews[] = {
        { preview_width, preview_height, preview },
    };

    return helios_watchfaces_register_full(name, tag, create_cb, previews, 1);
}

bool helios_watchfaces_register_full(const char * name,
                                     const char * tag,
                                     helios_watchface_create_cb_t create_cb,
                                     const helios_watchface_preview_t * previews,
                                     uint32_t preview_count)
{
    lv_lock();

    if (watchface_count >= HELIOS_WATCHFACES_MAX || !name || !create_cb) {
        lv_unlock();
        return false;
    }

    helios_watchface_t * watchface = &watchfaces[watchface_count++];
    watchface->id = next_watchface_id++;
    watchface->create_cb = create_cb;
    copy_text(watchface->name, sizeof(watchface->name), name);
    copy_text(watchface->tag, sizeof(watchface->tag), tag);
    watchface_set_previews(watchface, previews, preview_count);

    notify_observers(HELIOS_WATCHFACES_EVENT_ADDED, watchface);
    lv_unlock();
    return true;
}

bool helios_watchfaces_set_previews(const char * tag,
                                    const helios_watchface_preview_t * previews,
                                    uint32_t preview_count)
{
    lv_lock();

    helios_watchface_t * watchface = find_watchface_by_tag(tag);
    if (!watchface) {
        lv_unlock();
        return false;
    }

    watchface_set_previews(watchface, previews, preview_count);
    notify_observers(HELIOS_WATCHFACES_EVENT_CHANGED, watchface);
    lv_unlock();
    return true;
}

bool helios_watchfaces_initializer_add(helios_watchface_initializer_cb_t cb)
{
    if (!cb) return false;

    for (uint32_t i = 0; i < initializer_count; i++) {
        if (initializers[i] == cb) return true;
    }

    if (initializer_count >= HELIOS_WATCHFACE_INITIALIZERS_MAX) return false;

    initializers[initializer_count++] = cb;
    return true;
}

void helios_watchfaces_run_initializers(void)
{
    if (initializers_ran) return;
    initializers_ran = true;

    for (uint32_t i = 0; i < initializer_count; i++) {
        if (initializers[i]) initializers[i]();
    }
}

void helios_watchfaces_clear(void)
{
    lv_lock();
    watchface_count = 0;
    active_index = 0;
    notify_observers(HELIOS_WATCHFACES_EVENT_CLEARED, NULL);
    lv_unlock();
}

uint32_t helios_watchfaces_active_index(void)
{
    return active_index;
}

const helios_watchface_t * helios_watchfaces_active(void)
{
    if (watchface_count == 0) return NULL;
    if (active_index >= watchface_count) active_index = 0;
    return &watchfaces[active_index];
}

bool helios_watchfaces_set_active(uint32_t index)
{
    lv_lock();

    if (index >= watchface_count) {
        lv_unlock();
        return false;
    }

    active_index = index;
    notify_observers(HELIOS_WATCHFACES_EVENT_CHANGED, &watchfaces[index]);
    lv_unlock();
    return true;
}

const void * helios_watchface_get_preview(const helios_watchface_t * watchface)
{
    if (!watchface || watchface->preview_count == 0) return NULL;

    int32_t width = lv_subject_get_int(&sb_screen_width);
    int32_t height = lv_subject_get_int(&sb_screen_height);
    const void * width_match = NULL;

    for (uint32_t i = 0; i < watchface->preview_count; i++) {
        const helios_watchface_preview_t * preview = &watchface->previews[i];
        if (!preview->image) continue;

        if (preview->width == width && preview->height == height) {
            return preview->image;
        }

        if (!width_match && preview->width == width) {
            width_match = preview->image;
        }
    }

    return width_match ? width_match : watchface->previews[0].image;
}

lv_obj_t * helios_watchface_create_active(lv_obj_t * parent)
{
    const helios_watchface_t * watchface = helios_watchfaces_active();
    if (!watchface || !watchface->create_cb) return NULL;

    return watchface->create_cb(parent);
}

bool helios_watchfaces_observer_add(helios_watchfaces_observer_cb_t cb, void * user_data)
{
    lv_lock();

    if (!cb) {
        lv_unlock();
        return false;
    }

    for (uint32_t i = 0; i < HELIOS_WATCHFACES_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            lv_unlock();
            return true;
        }
    }

    for (uint32_t i = 0; i < HELIOS_WATCHFACES_OBSERVER_MAX; i++) {
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

void helios_watchfaces_observer_remove(helios_watchfaces_observer_cb_t cb, void * user_data)
{
    lv_lock();
    for (uint32_t i = 0; i < HELIOS_WATCHFACES_OBSERVER_MAX; i++) {
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

static lv_obj_t * default_watchface_create(lv_obj_t * parent)
{
    return watchface_default_create(parent);
}

static void notify_observers(helios_watchfaces_event_t event, const helios_watchface_t * watchface)
{
    for (uint32_t i = 0; i < HELIOS_WATCHFACES_OBSERVER_MAX; i++) {
        if (observers[i].cb) {
            observers[i].cb(event, watchface, observers[i].user_data);
        }
    }
}

static helios_watchface_t * find_watchface_by_tag(const char * tag)
{
    if (!tag) return NULL;

    for (uint32_t i = 0; i < watchface_count; i++) {
        if (lv_strcmp(watchfaces[i].tag, tag) == 0) {
            return &watchfaces[i];
        }
    }

    return NULL;
}

static void watchface_set_previews(helios_watchface_t * watchface,
                                   const helios_watchface_preview_t * previews,
                                   uint32_t preview_count)
{
    if (!watchface) return;

    if (!previews) preview_count = 0;
    watchface->preview_count = LV_MIN(preview_count, HELIOS_WATCHFACE_PREVIEWS_MAX);

    for (uint32_t i = 0; i < watchface->preview_count; i++) {
        watchface->previews[i] = previews[i];
    }
}

static void copy_text(char * dst, uint32_t dst_size, const char * src)
{
    if (!dst || dst_size == 0) return;

    if (!src) src = "";
    lv_strncpy(dst, src, dst_size);
}
