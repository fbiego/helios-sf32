/**
 * @file contacts.c
 * @brief Contacts data API
 */

/*********************
 *      INCLUDES
 *********************/

#include "contacts.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_CONTACTS_OBSERVER_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    helios_contacts_observer_cb_t cb;
    void * user_data;
} helios_contacts_observer_t;

/***********************
 *  STATIC VARIABLES
 **********************/

static helios_contact_t contacts[HELIOS_CONTACTS_MAX];
static helios_contacts_observer_t observers[HELIOS_CONTACTS_OBSERVER_MAX];
static uint32_t contact_count;
static uint32_t next_contact_id = 1;
static bool inited;

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void notify_observers(helios_contacts_event_t event, const helios_contact_t * contact);
static void copy_text(char * dst, uint32_t dst_size, const char * src);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_contacts_init(void)
{
    if (inited) return;
    inited = true;

    helios_contacts_add("John Doe", "03244875324");
    helios_contacts_add("Jane Roe", "03244875325");
    helios_contacts_add("Support", "100");
}

uint32_t helios_contacts_count(void)
{
    return contact_count;
}

const helios_contact_t * helios_contacts_get(uint32_t index)
{
    if (index >= contact_count) return NULL;
    return &contacts[index];
}

bool helios_contacts_add(const char * name, const char * number)
{
    lv_lock();

    if (contact_count >= HELIOS_CONTACTS_MAX || !name) {
        lv_unlock();
        return false;
    }

    helios_contact_t * contact = &contacts[contact_count++];
    contact->id = next_contact_id++;
    copy_text(contact->name, sizeof(contact->name), name);
    copy_text(contact->number, sizeof(contact->number), number);

    notify_observers(HELIOS_CONTACTS_EVENT_ADDED, contact);
    lv_unlock();
    return true;
}

void helios_contacts_clear(void)
{
    lv_lock();
    contact_count = 0;
    notify_observers(HELIOS_CONTACTS_EVENT_CLEARED, NULL);
    lv_unlock();
}

bool helios_contacts_observer_add(helios_contacts_observer_cb_t cb, void * user_data)
{
    lv_lock();

    if (!cb) {
        lv_unlock();
        return false;
    }

    for (uint32_t i = 0; i < HELIOS_CONTACTS_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            lv_unlock();
            return true;
        }
    }

    for (uint32_t i = 0; i < HELIOS_CONTACTS_OBSERVER_MAX; i++) {
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

void helios_contacts_observer_remove(helios_contacts_observer_cb_t cb, void * user_data)
{
    lv_lock();
    for (uint32_t i = 0; i < HELIOS_CONTACTS_OBSERVER_MAX; i++) {
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

static void notify_observers(helios_contacts_event_t event, const helios_contact_t * contact)
{
    for (uint32_t i = 0; i < HELIOS_CONTACTS_OBSERVER_MAX; i++) {
        if (observers[i].cb) {
            observers[i].cb(event, contact, observers[i].user_data);
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
