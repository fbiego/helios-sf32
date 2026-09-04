/**
 * @file notifications.h
 * @brief Notification data API
 */

#ifndef HELIOS_NOTIFICATIONS_H
#define HELIOS_NOTIFICATIONS_H

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

#define HELIOS_NOTIFICATIONS_MAX           16
#define HELIOS_NOTIFICATION_TITLE_MAX      64
#define HELIOS_NOTIFICATION_TIME_MAX       12
#define HELIOS_NOTIFICATION_MESSAGE_MAX    512

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t id;
    const void * icon;
    char title[HELIOS_NOTIFICATION_TITLE_MAX];
    char time[HELIOS_NOTIFICATION_TIME_MAX];
    char message[HELIOS_NOTIFICATION_MESSAGE_MAX];
    bool read;
} helios_notification_t;

typedef enum {
    HELIOS_NOTIFICATIONS_EVENT_ADDED,
    HELIOS_NOTIFICATIONS_EVENT_CHANGED,
    HELIOS_NOTIFICATIONS_EVENT_CLEARED,
} helios_notifications_event_t;

typedef void (*helios_notifications_observer_cb_t)(helios_notifications_event_t event,
                                                   const helios_notification_t * notification,
                                                   void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void helios_notifications_init(void);

uint32_t helios_notifications_count(void);
const helios_notification_t * helios_notifications_get(uint32_t index);

bool helios_notifications_add(const void * icon,
                              const char * title,
                              const char * time,
                              const char * message);

void helios_notifications_clear(void);

bool helios_notifications_observer_add(helios_notifications_observer_cb_t cb, void * user_data);
void helios_notifications_observer_remove(helios_notifications_observer_cb_t cb, void * user_data);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_NOTIFICATIONS_H*/
