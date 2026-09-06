/**
 * @file mpesa.h
 * @brief M-Pesa app data and deep-link API
 */

#ifndef HELIOS_MPESA_H
#define HELIOS_MPESA_H

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

#define HELIOS_MPESA_TRANSACTIONS_MAX 16
#define HELIOS_MPESA_NAME_MAX         64
#define HELIOS_MPESA_TIME_MAX         32
#define HELIOS_MPESA_AMOUNT_MAX       24
#define HELIOS_MPESA_ACCOUNT_MAX      64
#define HELIOS_MPESA_TX_ID_MAX        24
#define HELIOS_MPESA_FEE_MAX          16
#define HELIOS_MPESA_DETAILS_TIMEOUT_MS 8000

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t id;
    char name[HELIOS_MPESA_NAME_MAX];
    char time[HELIOS_MPESA_TIME_MAX];
    char amount[HELIOS_MPESA_AMOUNT_MAX];
    char account[HELIOS_MPESA_ACCOUNT_MAX];
    char tx_id[HELIOS_MPESA_TX_ID_MAX];
    char fee[HELIOS_MPESA_FEE_MAX];
    bool tx_in;
} helios_mpesa_transaction_t;

typedef enum {
    HELIOS_MPESA_EVENT_ADDED,
    HELIOS_MPESA_EVENT_CHANGED,
    HELIOS_MPESA_EVENT_CLEARED,
} helios_mpesa_event_t;

typedef void (*helios_mpesa_observer_cb_t)(helios_mpesa_event_t event,
                                           const helios_mpesa_transaction_t * transaction,
                                           void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void helios_mpesa_init(void);

lv_obj_t * helios_mpesa_screen_create(void);
void helios_mpesa_screen_events_cb(lv_event_t * e);

uint32_t helios_mpesa_count(void);
const helios_mpesa_transaction_t * helios_mpesa_get(uint32_t index);
const helios_mpesa_transaction_t * helios_mpesa_find(uint32_t id);

bool helios_mpesa_add(const char * name,
                      const char * time,
                      const char * amount,
                      const char * account,
                      const char * tx_id,
                      bool tx_in,
                      const char * fee);

bool helios_mpesa_add_and_open(const char * name,
                               const char * time,
                               const char * amount,
                               const char * account,
                               const char * tx_id,
                               bool tx_in,
                               const char * fee,
                               uint32_t timeout_ms);

void helios_mpesa_clear(void);
void helios_mpesa_set_balance(const char * balance);

bool helios_mpesa_open_transaction(uint32_t id, uint32_t timeout_ms);
bool helios_mpesa_open_latest(uint32_t timeout_ms);

bool helios_mpesa_observer_add(helios_mpesa_observer_cb_t cb, void * user_data);
void helios_mpesa_observer_remove(helios_mpesa_observer_cb_t cb, void * user_data);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_MPESA_H*/
