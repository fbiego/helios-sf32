/**
 * @file notifications.c
 * @brief Notification data API
 */

/*********************
 *      INCLUDES
 *********************/

#include "notifications.h"
#include "../../../helios_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_NOTIFICATIONS_OBSERVER_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    helios_notifications_observer_cb_t cb;
    void * user_data;
} helios_notifications_observer_t;

/***********************
 *  STATIC VARIABLES
 **********************/

static helios_notification_t notifications[HELIOS_NOTIFICATIONS_MAX];
static helios_notifications_observer_t observers[HELIOS_NOTIFICATIONS_OBSERVER_MAX];
static uint32_t notification_count;
static uint32_t next_notification_id = 1;
static bool inited;

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void notify_observers(helios_notifications_event_t event, const helios_notification_t * notification);
static void copy_text(char * dst, uint32_t dst_size, const char * src);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_notifications_init(void)
{
    if (inited) return;
    inited = true;

    helios_notifications_add(icon_gear, "System", "15:12",
                             "Battery level has dropped below 20%. To avoid interruptions, connect your device to a charger or enable power saving mode from settings.");
    helios_notifications_add(icon_music, "Now playing", "15:27",
                             "Midnight Drive - Synthwave Mix.\nEnjoy uninterrupted music streaming with premium quality audio and no ads.");
    helios_notifications_add(icon_chat, "Twitter", "16:03",
                             "You have 5 new followers and 2 new mentions. Tap to see who interacted with your latest post and join the conversation.");
}

uint32_t helios_notifications_count(void)
{
    return notification_count;
}

const helios_notification_t * helios_notifications_get(uint32_t index)
{
    if (index >= notification_count) return NULL;
    return &notifications[index];
}

bool helios_notifications_add(const void * icon,
                              const char * title,
                              const char * time,
                              const char * message)
{
    lv_lock();

    if (notification_count >= HELIOS_NOTIFICATIONS_MAX) {
        for (uint32_t i = 1; i < HELIOS_NOTIFICATIONS_MAX; i++) {
            notifications[i - 1] = notifications[i];
        }
        notification_count = HELIOS_NOTIFICATIONS_MAX - 1;
    }

    helios_notification_t * notification = &notifications[notification_count++];
    notification->id = next_notification_id++;
    notification->icon = icon;
    notification->read = false;
    copy_text(notification->title, sizeof(notification->title), title);
    copy_text(notification->time, sizeof(notification->time), time);
    copy_text(notification->message, sizeof(notification->message), message);

    notify_observers(HELIOS_NOTIFICATIONS_EVENT_ADDED, notification);
    lv_unlock();
    return true;
}

void helios_notifications_clear(void)
{
    lv_lock();
    notification_count = 0;
    notify_observers(HELIOS_NOTIFICATIONS_EVENT_CLEARED, NULL);
    lv_unlock();
}

bool helios_notifications_observer_add(helios_notifications_observer_cb_t cb, void * user_data)
{
    lv_lock();

    if (!cb) {
        lv_unlock();
        return false;
    }

    for (uint32_t i = 0; i < HELIOS_NOTIFICATIONS_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            lv_unlock();
            return true;
        }
    }

    for (uint32_t i = 0; i < HELIOS_NOTIFICATIONS_OBSERVER_MAX; i++) {
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

void helios_notifications_observer_remove(helios_notifications_observer_cb_t cb, void * user_data)
{
    lv_lock();
    for (uint32_t i = 0; i < HELIOS_NOTIFICATIONS_OBSERVER_MAX; i++) {
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

static void notify_observers(helios_notifications_event_t event, const helios_notification_t * notification)
{
    for (uint32_t i = 0; i < HELIOS_NOTIFICATIONS_OBSERVER_MAX; i++) {
        if (observers[i].cb) {
            observers[i].cb(event, notification, observers[i].user_data);
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
