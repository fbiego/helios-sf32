/**
 * @file mpesa.c
 * @brief M-Pesa app data and deep-link API
 */

/*********************
 *      INCLUDES
 *********************/

#include "mpesa.h"
#include "../../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_MPESA_OBSERVER_MAX 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    helios_mpesa_observer_cb_t cb;
    void * user_data;
} helios_mpesa_observer_t;

/***********************
 *  STATIC VARIABLES
 **********************/

static helios_mpesa_transaction_t transactions[HELIOS_MPESA_TRANSACTIONS_MAX];
static helios_mpesa_observer_t observers[HELIOS_MPESA_OBSERVER_MAX];
static uint32_t transaction_count;
static uint32_t next_transaction_id = 1;
static bool inited;

static lv_obj_t * transaction_list;
static lv_obj_t * active_details_screen;
static lv_obj_t * direct_return_screen;
static lv_timer_t * details_timeout_timer;

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void notify_observers(helios_mpesa_event_t event, const helios_mpesa_transaction_t * transaction);
static void screen_observer_cb(helios_mpesa_event_t event,
                               const helios_mpesa_transaction_t * transaction,
                               void * user_data);
static void render_all(lv_obj_t * list);
static void render_one(lv_obj_t * list, const helios_mpesa_transaction_t * transaction);
static void transaction_clicked_cb(lv_event_t * e);
static void open_details(const helios_mpesa_transaction_t * transaction,
                         uint32_t timeout_ms);
static void details_events_cb(lv_event_t * e);
static void details_timeout_cb(lv_timer_t * timer);
static void details_close(void);
static void details_cancel_timeout(void);
static void copy_text(char * dst, uint32_t dst_size, const char * src);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_mpesa_init(void)
{
    if (inited) return;
    inited = true;

    helios_mpesa_add("KCB", "4:23 PM • 3/9/2026", "3,500", "KCB Bank", "QJ12AB34CD", true, "0");
    helios_mpesa_add("Kenya Power", "1:08 PM • 3/9/2026", "1,200", "335489632", "QJ12AB34CD", false, "KSH 10");
    helios_mpesa_add("Airtime", "7:12 PM • 3/9/2026", "50", "Safaricom Airtime", "QJ12EF56GH", false, "0");
    helios_mpesa_add("John Doe", "7:28 PM • 3/9/2026", "5,120", "0722000000", "QJ12IJ78KL", true, "0");
}

lv_obj_t * helios_mpesa_screen_create(void)
{
    return sc_mpesa_create();
}

void helios_mpesa_screen_events_cb(lv_event_t * e)
{
    lv_obj_t * screen = lv_event_get_current_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOAD_START) {
        transaction_list = lv_obj_find_by_name(screen, "transaction_list");
        render_all(transaction_list);
        helios_mpesa_observer_add(screen_observer_cb, NULL);
    }

    if (code == LV_EVENT_DELETE) {
        helios_mpesa_observer_remove(screen_observer_cb, NULL);
        transaction_list = NULL;
    }
}

uint32_t helios_mpesa_count(void)
{
    return transaction_count;
}

const helios_mpesa_transaction_t * helios_mpesa_get(uint32_t index)
{
    if (index >= transaction_count) return NULL;
    return &transactions[index];
}

const helios_mpesa_transaction_t * helios_mpesa_find(uint32_t id)
{
    for (uint32_t i = 0; i < transaction_count; i++) {
        if (transactions[i].id == id) return &transactions[i];
    }

    return NULL;
}

