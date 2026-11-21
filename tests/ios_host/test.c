#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Use the real SDK profile layouts; replace only its RTOS/common layer. */
#define __BF0_BLE_COMMON_H
#define __PACKED_STRUCT struct __attribute__((packed))
#define BLE_ANCS_TYPE 0x90
#define BLE_AMS_TYPE 0xD0
#include "core/helios_ios.c"

static unsigned ancs_searches, ams_searches, ancs_writes, ams_writes;
static unsigned notifications, music_updates;
static unsigned call_updates;
static bool call_active;
static char caller[CHRONOS_TEXT_LARGE_SIZE];
static bool ancs_cccd, ams_cccd;
static uint8_t ams_result;
static chronos_notification_t received;
static chronos_music_info_t music_received;

uint8_t ble_ancs_enable(uint8_t idx) { assert(idx == 1); ++ancs_searches; return 0; }
uint8_t ble_ams_enable(uint8_t idx) { assert(idx == 1); ++ams_searches; return 0; }
uint8_t ble_ancs_cccd_enable(uint8_t on)
{
    if (g_ios.ancs_ready) { assert(on); ++ancs_writes; }
    ancs_cccd = on;
    return 0;
}
uint8_t ble_ams_cccd_enable(uint8_t on)
{
    /* SDK does nothing when on == is_cccd_on. */
    if (g_ios.ams_ready && on != ams_cccd) { assert(on); ++ams_writes; }
    ams_cccd = on;
    return ams_result;
}
void ble_ancs_category_mask_set(uint16_t mask) { (void)mask; }
void ble_ancs_attr_enable(uint8_t id, uint8_t on, uint16_t len)
{ (void)id; (void)on; (void)len; }
void ble_ams_player_attr_enable(ble_ams_entity_player_attribute_mask_t mask) { (void)mask; }
void ble_ams_track_attr_enable(ble_ams_entity_track_attribute_mask_t mask) { (void)mask; }
uint8_t ble_ams_send_command(ble_ams_cmd_t cmd) { (void)cmd; return 0; }
void helios_app_chronos_notification(const chronos_notification_t *value)
{ ++notifications; received = *value; }
void helios_app_music_update(const chronos_music_info_t *value)
{ ++music_updates; music_received = *value; }
void helios_app_call_update(const char *name, bool active)
{
    ++call_updates;
    call_active = active;
    snprintf(caller, sizeof(caller), "%s", name ? name : "");
}

static void event(uint16_t id, void *value, uint16_t len)
{ helios_ios_event_handler(id, value, len, 0); }
static void ancs_ready(uint8_t idx, uint8_t result)
{ ble_ancs_enable_rsp_t rsp = {idx, result}; event(BLE_ANCS_ENABLE_RSP, &rsp, sizeof(rsp)); }
static void ams_ready(uint8_t result)
{ ble_ams_enable_rsp_t rsp = {result}; event(BLE_AMS_ENABLE_RSP, &rsp, sizeof(rsp)); }
static void reset(void)
{
    helios_ios_on_disconnected(1);
    ancs_searches = ams_searches = ancs_writes = ams_writes = 0;
    ancs_cccd = ams_cccd = false;
    ams_result = 0;
}

