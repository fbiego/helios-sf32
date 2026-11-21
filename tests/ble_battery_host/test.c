#include <assert.h>
#include "core/helios_chronos.c"

static uint8_t connected, subscribed, last_percent, last_charging;
static unsigned reports, reads;
static unsigned activity_reports;
static unsigned reset_requests;
static unsigned notifications, call_updates;
static bool call_active;
static unsigned history_requests, history_reports, history_cancellations, history_tails;
static helios_steps_record_cb_t history_callback;
static void *history_user;
void helios_storage_request_steps(helios_steps_record_cb_t callback, void *user)
{
    history_callback = callback; history_user = user;
    if (callback) history_requests++; else history_cancellations++;
}
static uint32_t last_steps, last_calories;
static rt_err_t read_error;
static helios_battery_t sample = {42, 3678, 1};

uint8_t helios_ble_is_connected(void) { return connected; }
uint8_t helios_ble_can_notify(void) { return connected && subscribed; }
int helios_ble_send(const uint8_t *data, uint16_t len)
{
    assert(helios_ble_can_notify());
    if (len == 8 && data[4] == 0x91 && data[5] == 0x80) {
        last_charging = data[6];
        last_percent = data[7];
        reports++;
    }
    if (len == 17 && data[4] == 0x51 && data[5] == 0x08) {
        last_steps = ((uint32_t)data[6]<<16)|((uint32_t)data[7]<<8)|data[8];
        last_calories = ((uint32_t)data[9]<<16)|((uint32_t)data[10]<<8)|data[11];
        activity_reports++;
    }
    if (len == 20 && data[4] == 0x51 && data[5] == 0x20) {
        assert(data[6] == 26 && data[7] == 11 && data[8] == 2 && data[9] == 13);
        assert(data[10] == 0x01 && data[11] == 0x02 && data[12] == 0x03);
        assert(data[13] == 0 && data[14] == 0x01 && data[15] == 0x23);
        assert(data[16] == 0 && data[17] == 0 && data[18] == 0 && data[19] == 0);
        history_reports++;
    }
    if (len == 6 && data[0] == 0) {
        for (int i=1;i<6;i++) assert(data[i]==0);
        history_tails++;
    }
    return len;
}
rt_err_t helios_battery_read(helios_battery_t *battery)
{ reads++; *battery = sample; return read_error; }
void helios_app_chronos_init(void) {}
void helios_app_chronos_connection(bool value) { (void)value; }
void helios_app_chronos_notification(const chronos_notification_t *value) { (void)value; ++notifications; }
void helios_app_call_update(const char *caller, bool active)
{
    if (active)
        assert(strcmp(caller, "Alice") == 0);
    call_active = active;
    ++call_updates;
}
void helios_app_chronos_config(chronos_config_t config, uint32_t a, uint32_t b)
{
    if (config == CHRONOS_CONFIG_RESET) {
        assert(a == 0 && b == 0);
        reset_requests++;
    }
}
uint8_t helios_ios_music_command(const char *action) { (void)action; return 0; }
int set_date(unsigned int y, unsigned int m, unsigned int d) { (void)y; (void)m; (void)d; return 0; }
int set_time(unsigned int h, unsigned int m, unsigned int s) { (void)h; (void)m; (void)s; return 0; }

int main(void)
{
    helios_chronos_update_battery(50, 0);
    helios_chronos_update_activity(1000, 50);
    assert(activity_reports == 0);
    assert(reports == 0);
    connected = 1;
    helios_chronos_on_connected();
    /* Real NUS RX -> parser -> application reset request. No TX required. */
    uint8_t reset_packet[16];
    size_t reset_length = 0;
    assert(chronos_core_build_packet(0xAB, CHRONOS_PROTOCOL_STANDARD, 0x23, 0x80,
        NULL, 0, reset_packet, sizeof(reset_packet), &reset_length) == CHRONOS_STATUS_OK);
    helios_chronos_on_rx(reset_packet, (uint16_t)reset_length);
    assert(reset_requests == 1);
    uint8_t call_packet[16];
    size_t call_length = 0;
    uint8_t call_payload[] = {0x01, 0x02, 'A', 'l', 'i', 'c', 'e'};
    assert(chronos_core_build_packet(0xAB, CHRONOS_PROTOCOL_STANDARD, 0x72, 0x80,
        call_payload, sizeof(call_payload), call_packet, sizeof(call_packet), &call_length) == CHRONOS_STATUS_OK);
    helios_chronos_on_rx(call_packet, (uint16_t)call_length);
    assert(call_updates == 1 && call_active && !notifications);
    call_packet[6] = 0x02;
    helios_chronos_on_rx(call_packet, (uint16_t)call_length);
    assert(call_updates == 2 && !call_active && !notifications);
    helios_chronos_update_battery(50, 0);
    assert(reports == 0);
    subscribed = 1;
    helios_chronos_on_subscribed();
    uint8_t history_packet[16]; size_t history_length = 0;
    assert(chronos_core_build_packet(0xAB, CHRONOS_PROTOCOL_STANDARD, 0x51, 0x80,
        NULL, 0, history_packet, sizeof(history_packet), &history_length) == CHRONOS_STATUS_OK);
    helios_chronos_on_rx(history_packet, (uint16_t)history_length);
    assert(history_requests == 1 && history_reports == 0 && history_callback);
    helios_steps_record_t record = {.steps=0x010203, .calories=0x0123,
        .year=2026, .month=11, .day=2, .hour=13};
    assert(history_callback(&record, history_user) && history_reports == 1 && history_tails == 1);
    /* Other health requests must not start a steps export. */
    helios_chronos_health_request(CHRONOS_HEALTH_SLEEP_RECORDS, true);
    helios_chronos_health_request(CHRONOS_HEALTH_STEPS_RECORDS, false);
    assert(history_requests == 1);
    helios_chronos_update_activity(12345, 500);
    assert(activity_reports == 1 && last_steps == 12345 && last_calories == 500);
    assert(reports == 1 && reads == 1 && last_percent == 42 && last_charging == 1);
    helios_chronos_update_battery(0, 0);
    assert(reports == 2 && last_percent == 0 && last_charging == 0);
    sample.percent = 100;
    sample.charging = 0;
    helios_chronos_config(CHRONOS_CONFIG_DEVICE_INFO_REQUEST, 0, 0);
    assert(reports == 3 && last_percent == 100 && last_charging == 0);
    read_error = -RT_ERROR;
    helios_chronos_config(CHRONOS_CONFIG_DEVICE_INFO_REQUEST, 0, 0);
    assert(reports == 3);
    subscribed = 0;
    assert(!history_callback(&record, history_user) && history_reports == 1);
    helios_chronos_on_rx(history_packet, (uint16_t)history_length);
    assert(history_requests == 1);
    helios_chronos_update_activity(12346, 501);
    assert(activity_reports == 1);
    helios_chronos_update_battery(30, 1);
    assert(reports == 3);
    connected = 0;
    helios_chronos_on_disconnected();
    assert(history_cancellations == 1 && !history_callback);
    helios_chronos_update_battery(30, 1);
    assert(reports == 3);
    read_error = RT_EOK;
    connected = subscribed = 1;
    helios_chronos_on_connected();
    helios_chronos_on_subscribed();
    assert(reports == 4 && last_percent == 100);
    return 0;
}