bool helios_mpesa_add(const char * name,
                      const char * time,
                      const char * amount,
                      const char * account,
                      const char * tx_id,
                      bool tx_in,
                      const char * fee)
{
    lv_lock();

    if (transaction_count >= HELIOS_MPESA_TRANSACTIONS_MAX) {
        for (uint32_t i = 1; i < HELIOS_MPESA_TRANSACTIONS_MAX; i++) {
            transactions[i - 1] = transactions[i];
        }
        transaction_count = HELIOS_MPESA_TRANSACTIONS_MAX - 1;
    }

    helios_mpesa_transaction_t * transaction = &transactions[transaction_count++];
    transaction->id = next_transaction_id++;
    transaction->tx_in = tx_in;
    copy_text(transaction->name, sizeof(transaction->name), name);
    copy_text(transaction->time, sizeof(transaction->time), time);
    copy_text(transaction->amount, sizeof(transaction->amount), amount);
    copy_text(transaction->account, sizeof(transaction->account), account);
    copy_text(transaction->tx_id, sizeof(transaction->tx_id), tx_id);
    copy_text(transaction->fee, sizeof(transaction->fee), fee);

    notify_observers(HELIOS_MPESA_EVENT_ADDED, transaction);
    lv_unlock();
    return true;
}

bool helios_mpesa_add_and_open(const char * name,
                               const char * time,
                               const char * amount,
                               const char * account,
                               const char * tx_id,
                               bool tx_in,
                               const char * fee,
                               uint32_t timeout_ms)
{
    bool added = helios_mpesa_add(name, time, amount, account, tx_id, tx_in, fee);
    if (!added) return false;

    return helios_mpesa_open_latest(timeout_ms);
}

void helios_mpesa_clear(void)
{
    lv_lock();
    transaction_count = 0;
    notify_observers(HELIOS_MPESA_EVENT_CLEARED, NULL);
    lv_unlock();
}

void helios_mpesa_set_balance(const char * balance)
{
    helios_subject_set_mpesa_balance_text(balance);
}

bool helios_mpesa_open_transaction(uint32_t id, uint32_t timeout_ms)
{
    lv_lock();

    const helios_mpesa_transaction_t * transaction = helios_mpesa_find(id);
    if (!transaction) {
        lv_unlock();
        return false;
    }

    open_details(transaction, timeout_ms);
    lv_unlock();
    return true;
}

bool helios_mpesa_open_latest(uint32_t timeout_ms)
{
    lv_lock();

    if (transaction_count == 0) {
        lv_unlock();
        return false;
    }

    open_details(&transactions[transaction_count - 1], timeout_ms);
    lv_unlock();
    return true;
}