int main(void)
{
    /* Discover before pairing; confirmation must not start discovery twice. */
    helios_ios_on_connected(1); helios_ios_on_connected(1);
    assert(ancs_searches == 1 && ams_searches == 0);
    ancs_ready(1, 0);
    assert(!ancs_cccd && !ams_cccd && !ancs_writes && !ams_writes);
    helios_ios_on_bonded(2); assert(!ancs_writes && !ams_writes);
    helios_ios_on_bonded(1); helios_ios_on_bonded(1);
    assert(ams_searches == 1);
    ams_ready(0);
    assert(ancs_writes == 1 && ams_writes == 1);
    helios_ios_on_connected(1); assert(ancs_writes == 1 && ams_writes == 1);

    /* Pairing/encryption can precede either profile response. */
    reset(); helios_ios_on_connected(1); helios_ios_on_bonded(1);
    helios_ios_on_bonded(1);
    assert(ancs_searches == 1 && ams_searches == 1);
    ancs_ready(2, 0); assert(!helios_ios_ancs_ready());
    ams_ready(0); assert(!ancs_writes && ams_writes == 1);
    ancs_ready(1, 0); assert(ancs_writes == 1);
    ancs_ready(1, 0); ams_ready(0); assert(ancs_writes == 1 && ams_writes == 1);

    /* ANCS may retry after pairing; AMS never searches before security. */
    reset(); helios_ios_on_connected(1);
    ancs_ready(1, BLE_ANCS_ERR_SEARCH_REMOTE_SERVICE_FAILED);
    helios_ios_on_bonded(1);
    assert(ancs_searches == 2 && ams_searches == 1);
    ancs_ready(1, 0); ams_result = BLE_AMS_ERR_CCCD_PENDING; ams_ready(0);
    helios_ios_on_bonded(1); assert(ancs_writes == 1 && ams_writes == 1);
    ble_ams_enable_pending_ind_t pending = {0};
    event(BLE_AMS_ENABLE_PENDING_IND, &pending, sizeof(pending));
    assert(ams_writes == 1);

    /* Malformed fixed events must not dereference short/null buffers. */
    const uint16_t fixed[] = {BLE_ANCS_ENABLE_RSP, BLE_AMS_ENABLE_RSP,
        BLE_AMS_ENABLE_PENDING_IND, BLE_AMS_SUPPORTED_CMD_NOTIFY_IND,
        BLE_ANCS_NOTIFICATION_IND, BLE_AMS_ENTITY_ATTRIBUTE_PAIR_IND};
    uint8_t short_data = 0;
    for (unsigned i = 0; i < sizeof(fixed) / sizeof(fixed[0]); ++i) {
        event(fixed[i], NULL, 0); event(fixed[i], &short_data, 0);
    }
    ble_ams_entity_attr_value_t truncated = {0};
    truncated.len = 200;
    event(BLE_AMS_ENTITY_ATTRIBUTE_PAIR_IND, &truncated, sizeof(truncated));
    assert(!music_updates);

    /* Valid AMS metadata still publishes, and ANCS packed attributes copy. */
    uint8_t storage[sizeof(ble_ams_entity_attr_value_t) + 5] = {0};
    ble_ams_entity_attr_value_t *track = (void *)storage;
    track->entity_id = BLE_AMS_ENTITY_ID_TRACK;
    track->attr_id = BLE_AMS_TRACK_ATTR_ID_TILTE;
    track->len = 5; memcpy(track->value, "Track", 5);
    event(BLE_AMS_ENTITY_ATTRIBUTE_PAIR_IND, track, sizeof(storage));
    assert(music_updates == 1 && !strcmp(music_received.title, "Track"));
    uint8_t attribute[sizeof(ble_ancs_attr_value_t) + 5] = {0};
    ble_ancs_attr_value_t *title = (void *)attribute;
    title->attr_id = BLE_ANCS_NOTIFICATION_ATTR_ID_TITLE;
    title->len = 5; memcpy(title->data, "Hello", 5);
    ble_ancs_noti_attr_t noti = {0};
    noti.evt_id = BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED;
    noti.attr_count = 1; noti.value = title;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(notifications == 1 && !strcmp(received.title, "Hello"));
    reset(); event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(notifications == 1 && !helios_ios_ancs_ready() && !helios_ios_ams_ready());
    helios_ios_on_connected(1); helios_ios_on_bonded(1);
    ancs_ready(1, 0); ams_ready(0);
    assert(ancs_writes == 1 && ams_writes == 1);

    noti.cate_id = BLE_ANCS_CATEGORY_ID_INCOMING_CALL;
    noti.noti_uid = 7;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 1 && call_active && !strcmp(caller, "Hello"));
    noti.evt_id = BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED;
    memcpy(title->data, "Alice", 5);
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 2 && call_active && !strcmp(caller, "Alice"));
    noti.evt_id = BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED;
    noti.noti_uid = 8;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 2 && call_active);
    noti.noti_uid = 7;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 3 && !call_active);
    noti.evt_id = BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 3 && !call_active);
    noti.noti_uid = 9;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 4 && call_active && notifications == 1);
    noti.cate_id = BLE_ANCS_CATEGORY_ID_MISSED_CALL;
    noti.noti_uid = 10;
    event(BLE_ANCS_NOTIFICATION_IND, &noti, sizeof(noti));
    assert(call_updates == 4 && call_active && notifications == 2);
    reset();
    assert(call_updates == 5 && !call_active);

    puts("iOS: pairing, reconnect, malformed events, metadata and incoming-call lifecycle passed");
    return 0;
}
