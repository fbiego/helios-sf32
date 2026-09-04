/**
 * @file watchface_manager.h
 * @brief Watchface registry API
 */

#ifndef HELIOS_WATCHFACE_MANAGER_H
#define HELIOS_WATCHFACE_MANAGER_H

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

/*********************
 *      DEFINES
 *********************/

#define HELIOS_WATCHFACES_MAX          16
#define HELIOS_WATCHFACE_PREVIEWS_MAX   8
#define HELIOS_WATCHFACE_NAME_MAX      32
#define HELIOS_WATCHFACE_TAG_MAX       32
#define HELIOS_WATCHFACE_INITIALIZERS_MAX 16

#ifndef HELIOS_CONSTRUCTOR_ATTR
    #if defined(__GNUC__) || defined(__clang__)
        #define HELIOS_CONSTRUCTOR_ATTR __attribute__((constructor))
    #else
        #define HELIOS_CONSTRUCTOR_ATTR
    #endif
#endif

#ifndef HELIOS_CONCAT
    #define HELIOS_CONCAT_INNER(a, b) a##b
    #define HELIOS_CONCAT(a, b) HELIOS_CONCAT_INNER(a, b)
#endif

#ifndef HELIOS_UNIQUE_NAME
    #define HELIOS_UNIQUE_NAME(prefix) HELIOS_CONCAT(prefix, __LINE__)
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_obj_t * (*helios_watchface_create_cb_t)(lv_obj_t * parent);
typedef void (*helios_watchface_initializer_cb_t)(void);

typedef struct {
    int32_t width;
    int32_t height;
    const void * image;
} helios_watchface_preview_t;

typedef struct {
    uint32_t id;
    helios_watchface_create_cb_t create_cb;
    uint32_t preview_count;
    helios_watchface_preview_t previews[HELIOS_WATCHFACE_PREVIEWS_MAX];
    char name[HELIOS_WATCHFACE_NAME_MAX];
    char tag[HELIOS_WATCHFACE_TAG_MAX];
} helios_watchface_t;

typedef enum {
    HELIOS_WATCHFACES_EVENT_ADDED,
    HELIOS_WATCHFACES_EVENT_CHANGED,
    HELIOS_WATCHFACES_EVENT_CLEARED,
} helios_watchfaces_event_t;

typedef void (*helios_watchfaces_observer_cb_t)(helios_watchfaces_event_t event,
                                                const helios_watchface_t * watchface,
                                                void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const void * img_preview_240;
extern const void * img_preview_280;
extern const void * img_preview_360;
extern const void * img_preview_390;
extern const void * img_preview_410;
extern const void * img_preview_466;

void helios_watchfaces_init(void);
void helios_watchfaces_init_all(void);

uint32_t helios_watchfaces_count(void);
const helios_watchface_t * helios_watchfaces_get(uint32_t index);

bool helios_watchfaces_register(const char * name,
                                const char * tag,
                                helios_watchface_create_cb_t create_cb,
                                const void * preview);

bool helios_watchfaces_register_previews(const char * name,
                                         const char * tag,
                                         helios_watchface_create_cb_t create_cb,
                                         const helios_watchface_preview_t * previews,
                                         uint32_t preview_count);

bool helios_watchfaces_register_preview_res(const char * name,
                                            const char * tag,
                                            helios_watchface_create_cb_t create_cb,
                                            const void * preview,
                                            int32_t preview_width,
                                            int32_t preview_height);

bool helios_watchfaces_register_full(const char * name,
                                     const char * tag,
                                     helios_watchface_create_cb_t create_cb,
                                     const helios_watchface_preview_t * previews,
                                     uint32_t preview_count);

bool helios_watchfaces_set_previews(const char * tag,
                                    const helios_watchface_preview_t * previews,
                                    uint32_t preview_count);

bool helios_watchfaces_initializer_add(helios_watchface_initializer_cb_t cb);
void helios_watchfaces_run_initializers(void);

void helios_watchfaces_clear(void);

uint32_t helios_watchfaces_active_index(void);
const helios_watchface_t * helios_watchfaces_active(void);
bool helios_watchfaces_set_active(uint32_t index);

const void * helios_watchface_get_preview(const helios_watchface_t * watchface);
lv_obj_t * helios_watchface_create_active(lv_obj_t * parent);

bool helios_watchfaces_observer_add(helios_watchfaces_observer_cb_t cb, void * user_data);
void helios_watchfaces_observer_remove(helios_watchfaces_observer_cb_t cb, void * user_data);

#define HELIOS_REGISTER_WATCHFACE_INITIALIZER(init_cb) \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_watchface_initializer_constructor_)(void) \
    { \
        helios_watchfaces_initializer_add((init_cb)); \
    }

#define HELIOS_REGISTER_WATCHFACE(name, tag, create_cb, preview) \
    static void HELIOS_UNIQUE_NAME(_helios_watchface_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_watchface_constructor_)(void) \
    { \
        helios_watchfaces_initializer_add(HELIOS_UNIQUE_NAME(_helios_watchface_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_watchface_register_cb_)(void) \
    { \
        helios_watchfaces_register((name), (tag), (create_cb), (preview)); \
    }

#define HELIOS_REGISTER_WATCHFACE_PREVIEW_RES(name, tag, create_cb, preview, preview_width, preview_height) \
    static void HELIOS_UNIQUE_NAME(_helios_watchface_register_cb_)(void); \
    static void HELIOS_CONSTRUCTOR_ATTR HELIOS_UNIQUE_NAME(_helios_watchface_constructor_)(void) \
    { \
        helios_watchfaces_initializer_add(HELIOS_UNIQUE_NAME(_helios_watchface_register_cb_)); \
    } \
    static void HELIOS_UNIQUE_NAME(_helios_watchface_register_cb_)(void) \
    { \
        helios_watchfaces_register_preview_res((name), (tag), (create_cb), (preview), (preview_width), (preview_height)); \
    }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_WATCHFACE_MANAGER_H*/