bool helios_mpesa_observer_add(helios_mpesa_observer_cb_t cb, void * user_data)
{
    lv_lock();

    if (!cb) {
        lv_unlock();
        return false;
    }

    for (uint32_t i = 0; i < HELIOS_MPESA_OBSERVER_MAX; i++) {
        if (observers[i].cb == cb && observers[i].user_data == user_data) {
            lv_unlock();
            return true;
        }
    }

    for (uint32_t i = 0; i < HELIOS_MPESA_OBSERVER_MAX; i++) {
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

void helios_mpesa_observer_remove(helios_mpesa_observer_cb_t cb, void * user_data)
{
    lv_lock();
    for (uint32_t i = 0; i < HELIOS_MPESA_OBSERVER_MAX; i++) {
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

static void notify_observers(helios_mpesa_event_t event, const helios_mpesa_transaction_t * transaction)
{
    for (uint32_t i = 0; i < HELIOS_MPESA_OBSERVER_MAX; i++) {
        if (observers[i].cb) {
            observers[i].cb(event, transaction, observers[i].user_data);
        }
    }
}

static void screen_observer_cb(helios_mpesa_event_t event,
                               const helios_mpesa_transaction_t * transaction,
                               void * user_data)
{
    LV_UNUSED(event);
    LV_UNUSED(transaction);
    LV_UNUSED(user_data);

    render_all(transaction_list);
}

static void render_all(lv_obj_t * list)
{
    if (!list) return;

    int32_t scroll_y = lv_obj_get_scroll_y(list);
    lv_obj_clean(list);

    for (uint32_t i = 0; i < transaction_count; i++) {
        render_one(list, &transactions[i]);
    }

    lv_obj_scroll_to_y(list, scroll_y, LV_ANIM_OFF);
}

static void render_one(lv_obj_t * list, const helios_mpesa_transaction_t * transaction)
{
    if (!list || !transaction) return;

    lv_obj_t * item = mpesa_item_create(list,
                                        transaction->name,
                                        transaction->time,
                                        transaction->amount,
                                        transaction->tx_in);
    if (item) {
        lv_obj_add_event_cb(item, transaction_clicked_cb, LV_EVENT_CLICKED, (void *)transaction);
    }
}

static void transaction_clicked_cb(lv_event_t * e)
{
    const helios_mpesa_transaction_t * transaction = lv_event_get_user_data(e);
    open_details(transaction, 0);
}

static void open_details(const helios_mpesa_transaction_t * transaction,
                         uint32_t timeout_ms)
{
    if (!transaction) return;

    details_cancel_timeout();

    lv_obj_t * current = lv_screen_active();
    bool replacing_details = current == active_details_screen;
    if (replacing_details && direct_return_screen && lv_obj_is_valid(direct_return_screen)) {
        current = direct_return_screen;
    }

    lv_obj_t * screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(screen, 255, 0);
    lv_obj_add_flag(screen, LV_OBJ_FLAG_CLICKABLE);

    mpesa_details_create(screen,
                         transaction->name,
                         transaction->time,
                         transaction->amount,
                         transaction->account,
                         transaction->tx_id,
                         transaction->tx_in,
                         transaction->fee);

    active_details_screen = screen;
    direct_return_screen = current;

    lv_obj_add_event_cb(screen, details_events_cb, LV_EVENT_ALL, NULL);

    if (timeout_ms > 0) {
        details_timeout_timer = lv_timer_create(details_timeout_cb, timeout_ms, screen);
        lv_timer_set_repeat_count(details_timeout_timer, 1);
    }

    helios_screen_load_transition(screen,
                                  LV_SCR_LOAD_ANIM_OVER_LEFT,
                                  HELIOS_SCREEN_TRANSITION_TIME,
                                  replacing_details,
                                  HELIOS_SCREEN_TRANSITION_APP_OPEN_LEFT);
}

static void details_events_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DELETE) {
        if (lv_event_get_current_target(e) == active_details_screen) {
            details_cancel_timeout();
            active_details_screen = NULL;
            direct_return_screen = NULL;
        }
        return;
    }

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_CLICKED || code == LV_EVENT_SCROLL_BEGIN) {
        details_cancel_timeout();
        return;
    }

    if (code == LV_EVENT_GESTURE) {
        details_cancel_timeout();

        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        if (dir == LV_DIR_RIGHT) details_close();
    }
}

static void details_timeout_cb(lv_timer_t * timer)
{
    if (timer == details_timeout_timer) details_timeout_timer = NULL;
    details_close();
}

static void details_close(void)
{
    if (!active_details_screen || lv_screen_active() != active_details_screen) return;

    lv_obj_t * next_screen = NULL;
    if (direct_return_screen &&
        lv_obj_is_valid(direct_return_screen)) {
        next_screen = direct_return_screen;
    } else {
        next_screen = helios_mpesa_screen_create();
    }

    helios_screen_load_transition(next_screen,
                                  LV_SCR_LOAD_ANIM_OUT_RIGHT,
                                  HELIOS_SCREEN_TRANSITION_TIME,
                                  true,
                                  HELIOS_SCREEN_TRANSITION_APP_CLOSE_RIGHT);
}

static void details_cancel_timeout(void)
{
    if (!details_timeout_timer) return;

    lv_timer_delete(details_timeout_timer);
    details_timeout_timer = NULL;
}

static void copy_text(char * dst, uint32_t dst_size, const char * src)
{
    if (!dst || dst_size == 0) return;

    if (!src) src = "";
    lv_strncpy(dst, src, dst_size);
    dst[dst_size - 1] = '\0';
}
