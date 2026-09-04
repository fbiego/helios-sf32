/**
 * @file contacts.h
 * @brief Contacts data API
 */

#ifndef HELIOS_CONTACTS_H
#define HELIOS_CONTACTS_H

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

#define HELIOS_CONTACTS_MAX        64
#define HELIOS_CONTACT_NAME_MAX    48
#define HELIOS_CONTACT_NUMBER_MAX  24

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t id;
    char name[HELIOS_CONTACT_NAME_MAX];
    char number[HELIOS_CONTACT_NUMBER_MAX];
} helios_contact_t;

typedef enum {
    HELIOS_CONTACTS_EVENT_ADDED,
    HELIOS_CONTACTS_EVENT_CHANGED,
    HELIOS_CONTACTS_EVENT_CLEARED,
} helios_contacts_event_t;

typedef void (*helios_contacts_observer_cb_t)(helios_contacts_event_t event,
                                              const helios_contact_t * contact,
                                              void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void helios_contacts_init(void);

uint32_t helios_contacts_count(void);
const helios_contact_t * helios_contacts_get(uint32_t index);

bool helios_contacts_add(const char * name, const char * number);
void helios_contacts_clear(void);

bool helios_contacts_observer_add(helios_contacts_observer_cb_t cb, void * user_data);
void helios_contacts_observer_remove(helios_contacts_observer_cb_t cb, void * user_data);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_CONTACTS_H*/
