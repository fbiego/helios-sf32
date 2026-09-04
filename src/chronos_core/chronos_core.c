/*********************
 *      INCLUDES
 *********************/

#include "chronos_core.h"

#include <string.h>

/*********************
 *      DEFINES
 *********************/

#define CHRONOS_PREFIX_AB 0xABu
#define CHRONOS_PREFIX_EA 0xEAu
#define CHRONOS_PROTOCOL_EXTENSION_BYTE 0xFEu
#define CHRONOS_PROTOCOL_STANDARD_BYTE 0xFFu
#define CHRONOS_HEADER_SIZE 6u
#define CHRONOS_LENGTH_EXTRA 3u

/**********************
 *  STATIC VARIABLES
 **********************/

static chronos_core_t default_core;
static chronos_packet_callback_t tx_callback;
static chronos_connection_callback_t connection_callback;
static chronos_notification_callback_t notification_callback;
static chronos_ringer_callback_t ringer_callback;
static chronos_packet_callback_t raw_rx_callback;
static chronos_packet_callback_t rx_packet_callback;
static chronos_config_callback_t config_callback;
static chronos_health_request_callback_t health_request_callback;
static chronos_time_callback_t time_callback;

/**********************
 *  STATIC FUNCTIONS
 **********************/

static uint16_t read_u16_be(const uint8_t *data)
{
    return (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
}

static void write_u16_be(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)value;
}

static uint32_t pack_u24(uint32_t value)
{
    return value > 0xFFFFFFu ? 0xFFFFFFu : value;
}

static uint8_t year_since_2000(uint32_t year)
{
    return year >= 2000u ? (uint8_t)(year - 2000u) : (uint8_t)year;
}

static void copy_bytes_to_string(char *dest, size_t dest_size, const uint8_t *src, size_t src_size)
{
    if (dest == NULL || dest_size == 0u) {
        return;
    }

    size_t copy_size = src_size;
    if (copy_size >= dest_size) {
        copy_size = dest_size - 1u;
    }
    if (copy_size > 0u && src != NULL) {
        memcpy(dest, src, copy_size);
    }
    dest[copy_size] = '\0';
}

static void copy_cstr(char *dest, size_t dest_size, const char *src)
{
    if (src == NULL) {
        copy_bytes_to_string(dest, dest_size, NULL, 0u);
        return;
    }

    copy_bytes_to_string(dest, dest_size, (const uint8_t *)src, strlen(src));
}

static void set_config_event(chronos_event_t *event, chronos_config_t config, uint32_t a, uint32_t b)
{
    event->type = CHRONOS_EVENT_CONFIG;
    event->has_config = true;
    event->config = config;
    event->config_a = a;
    event->config_b = b;
}

static uint32_t pack_time_range(uint8_t hour, uint8_t minute, uint8_t hour2, uint8_t minute2)
{
    return ((uint32_t)hour << 24) | ((uint32_t)minute << 16) |
           ((uint32_t)hour2 << 8) | (uint32_t)minute2;
}

static uint32_t read_u32_be(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) | (uint32_t)data[3];
}

static uint32_t current_unix_timestamp(void)
{
    return time_callback != NULL ? time_callback() : 0u;
}

static const char *chronos_app_name(uint8_t id)
{
    switch (id) {
    case 0x03:
        return "Message";
    case 0x04:
        return "Mail";
    case 0x07:
        return "Tencent";
    case 0x08:
        return "Skype";
    case 0x09:
        return "Wechat";
    case 0x0A:
        return "WhatsApp";
    case 0x0B:
        return "Gmail";
    case 0x0E:
        return "Line";
    case 0x0F:
        return "Twitter";
    case 0x10:
        return "Facebook";
    case 0x11:
        return "Messenger";
    case 0x12:
        return "Instagram";
    case 0x13:
        return "Weibo";
    case 0x14:
        return "KakaoTalk";
    case 0x16:
        return "Viber";
    case 0x17:
        return "Vkontakte";
    case 0x18:
        return "Telegram";
    case 0x1B:
        return "DingTalk";
    case 0x20:
        return "WhatsApp Business";
    case 0x22:
        return "WearFit Pro";
    case 0xC0:
        return "Chronos";
    default:
        return "Message";
    }
}

static void split_notification_text(
    uint8_t icon,
    const uint8_t *message,
    size_t message_length,
    chronos_notification_t *notification)
{
    if (message == NULL || notification == NULL) {
        return;
    }

    /* The supplied length may include unused or stale buffer data. */
    size_t actual_length = 0;
    while (actual_length < message_length && message[actual_length] != '\0') {
        actual_length++;
    }
    message_length = actual_length;

    notification->title[0] = '\0';
    notification->message[0] = '\0';

    size_t colon = message_length;
    size_t newline = message_length;

    for (size_t i = 0; i < message_length; i++) {
        if (message[i] == ':' && colon == message_length) {
            colon = i;
        }

        if (message[i] == '\n' && newline == message_length) {
            newline = i;
        }
    }

    if (colon != message_length && colon < 30u &&
        (newline == message_length || newline > colon)) {
        copy_bytes_to_string(
            notification->title,
            sizeof(notification->title),
            message,
            colon);

        /* Optionally skip one space after the colon. */
        size_t message_start = colon + 1u;
        if (message_start < message_length &&
            message[message_start] == ' ') {
            message_start++;
        }

        copy_bytes_to_string(
            notification->message,
            sizeof(notification->message),
            &message[message_start],
            message_length - message_start);
    } else {
        copy_cstr(
            notification->title,
            sizeof(notification->title),
            chronos_app_name(icon));

        copy_bytes_to_string(
            notification->message,
            sizeof(notification->message),
            message,
            message_length);
    }
}

static void dispatch_config_callback(chronos_core_t *core, const chronos_event_t *event)
{
    if (config_callback == NULL || core == NULL || event == NULL || !event->has_config) {
        return;
    }

    config_callback(event->config, event->config_a, event->config_b);
}

/**********************
 * GLOBAL FUNCTIONS
 **********************/

void chronos_core_context_init(chronos_core_t *core)
{
    if (core == NULL) {
        return;
    }

    memset(core, 0, sizeof(*core));
    core->notify_phone = true;
    core->hour_24 = true;
    core->notification_index = 0;
    core->navigation.icon_crc = 0xFFFFFFFFu;
    core->phone_info.battery_level = 0;
    copy_cstr(core->qr_links[0], sizeof(core->qr_links[0]), "https://chronos.ke/");
    core->notifications[0].icon = 0xC0;
    core->notifications[0].timestamp = current_unix_timestamp();
    copy_cstr(core->notifications[0].app, sizeof(core->notifications[0].app), "Chronos");
    copy_cstr(
        core->notifications[0].message,
        sizeof(core->notifications[0].message),
        "Download from Google Play to sync time and receive notifications");
}

void chronos_core_init(void)
{
    chronos_core_context_init(&default_core);
}

chronos_core_t *chronos_core_get_state(void)
{
    return &default_core;
}

void chronos_core_set_tx_cb(chronos_packet_callback_t callback)
{
    tx_callback = callback;
}

void chronos_core_set_connection_cb(chronos_connection_callback_t callback)
{
    connection_callback = callback;
}

void chronos_core_set_notification_cb(chronos_notification_callback_t callback)
{
    notification_callback = callback;
}

void chronos_core_set_ringer_cb(chronos_ringer_callback_t callback)
{
    ringer_callback = callback;
}

void chronos_core_set_raw_rx_cb(chronos_packet_callback_t callback)
{
    raw_rx_callback = callback;
}

void chronos_core_set_data_cb(chronos_packet_callback_t callback)
{
    rx_packet_callback = callback;
}

void chronos_core_set_rx_packet_cb(chronos_packet_callback_t callback)
{
    rx_packet_callback = callback;
}

void chronos_core_set_config_cb(chronos_config_callback_t callback)
{
    config_callback = callback;
}

void chronos_core_set_health_request_cb(chronos_health_request_callback_t callback)
{
    health_request_callback = callback;
}

void chronos_core_set_time_cb(chronos_time_callback_t callback)
{
    time_callback = callback;
}

void chronos_core_set_connected(bool connected)
{
    bool changed = default_core.connected != connected;
    default_core.connected = connected;
    if (changed && connection_callback != NULL) {
        connection_callback(connected);
    }
}

void chronos_core_set_subscribed(bool subscribed)
{
    default_core.subscribed = subscribed;
}

void chronos_core_set_connection_status(bool connected, bool subscribed)
{
    bool changed = default_core.connected != connected;
    default_core.connected = connected;
    default_core.subscribed = subscribed;
    if (changed && connection_callback != NULL) {
        connection_callback(connected);
    }
}

bool chronos_core_is_connected(void)
{
    return default_core.connected;
}

bool chronos_core_is_subscribed(void)
{
    return default_core.subscribed;
}

bool chronos_core_is_chunked(void)
{
    return default_core.chunked;
}

bool chronos_core_is_24_hour(void)
{
    return default_core.hour_24;
}

bool chronos_core_is_camera_ready(void)
{
    return default_core.camera_ready;
}

bool chronos_core_is_quiet_enabled(void)
{
    return default_core.quiet_enabled;
}

bool chronos_core_is_sleep_enabled(void)
{
    return default_core.sleep_enabled;
}

uint16_t chronos_core_get_quiet_start(void)
{
    return default_core.quiet_start;
}

uint16_t chronos_core_get_quiet_end(void)
{
    return default_core.quiet_end;
}

uint16_t chronos_core_get_sleep_start(void)
{
    return default_core.sleep_start;
}

uint16_t chronos_core_get_sleep_end(void)
{
    return default_core.sleep_end;
}

const chronos_date_time_t *chronos_core_get_date_time(void)
{
    return &default_core.date_time;
}

bool chronos_core_is_phone_charging(void)
{
    return default_core.phone_info.is_charging;
}

uint8_t chronos_core_get_phone_battery(void)
{
    return default_core.phone_info.battery_level;
}

int chronos_core_get_app_code(void)
{
    return default_core.phone_info.app_code;
}

int chronos_core_get_sdk_version(void)
{
    return default_core.phone_info.sdk_version;
}

const char *chronos_core_get_app_version(void)
{
    return default_core.phone_info.app_version;
}

const char *chronos_core_get_phone_manufacturer(void)
{
    return default_core.phone_info.manufacturer;
}

const char *chronos_core_get_phone_model(void)
{
    return default_core.phone_info.model;
}

const chronos_phone_info_t *chronos_core_get_phone_info(void)
{
    return &default_core.phone_info;
}

const chronos_remote_touch_t *chronos_core_get_touch(void)
{
    return &default_core.touch;
}

const chronos_navigation_t *chronos_core_get_navigation(void)
{
    return &default_core.navigation;
}

const chronos_music_info_t *chronos_core_get_music_info(void)
{
    return &default_core.music_info;
}

int chronos_core_get_notification_count(void)
{
    int count = default_core.notification_index + 1;
    if (count < 0) {
        return 0;
    }
    return count > (int)CHRONOS_NOTIF_SIZE ? (int)CHRONOS_NOTIF_SIZE : count;
}

const chronos_notification_t *chronos_core_get_notification_at(int index)
{
    int count = chronos_core_get_notification_count();
    if (index < 0 || index >= count) {
        return NULL;
    }

    int latest_index = (default_core.notification_index - index + (int)CHRONOS_NOTIF_SIZE) %
                       (int)CHRONOS_NOTIF_SIZE;
    return &default_core.notifications[latest_index];
}

int chronos_core_get_weather_count(void)
{
    return default_core.weather_size;
}

const chronos_weather_t *chronos_core_get_weather_at(int index)
{
    if (index < 0 || index >= (int)CHRONOS_WEATHER_SIZE) {
        return NULL;
    }
    return &default_core.weather[index];
}

const char *chronos_core_get_weather_city(void)
{
    return default_core.weather_city;
}

uint32_t chronos_core_get_weather_time(void)
{
    return default_core.weather_time;
}

const chronos_weather_location_t *chronos_core_get_weather_location(void)
{
    return &default_core.weather_location;
}

const chronos_hourly_forecast_t *chronos_core_get_forecast_hour(int hour)
{
    if (hour < 0 || hour >= (int)CHRONOS_FORECAST_SIZE) {
        return NULL;
    }
    return &default_core.hourly_forecast[hour];
}

bool chronos_core_has_forecast_hour(int hour)
{
    if (hour < 0 || hour >= (int)CHRONOS_FORECAST_SIZE) {
        return false;
    }
    return default_core.hourly_forecast_valid[hour];
}

const chronos_alarm_t *chronos_core_get_alarm(int index)
{
    if (index < 0 || index >= (int)CHRONOS_ALARM_SIZE) {
        return NULL;
    }
    return &default_core.alarms[index];
}

const char *chronos_core_get_qr_at(int index)
{
    if (index < 0 || index >= (int)CHRONOS_QR_SIZE) {
        return NULL;
    }
    return default_core.qr_links[index];
}

const chronos_contact_t *chronos_core_get_contact(int index)
{
    if (index < 0 || index >= (int)CHRONOS_CONTACTS_SIZE) {
        return NULL;
    }
    return &default_core.contacts[index];
}

int chronos_core_get_contact_count(void)
{
    return default_core.contact_size;
}

int chronos_core_get_sos_contact_index(void)
{
    return default_core.sos_contact;
}

chronos_status_t chronos_core_send_raw_packet(uint8_t *data, size_t length, bool chunked)
{
    if (data == NULL || length == 0u || tx_callback == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }

    if (!chunked || length <= CHRONOS_CORE_BLE_CHUNK_SIZE) {
        tx_callback(data, length);
        return CHRONOS_STATUS_OK;
    }

    chronos_tx_t tx;
    chronos_status_t status = chronos_core_tx_begin(&tx, data, length, true);
    if (status != CHRONOS_STATUS_OK) {
        return status;
    }

    uint8_t chunk[CHRONOS_CORE_BLE_CHUNK_SIZE];
    size_t chunk_length = 0;
    while (chronos_core_tx_next(&tx, chunk, sizeof(chunk), &chunk_length)) {
        tx_callback(chunk, chunk_length);
    }

    return CHRONOS_STATUS_OK;
}

static chronos_status_t send_built_packet(chronos_status_t build_status, uint8_t *packet, size_t length, bool chunked)
{
    if (build_status != CHRONOS_STATUS_OK) {
        return build_status;
    }

    return chronos_core_send_raw_packet(packet, length, chunked);
}

chronos_status_t chronos_core_receive_ble_packet(uint8_t *data, size_t length)
{
    if (data == NULL || length == 0u) {
        return CHRONOS_STATUS_INVALID_ARG;
    }

    if (raw_rx_callback != NULL) {
        raw_rx_callback(data, length);
    }

    chronos_packet_t packet;
    chronos_status_t status = chronos_core_receive(&default_core, data, length, &packet);
    if (status != CHRONOS_STATUS_PACKET_READY) {
        return status;
    }

    if (rx_packet_callback != NULL) {
        rx_packet_callback(packet.data, packet.length);
    }

    chronos_const_packet_t const_packet = {packet.data, packet.length};
    chronos_event_t event;
    status = chronos_core_handle_packet(&default_core, &const_packet, &event);
    if (status == CHRONOS_STATUS_OK) {
        dispatch_config_callback(&default_core, &event);
    }

    return status;
}

void chronos_core_reset_rx(chronos_core_t *core)
{
    if (core == NULL) {
        return;
    }

    core->expected_length = 0;
    core->received_length = 0;
    core->active = false;
}

bool chronos_core_is_packet_start(const uint8_t *data, size_t length)
{
    if (data == NULL || length < 4u) {
        return false;
    }

    return (data[0] == CHRONOS_PREFIX_AB || data[0] == CHRONOS_PREFIX_EA) &&
           (data[3] == CHRONOS_PROTOCOL_EXTENSION_BYTE ||
            data[3] == CHRONOS_PROTOCOL_STANDARD_BYTE);
}

size_t chronos_core_expected_length(const uint8_t *data, size_t length)
{
    if (!chronos_core_is_packet_start(data, length)) {
        return 0;
    }

    return (size_t)read_u16_be(&data[1]) + CHRONOS_LENGTH_EXTRA;
}

chronos_status_t chronos_core_receive(
    chronos_core_t *core,
    const uint8_t *data,
    size_t length,
    chronos_packet_t *packet)
{
    if (core == NULL || data == NULL || packet == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }
    if (length == 0u) {
        return CHRONOS_STATUS_INVALID_PACKET;
    }

    packet->data = NULL;
    packet->length = 0;

    if (chronos_core_is_packet_start(data, length)) {
        size_t expected = chronos_core_expected_length(data, length);
        if (expected < CHRONOS_HEADER_SIZE || expected > CHRONOS_CORE_MAX_PACKET_SIZE) {
            chronos_core_reset_rx(core);
            return CHRONOS_STATUS_INVALID_PACKET;
        }
        if (length > CHRONOS_CORE_MAX_PACKET_SIZE) {
            chronos_core_reset_rx(core);
            return CHRONOS_STATUS_OVERFLOW;
        }

        memcpy(core->data, data, length);
        core->expected_length = expected;
        core->received_length = length;
        core->active = true;

        if (core->received_length >= core->expected_length) {
            packet->data = core->data;
            packet->length = core->expected_length;
            core->active = false;
            return CHRONOS_STATUS_PACKET_READY;
        }

        return CHRONOS_STATUS_PENDING;
    }

    if (!core->active) {
        return CHRONOS_STATUS_NO_ACTIVE_PACKET;
    }
    if (length < 2u) {
        return CHRONOS_STATUS_INVALID_PACKET;
    }

    size_t offset = CHRONOS_CORE_BLE_CHUNK_SIZE +
                    ((size_t)data[0] * (CHRONOS_CORE_BLE_CHUNK_SIZE - 1u));
    size_t payload_length = length - 1u;
    if (offset >= CHRONOS_CORE_MAX_PACKET_SIZE ||
        payload_length > CHRONOS_CORE_MAX_PACKET_SIZE - offset) {
        chronos_core_reset_rx(core);
        return CHRONOS_STATUS_OVERFLOW;
    }

    memcpy(&core->data[offset], &data[1], payload_length);
    if (offset + payload_length > core->received_length) {
        core->received_length = offset + payload_length;
    }

    if (core->received_length >= core->expected_length) {
        packet->data = core->data;
        packet->length = core->expected_length;
        core->active = false;
        return CHRONOS_STATUS_PACKET_READY;
    }

    return CHRONOS_STATUS_PENDING;
}

chronos_status_t chronos_core_parse_event(
    const chronos_const_packet_t *packet,
    chronos_event_t *event)
{
    if (packet == NULL || packet->data == NULL || event == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }
    if (packet->length < CHRONOS_HEADER_SIZE ||
        !chronos_core_is_packet_start(packet->data, packet->length)) {
        return CHRONOS_STATUS_INVALID_PACKET;
    }

    memset(event, 0, sizeof(*event));
    event->type = CHRONOS_EVENT_RAW_PACKET;
    event->prefix = packet->data[0];
    event->protocol = packet->data[3];
    event->command = packet->data[4];
    event->subtype = packet->data[5];
    event->payload = &packet->data[6];
    event->payload_length = packet->length - 6u;

    if (event->prefix == CHRONOS_PREFIX_AB) {
        switch (event->command) {
        case 0x20:
            if (event->protocol == CHRONOS_PROTOCOL_EXTENSION_BYTE) {
                event->type = CHRONOS_EVENT_SYNCED;
            }
            break;

        case 0x23:
            event->type = CHRONOS_EVENT_RESET_REQUEST;
            break;

        case 0x31:
            if (event->payload_length >= 1u) {
                event->type = CHRONOS_EVENT_HEALTH_REQUEST;
                event->value.health.enabled = event->payload[0] != 0u;
                switch (event->subtype) {
                case 0x0A:
                    event->value.health.request = CHRONOS_HEALTH_HEART_RATE_MEASURE;
                    break;
                case 0x12:
                    event->value.health.request = CHRONOS_HEALTH_BLOOD_OXYGEN_MEASURE;
                    break;
                case 0x22:
                    event->value.health.request = CHRONOS_HEALTH_BLOOD_PRESSURE_MEASURE;
                    break;
                default:
                    event->type = CHRONOS_EVENT_RAW_PACKET;
                    break;
                }
            }
            break;

        case 0x32:
            if (event->payload_length >= 1u) {
                event->type = CHRONOS_EVENT_HEALTH_REQUEST;
                event->value.health.request = CHRONOS_HEALTH_MEASURE_ALL;
                event->value.health.enabled = event->payload[0] != 0u;
            }
            break;

        case 0x51:
            if (event->subtype == 0x80) {
                event->type = CHRONOS_EVENT_HEALTH_REQUEST;
                event->value.health.request = CHRONOS_HEALTH_STEPS_RECORDS;
                event->value.health.enabled = true;
            }
            break;

        case 0x52:
            if (event->subtype == 0x80) {
                event->type = CHRONOS_EVENT_HEALTH_REQUEST;
                event->value.health.request = CHRONOS_HEALTH_SLEEP_RECORDS;
                event->value.health.enabled = true;
            }
            break;

        case 0x91:
            if (event->protocol == CHRONOS_PROTOCOL_EXTENSION_BYTE && event->payload_length >= 2u) {
                event->type = CHRONOS_EVENT_PHONE_BATTERY;
                event->value.phone_battery.charging = event->payload[0] == 1u;
                event->value.phone_battery.level = event->payload[1];
            }
            break;

        case 0xBF:
            if (event->protocol == CHRONOS_PROTOCOL_EXTENSION_BYTE && event->payload_length >= 4u) {
                event->type = CHRONOS_EVENT_TOUCH;
                event->value.touch.pressed = event->subtype == 1u;
                event->value.touch.x = read_u16_be(&event->payload[0]);
                event->value.touch.y = read_u16_be(&event->payload[2]);
            }
            break;

        case 0xCA:
            if (event->protocol == CHRONOS_PROTOCOL_EXTENSION_BYTE && event->payload_length >= 2u) {
                event->type = CHRONOS_EVENT_APP_INFO;
                event->value.app_info.code = read_u16_be(&event->payload[0]);
                event->value.app_info.version = (const char *)&event->payload[2];
                event->value.app_info.version_length = event->payload_length - 2u;
            }
            break;

        case 0xCB:
            if (event->protocol == CHRONOS_PROTOCOL_EXTENSION_BYTE && event->payload_length >= 2u) {
                size_t index = 2u;
                event->type = CHRONOS_EVENT_PHONE_INFO;
                event->value.phone_info.sdk_version = read_u16_be(&event->payload[0]);
                event->value.phone_info.manufacturer = (const char *)&event->payload[index];
                while (index < event->payload_length && event->payload[index] != 0u) {
                    index++;
                }
                event->value.phone_info.manufacturer_length =
                    (size_t)((const char *)&event->payload[index] -
                             event->value.phone_info.manufacturer);
                if (index < event->payload_length) {
                    index++;
                }
                event->value.phone_info.model = (const char *)&event->payload[index];
                while (index < event->payload_length && event->payload[index] != 0u) {
                    index++;
                }
                event->value.phone_info.model_length =
                    (size_t)((const char *)&event->payload[index] -
                             event->value.phone_info.model);
            }
            break;

        case 0xCC:
            if (event->protocol == CHRONOS_PROTOCOL_EXTENSION_BYTE) {
                event->type = CHRONOS_EVENT_CHUNKED_TRANSFER;
                event->value.chunked_transfer.enabled = event->subtype != 0u;
            }
            break;

        default:
            break;
        }
    }

    return CHRONOS_STATUS_OK;
}

chronos_status_t chronos_core_handle_packet(
    chronos_core_t *core,
    const chronos_const_packet_t *packet,
    chronos_event_t *event)
{
    chronos_event_t local_event;
    chronos_event_t *target_event = event != NULL ? event : &local_event;

    if (core == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }

    chronos_status_t status = chronos_core_parse_event(packet, target_event);
    if (status != CHRONOS_STATUS_OK) {
        return status;
    }

    core->incoming_data.length = (int)packet->length;
    if (packet->length <= CHRONOS_CORE_MAX_PACKET_SIZE) {
        memcpy(core->incoming_data.data, packet->data, packet->length);
    }

    if (target_event->type == CHRONOS_EVENT_RAW_PACKET &&
        packet->length >= 8u &&
        packet->data[0] == CHRONOS_PREFIX_AB &&
        packet->data[4] == 0x72u) {
        uint8_t icon = packet->data[6];
        uint8_t state = packet->data[7];
        const uint8_t *message = &packet->data[8];
        size_t message_length = packet->length - 8u;

        if (icon == 0x01u || icon == 0x02u) {
            copy_bytes_to_string(core->ringer_caller, sizeof(core->ringer_caller), message, message_length);
            target_event->type = CHRONOS_EVENT_RINGER;
            target_event->value.ringer.caller = core->ringer_caller;
            target_event->value.ringer.active = icon == 0x01u;
        } else if (state == 0x02u) {
            core->notification_index++;
            int index = core->notification_index % (int)CHRONOS_NOTIF_SIZE;
            chronos_notification_t *notification = &core->notifications[index];
            memset(notification, 0, sizeof(*notification));
            notification->icon = icon;
            notification->timestamp = current_unix_timestamp();
            copy_cstr(notification->app, sizeof(notification->app), chronos_app_name(icon));
            split_notification_text(icon, message, message_length, notification);
            target_event->type = CHRONOS_EVENT_NOTIFICATION;
            target_event->value.notification.notification = notification;
        }
    }

    if (target_event->type == CHRONOS_EVENT_RAW_PACKET &&
        packet->length >= 6u &&
        packet->data[0] == CHRONOS_PREFIX_AB &&
        packet->data[3] == CHRONOS_PROTOCOL_EXTENSION_BYTE &&
        packet->data[4] == 0xEFu) {
        uint8_t state = packet->data[5];
        target_event->type = CHRONOS_EVENT_NAVIGATION;
        target_event->has_config = true;
        target_event->config = CHRONOS_CONFIG_NAV_DATA;
        target_event->config_a = 0;
        target_event->config_b = 0;

        if (state == 0x00u) {
            core->navigation.active = false;
            core->navigation.has_icon = false;
            core->navigation.is_navigation = false;
            core->navigation.icon_crc = 0xFFFFFFFFu;
            copy_cstr(core->navigation.eta, sizeof(core->navigation.eta), "Navigation");
            copy_cstr(core->navigation.title, sizeof(core->navigation.title), "Chronos");
            copy_cstr(core->navigation.duration, sizeof(core->navigation.duration), "Inactive");
            copy_cstr(core->navigation.distance, sizeof(core->navigation.distance), "");
            copy_cstr(core->navigation.speed, sizeof(core->navigation.speed), "");
            copy_cstr(
                core->navigation.directions,
                sizeof(core->navigation.directions),
                "Start navigation on Google maps");
        } else if (state == 0xFFu) {
            core->navigation.active = true;
            core->navigation.has_icon = false;
            core->navigation.is_navigation = false;
            core->navigation.icon_crc = 0xFFFFFFFFu;
            copy_cstr(core->navigation.title, sizeof(core->navigation.title), "Chronos");
            copy_cstr(core->navigation.duration, sizeof(core->navigation.duration), "Disabled");
            copy_cstr(core->navigation.distance, sizeof(core->navigation.distance), "");
            copy_cstr(core->navigation.speed, sizeof(core->navigation.speed), "");
            copy_cstr(core->navigation.eta, sizeof(core->navigation.eta), "Navigation");
            copy_cstr(core->navigation.directions, sizeof(core->navigation.directions), "Check Chronos app settings");
        } else if (state == 0x80u && packet->length >= 12u) {
            size_t index = 12u;
            core->navigation.active = true;
            core->navigation.has_icon = packet->data[6] == 1u;
            core->navigation.is_navigation = packet->data[7] == 1u;
            core->navigation.icon_crc = ((uint32_t)packet->data[8] << 24) |
                                        ((uint32_t)packet->data[9] << 16) |
                                        ((uint32_t)packet->data[10] << 8) |
                                        (uint32_t)packet->data[11];
            char *fields[] = {
                core->navigation.title,
                core->navigation.duration,
                core->navigation.distance,
                core->navigation.eta,
                core->navigation.directions,
                core->navigation.speed
            };
            size_t sizes[] = {
                sizeof(core->navigation.title),
                sizeof(core->navigation.duration),
                sizeof(core->navigation.distance),
                sizeof(core->navigation.eta),
                sizeof(core->navigation.directions),
                sizeof(core->navigation.speed)
            };

            for (size_t field = 0; field < 6u; field++) {
                size_t start = index;
                while (index < packet->length && packet->data[index] != 0u) {
                    index++;
                }
                copy_bytes_to_string(fields[field], sizes[field], &packet->data[start], index - start);
                if (index < packet->length) {
                    index++;
                }
            }
        }

        target_event->config_a = core->navigation.active ? 1u : 0u;
    }

    if (target_event->type == CHRONOS_EVENT_RAW_PACKET &&
        packet->length >= 6u &&
        packet->data[0] == CHRONOS_PREFIX_AB) {
        switch (packet->data[4]) {
        case 0x53:
            if (packet->length >= 12u) {
                uint32_t interval = ((uint32_t)packet->data[11] << 16) | (uint16_t)packet->data[6];
                uint32_t water = pack_time_range(packet->data[7], packet->data[8], packet->data[9], packet->data[10]);
                set_config_event(target_event, CHRONOS_CONFIG_WATER, interval, water);
            }
            break;

        case 0x71:
            set_config_event(target_event, CHRONOS_CONFIG_FIND, 0, 0);
            break;

        case 0x73:
            if (packet->length >= 11u) {
                uint32_t index = packet->data[6] % CHRONOS_ALARM_SIZE;
                chronos_alarm_t *alarm = &core->alarms[index];
                alarm->hour = packet->data[8];
                alarm->minute = packet->data[9];
                alarm->repeat = packet->data[10];
                alarm->enabled = packet->data[7] != 0u;
                uint32_t packed_alarm = ((uint32_t)alarm->hour << 24) |
                                        ((uint32_t)alarm->minute << 16) |
                                        ((uint32_t)alarm->repeat << 8) |
                                        (uint32_t)alarm->enabled;
                set_config_event(target_event, CHRONOS_CONFIG_ALARM, index, packed_alarm);
            }
            break;

        case 0x74:
            if (packet->length >= 13u) {
                uint32_t u1 = ((uint32_t)packet->data[7] << 24) |
                              ((uint32_t)packet->data[8] << 16) |
                              ((uint32_t)packet->data[9] << 8) |
                              (uint32_t)packet->data[6];
                uint32_t u2 = ((uint32_t)packet->data[10] << 24) |
                              ((uint32_t)packet->data[11] << 16) |
                              ((uint32_t)packet->data[12] << 8) |
                              (uint32_t)packet->data[6];
                set_config_event(target_event, CHRONOS_CONFIG_USER, u1, u2);
            }
            break;

        case 0x75:
            if (packet->length >= 12u) {
                uint32_t interval = ((uint32_t)packet->data[11] << 16) | (uint16_t)packet->data[6];
                uint32_t sedentary = pack_time_range(packet->data[7], packet->data[8], packet->data[9], packet->data[10]);
                set_config_event(target_event, CHRONOS_CONFIG_SEDENTARY, interval, sedentary);
            }
            break;

        case 0x76:
            if (packet->length >= 11u) {
                core->quiet_enabled = packet->data[6] != 0u;
                core->quiet_start = (uint16_t)(packet->data[7] * 60u + packet->data[8]);
                core->quiet_end = (uint16_t)(packet->data[9] * 60u + packet->data[10]);
                set_config_event(
                    target_event,
                    CHRONOS_CONFIG_QUIET,
                    core->quiet_enabled ? 1u : 0u,
                    pack_time_range(packet->data[7], packet->data[8], packet->data[9], packet->data[10]));
            }
            break;

        case 0x77:
            if (packet->length >= 7u) {
                set_config_event(target_event, CHRONOS_CONFIG_RAISE_TO_WAKE, 0, packet->data[6]);
            }
            break;

        case 0x78:
            if (packet->length >= 7u) {
                set_config_event(target_event, CHRONOS_CONFIG_HOURLY, 0, packet->data[6]);
            }
            break;

        case 0x79:
            if (packet->length >= 7u) {
                core->camera_ready = packet->data[6] == 1u;
                set_config_event(target_event, CHRONOS_CONFIG_CAMERA, 0, packet->data[6]);
            }
            break;

        case 0x7B:
            if (packet->length >= 7u) {
                set_config_event(target_event, CHRONOS_CONFIG_LANGUAGE, 0, packet->data[6]);
            }
            break;

        case 0x7C:
            if (packet->length >= 7u) {
                core->hour_24 = packet->data[6] == 0u;
                set_config_event(target_event, CHRONOS_CONFIG_24_HOUR, 0, core->hour_24 ? 1u : 0u);
            }
            break;

        case 0x7E:
            core->weather_size = 0;
            for (size_t k = 0; k < (packet->length - 6u) / 2u && k < CHRONOS_WEATHER_SIZE; k++) {
                uint8_t icon_temp = packet->data[(k * 2u) + 6u];
                int sign = (icon_temp & 1u) ? -1 : 1;
                core->weather[k].day = (int)k;
                core->weather[k].icon = icon_temp >> 4;
                core->weather[k].temp = (int)packet->data[(k * 2u) + 7u] * sign;
                core->weather_size++;
            }
            core->weather_time = current_unix_timestamp();
            set_config_event(target_event, CHRONOS_CONFIG_WEATHER, 1, 0);
            break;

        case 0x88:
            for (size_t k = 0; k < (packet->length - 6u) / 2u && k < CHRONOS_WEATHER_SIZE; k++) {
                uint8_t high = packet->data[(k * 2u) + 6u];
                uint8_t low = packet->data[(k * 2u) + 7u];
                core->weather[k].high = (int)(high & 0x7Fu) * ((high & 0x80u) ? -1 : 1);
                core->weather[k].low = (int)(low & 0x7Fu) * ((low & 0x80u) ? -1 : 1);
            }
            core->weather_time = current_unix_timestamp();
            set_config_event(target_event, CHRONOS_CONFIG_WEATHER, 2, 0);
            break;

        case 0x8A:
            if (packet->length >= 9u) {
                core->weather[0].uv = packet->data[6];
                core->weather[0].pressure = read_u16_be(&packet->data[7]);
                core->weather_time = current_unix_timestamp();
                target_event->type = CHRONOS_EVENT_CONFIG;
            }
            break;

        case 0x92:
            if (packet->data[3] == CHRONOS_PROTOCOL_STANDARD_BYTE &&
                packet->data[5] == 0x80u &&
                packet->length >= 7u) {
                set_config_event(
                    target_event,
                    CHRONOS_CONFIG_DEVICE_INFO_REQUEST,
                    packet->data[5],
                    packet->data[6]);
            }
            break;

        case 0x7F:
            if (packet->length >= 11u) {
                core->sleep_enabled = packet->data[6] != 0u;
                core->sleep_start = (uint16_t)(packet->data[7] * 60u + packet->data[8]);
                core->sleep_end = (uint16_t)(packet->data[9] * 60u + packet->data[10]);
                set_config_event(
                    target_event,
                    CHRONOS_CONFIG_SLEEP,
                    core->sleep_enabled ? 1u : 0u,
                    pack_time_range(packet->data[7], packet->data[8], packet->data[9], packet->data[10]));
            }
            break;

        case 0x93:
            if (packet->length >= 14u) {
                core->date_time.second = packet->data[13];
                core->date_time.minute = packet->data[12];
                core->date_time.hour = packet->data[11];
                core->date_time.day = packet->data[10];
                core->date_time.month = packet->data[9];
                core->date_time.year = ((uint32_t)packet->data[7] << 8) | packet->data[8];
                set_config_event(target_event, CHRONOS_CONFIG_TIME, 1, 0);
            }
            break;

        case 0x9C:
            if (packet->length >= 10u) {
                uint32_t color = ((uint32_t)packet->data[5] << 16) |
                                 ((uint32_t)packet->data[6] << 8) |
                                 (uint32_t)packet->data[7];
                uint32_t select = ((uint32_t)packet->data[8] << 16) | (uint32_t)packet->data[9];
                set_config_event(target_event, CHRONOS_CONFIG_FONT, color, select);
            }
            break;

        case 0x9D:
            if (packet->data[3] == CHRONOS_PROTOCOL_EXTENSION_BYTE) {
                if (packet->data[5] == 0x80u && packet->length >= 13u) {
                    size_t index = 13u;
                    core->music_info.state = packet->data[6];
                    core->music_info.background_color = ((uint32_t)packet->data[7] << 16) |
                                                        ((uint32_t)packet->data[8] << 8) |
                                                        (uint32_t)packet->data[9];
                    core->music_info.text_color = ((uint32_t)packet->data[10] << 16) |
                                                  ((uint32_t)packet->data[11] << 8) |
                                                  (uint32_t)packet->data[12];
                    size_t start = index;
                    while (index < packet->length && packet->data[index] != 0u) {
                        index++;
                    }
                    copy_bytes_to_string(core->music_info.app_name, sizeof(core->music_info.app_name), &packet->data[start], index - start);
                    if (index < packet->length) {
                        index++;
                    }
                    start = index;
                    while (index < packet->length && packet->data[index] != 0u) {
                        index++;
                    }
                    copy_bytes_to_string(core->music_info.package_name, sizeof(core->music_info.package_name), &packet->data[start], index - start);
                    set_config_event(target_event, CHRONOS_CONFIG_MUSIC, 0, core->music_info.state);
                } else if (packet->data[5] == 0x81u && packet->length >= 7u) {
                    copy_bytes_to_string(core->music_info.title, sizeof(core->music_info.title), &packet->data[7], packet->length - 7u);
                    set_config_event(target_event, CHRONOS_CONFIG_MUSIC, 1, core->music_info.state);
                } else if (packet->data[5] == 0x82u && packet->length >= 7u) {
                    copy_bytes_to_string(core->music_info.artist, sizeof(core->music_info.artist), &packet->data[7], packet->length - 7u);
                    set_config_event(target_event, CHRONOS_CONFIG_MUSIC, 2, core->music_info.state);
                }
            }
            break;

        case 0xA2:
            if (packet->length >= 6u && packet->data[5] < CHRONOS_CONTACTS_SIZE) {
                int pos = packet->data[5];
                copy_bytes_to_string(core->contacts[pos].name, sizeof(core->contacts[pos].name), &packet->data[6], packet->length - 6u);
                target_event->type = CHRONOS_EVENT_CONFIG;
            }
            break;

        case 0xA3:
            if (packet->length >= 7u && packet->data[5] < CHRONOS_CONTACTS_SIZE) {
                int pos = packet->data[5];
                size_t wanted = packet->data[6];
                size_t out = 0;
                for (size_t i = 7u; i < packet->length && out < sizeof(core->contacts[pos].number) - 1u && out < wanted; i++) {
                    uint8_t nibbles[] = { (uint8_t)(packet->data[i] & 0x0Fu), (uint8_t)(packet->data[i] >> 4) };
                    for (size_t n = 0; n < 2u && out < sizeof(core->contacts[pos].number) - 1u && out < wanted; n++) {
                        core->contacts[pos].number[out++] = nibbles[n] == 0x0Au ? '+' : (char)('0' + nibbles[n]);
                    }
                }
                core->contacts[pos].number[out] = '\0';
                if (pos == core->contact_size - 1) {
                    set_config_event(target_event, CHRONOS_CONFIG_CONTACT, 1, ((uint32_t)core->sos_contact << 8) | (uint32_t)core->contact_size);
                } else {
                    target_event->type = CHRONOS_EVENT_CONFIG;
                }
            }
            break;

        case 0xA5:
            if (packet->length >= 8u) {
                core->sos_contact = packet->data[6];
                core->contact_size = packet->data[7];
                set_config_event(target_event, CHRONOS_CONFIG_CONTACT, 0, ((uint32_t)core->sos_contact << 8) | (uint32_t)core->contact_size);
            }
            break;

        case 0xA8:
            if (packet->data[3] == CHRONOS_PROTOCOL_EXTENSION_BYTE) {
                set_config_event(target_event, CHRONOS_CONFIG_QR, 1, packet->data[5]);
            } else if (packet->data[3] == CHRONOS_PROTOCOL_STANDARD_BYTE && packet->data[5] < CHRONOS_QR_SIZE) {
                uint8_t index = packet->data[5];
                copy_bytes_to_string(core->qr_links[index], sizeof(core->qr_links[index]), &packet->data[6], packet->length - 6u);
                set_config_event(target_event, CHRONOS_CONFIG_QR, 0, index);
            }
            break;

        case 0xEE:
            if (packet->data[3] == CHRONOS_PROTOCOL_EXTENSION_BYTE && packet->length >= 107u) {
                uint8_t pos = packet->data[6];
                uint32_t crc = read_u32_be(&packet->data[7]);
                size_t offset = (size_t)pos * 96u;
                if (offset + 96u <= CHRONOS_ICON_DATA_SIZE) {
                    memcpy(&core->navigation.icon[offset], &packet->data[11], 96u);
                }
                set_config_event(target_event, CHRONOS_CONFIG_NAV_ICON, pos, crc);
            }
            break;

        default:
            break;
        }
    }

    if (target_event->type == CHRONOS_EVENT_RAW_PACKET &&
        packet->length >= 6u &&
        packet->data[0] == CHRONOS_PREFIX_EA) {
        switch (packet->data[4]) {
        case 0x7E:
            if (packet->data[5] == 0x01u && packet->length >= 7u) {
                copy_bytes_to_string(core->weather_city, sizeof(core->weather_city), &packet->data[7], packet->length - 7u);
                core->weather_time = current_unix_timestamp();
                set_config_event(target_event, CHRONOS_CONFIG_WEATHER, 0, 1);
            } else if (packet->data[5] == 0x02u && packet->length >= 8u) {
                uint8_t size = packet->data[6];
                uint8_t hour = packet->data[7];
                for (uint8_t z = 0; z < size && hour + z < CHRONOS_FORECAST_SIZE; z++) {
                    size_t base = 8u + (6u * z);
                    if (base + 5u >= packet->length) {
                        break;
                    }
                    uint8_t icon_temp = packet->data[base];
                    int sign = (icon_temp & 1u) ? -1 : 1;
                    core->hourly_forecast[hour + z].day = 0;
                    core->hourly_forecast[hour + z].hour = hour + z;
                    core->hourly_forecast[hour + z].icon = icon_temp >> 4;
                    core->hourly_forecast[hour + z].temp = (int)packet->data[base + 1u] * sign;
                    core->hourly_forecast[hour + z].wind = read_u16_be(&packet->data[base + 2u]);
                    core->hourly_forecast[hour + z].humidity = packet->data[base + 4u];
                    core->hourly_forecast[hour + z].uv = packet->data[base + 5u];
                    core->hourly_forecast_valid[hour + z] = true;
                }
                core->weather_time = current_unix_timestamp();
                set_config_event(target_event, CHRONOS_CONFIG_WEATHER, 3, 0);
            }
            break;

        case 0x7F:
            if (packet->data[3] == CHRONOS_PROTOCOL_EXTENSION_BYTE && packet->length >= 16u) {
                uint8_t payload_len = packet->data[6];
                const uint8_t *payload = &packet->data[7];
                if ((size_t)payload_len <= packet->length - 7u && payload_len >= 8u) {
                    memcpy(&core->weather_location.latitude, payload, sizeof(float));
                    memcpy(&core->weather_location.longitude, payload + 4, sizeof(float));
                    size_t index = 8u;
                    size_t start = index;
                    while (index < payload_len && payload[index] != 0u) {
                        index++;
                    }
                    copy_bytes_to_string(core->weather_location.city, sizeof(core->weather_location.city), &payload[start], index - start);
                    if (index < payload_len) {
                        index++;
                    }
                    start = index;
                    while (index < payload_len && payload[index] != 0u) {
                        index++;
                    }
                    copy_bytes_to_string(core->weather_location.region, sizeof(core->weather_location.region), &payload[start], index - start);
                    if (index < payload_len) {
                        index++;
                    }
                    copy_bytes_to_string(core->weather_location.country, sizeof(core->weather_location.country), &payload[index], payload_len - index);
                    core->weather_time = current_unix_timestamp();
                    set_config_event(target_event, CHRONOS_CONFIG_WEATHER, 4, 0);
                }
            }
            break;

        default:
            break;
        }
    }

    switch (target_event->type) {
    case CHRONOS_EVENT_HEALTH_REQUEST:
        if (health_request_callback != NULL) {
            health_request_callback(target_event->value.health.request, target_event->value.health.enabled);
        }
        break;

    case CHRONOS_EVENT_SYNCED:
        target_event->has_config = true;
        target_event->config = CHRONOS_CONFIG_SYNCED;
        target_event->config_a = 0;
        target_event->config_b = 0;
        break;

    case CHRONOS_EVENT_RESET_REQUEST:
        target_event->has_config = true;
        target_event->config = CHRONOS_CONFIG_RESET;
        target_event->config_a = 0;
        target_event->config_b = 0;
        break;

    case CHRONOS_EVENT_PHONE_BATTERY:
        core->phone_info.is_charging = target_event->value.phone_battery.charging;
        core->phone_info.battery_level = target_event->value.phone_battery.level;
        target_event->has_config = true;
        target_event->config = CHRONOS_CONFIG_PHONE_BATTERY;
        target_event->config_a = target_event->value.phone_battery.charging ? 1u : 0u;
        target_event->config_b = target_event->value.phone_battery.level;
        break;

    case CHRONOS_EVENT_NOTIFICATION:
        if (notification_callback != NULL) {
            notification_callback(target_event->value.notification.notification);
        }
        break;

    case CHRONOS_EVENT_RINGER:
        if (ringer_callback != NULL) {
            ringer_callback(target_event->value.ringer.caller, target_event->value.ringer.active);
        }
        break;

    case CHRONOS_EVENT_NAVIGATION:
        break;

    case CHRONOS_EVENT_CHUNKED_TRANSFER:
        core->chunked = target_event->value.chunked_transfer.enabled;
        break;

    case CHRONOS_EVENT_TOUCH:
        core->touch.state = target_event->value.touch.pressed;
        core->touch.x = target_event->value.touch.x;
        core->touch.y = target_event->value.touch.y;
        break;

    case CHRONOS_EVENT_APP_INFO:
        core->phone_info.app_code = target_event->value.app_info.code;
        copy_bytes_to_string(
            core->phone_info.app_version,
            sizeof(core->phone_info.app_version),
            (const uint8_t *)target_event->value.app_info.version,
            target_event->value.app_info.version_length);
        target_event->has_config = true;
        target_event->config = CHRONOS_CONFIG_APP;
        target_event->config_a = core->phone_info.app_code;
        target_event->config_b = 0;
        break;

    case CHRONOS_EVENT_PHONE_INFO:
        core->phone_info.sdk_version = target_event->value.phone_info.sdk_version;
        copy_bytes_to_string(
            core->phone_info.manufacturer,
            sizeof(core->phone_info.manufacturer),
            (const uint8_t *)target_event->value.phone_info.manufacturer,
            target_event->value.phone_info.manufacturer_length);
        copy_bytes_to_string(
            core->phone_info.model,
            sizeof(core->phone_info.model),
            (const uint8_t *)target_event->value.phone_info.model,
            target_event->value.phone_info.model_length);
        target_event->has_config = true;
        target_event->config = CHRONOS_CONFIG_APP;
        target_event->config_a = core->phone_info.sdk_version;
        target_event->config_b = 1;
        break;

    default:
        break;
    }

    return CHRONOS_STATUS_OK;
}

chronos_status_t chronos_core_build_packet(
    uint8_t prefix,
    uint8_t protocol,
    uint8_t command,
    uint8_t subtype,
    const uint8_t *payload,
    size_t payload_length,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    if (out == NULL || out_length == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }
    if (payload_length > 0u && payload == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }
    if (prefix != CHRONOS_PREFIX_AB && prefix != CHRONOS_PREFIX_EA) {
        return CHRONOS_STATUS_INVALID_ARG;
    }
    if (protocol != CHRONOS_PROTOCOL_EXTENSION_BYTE &&
        protocol != CHRONOS_PROTOCOL_STANDARD_BYTE) {
        return CHRONOS_STATUS_INVALID_ARG;
    }

    size_t total = CHRONOS_HEADER_SIZE + payload_length;
    size_t encoded_length = total - CHRONOS_LENGTH_EXTRA;
    if (total > CHRONOS_CORE_MAX_PACKET_SIZE || encoded_length > 0xFFFFu) {
        return CHRONOS_STATUS_OVERFLOW;
    }
    if (out_size < total) {
        return CHRONOS_STATUS_OUT_TOO_SMALL;
    }

    out[0] = prefix;
    write_u16_be(&out[1], (uint16_t)encoded_length);
    out[3] = protocol;
    out[4] = command;
    out[5] = subtype;
    if (payload_length > 0u) {
        memcpy(&out[6], payload, payload_length);
    }

    *out_length = total;
    return CHRONOS_STATUS_OK;
}

chronos_status_t chronos_core_build_battery(
    uint8_t level,
    bool charging,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { charging ? 1u : 0u, level };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x91u, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_sync_request(
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_EXTENSION_BYTE, 0x23u, 0x80u,
        NULL, 0u, out, out_size, out_length);
}

chronos_status_t chronos_core_build_notify_battery(
    bool enabled,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { enabled ? 1u : 0u };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_EXTENSION_BYTE, 0x91u, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_music_control(
    chronos_control_t command,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint16_t control = (uint16_t)command;
    uint8_t payload[] = { (uint8_t)control };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, (uint8_t)(control >> 8), 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_realtime_steps(
    uint32_t steps,
    uint32_t calories,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    steps = pack_u24(steps);
    calories = pack_u24(calories);

    uint8_t payload[] = {
        (uint8_t)(steps >> 16), (uint8_t)(steps >> 8), (uint8_t)steps,
        (uint8_t)(calories >> 16), (uint8_t)(calories >> 8), (uint8_t)calories,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x51u, 0x08u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_info(
    uint8_t screen,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = {
        (uint8_t)CHRONOS_CORE_VERSION_MAJOR,
        (uint8_t)(CHRONOS_CORE_VERSION_MINOR * 10u + CHRONOS_CORE_VERSION_PATCH),
        0x00u, 0xFBu, 0x1Eu, 0x40u, 0xC0u, 0x0Eu, 0x32u, 0x28u,
        0x00u, 0xE2u, screen, 0x80u
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x92u, 0xC0u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_system_info(
    const char *info,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    if (info == NULL) {
        return CHRONOS_STATUS_INVALID_ARG;
    }

    size_t info_length = strlen(info);
    if (info_length > 505u) {
        info_length = 505u;
    }

    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_EXTENSION_BYTE, 0x92u, 0x80u,
        (const uint8_t *)info, info_length, out, out_size, out_length);
}

chronos_status_t chronos_core_build_esp_info(
    const char *info,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    return chronos_core_build_system_info(info, out, out_size, out_length);
}

chronos_status_t chronos_core_build_volume(
    uint8_t level,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { 0xA0u, level };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x99u, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_capture_photo(
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { 0x01u };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x79u, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_find_phone(
    bool enabled,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { enabled ? 1u : 0u };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x7Du, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_realtime_heart_rate(
    uint8_t heart_rate,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { heart_rate, 0x1Bu };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x31u, 0x0Au,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_realtime_blood_pressure(
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { systolic, diastolic };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x31u, 0x22u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_realtime_blood_oxygen(
    uint8_t blood_oxygen,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { blood_oxygen, 0x30u };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x31u, 0x12u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_realtime_health_data(
    uint8_t heart_rate,
    uint8_t blood_oxygen,
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = { heart_rate, blood_oxygen, systolic, diastolic };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x32u, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_steps_record(
    uint32_t steps,
    uint32_t calories,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t heart_rate,
    uint8_t blood_oxygen,
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    steps = pack_u24(steps);
    calories = pack_u24(calories);
    uint8_t payload[] = {
        year_since_2000(year), month, day, hour,
        (uint8_t)(steps >> 16), (uint8_t)(steps >> 8), (uint8_t)steps,
        (uint8_t)(calories >> 16), (uint8_t)(calories >> 8), (uint8_t)calories,
        heart_rate, blood_oxygen, systolic, diastolic,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x51u, 0x20u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_heart_rate_record(
    uint8_t heart_rate,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = {
        year_since_2000(year), month, day, hour, minute,
        heart_rate, 0x00u
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x51u, 0x11u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_blood_pressure_record(
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = {
        year_since_2000(year), month, day, hour, minute,
        systolic, diastolic
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x51u, 0x14u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_blood_oxygen_record(
    uint8_t blood_oxygen,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = {
        year_since_2000(year), month, day, hour, minute,
        blood_oxygen, 0x00u
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x51u, 0x12u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_sleep_record(
    uint16_t sleep_time,
    chronos_sleep_type_t type,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = {
        year_since_2000(year), month, day, hour, minute,
        (uint8_t)type, (uint8_t)(sleep_time >> 8), (uint8_t)sleep_time
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x52u, 0x80u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_build_temperature_record(
    float temperature,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length)
{
    uint8_t payload[] = {
        year_since_2000(year), month, day, hour, minute,
        (uint8_t)temperature, (uint8_t)((uint16_t)(temperature * 100.0f) % 100u)
    };
    return chronos_core_build_packet(
        CHRONOS_PREFIX_AB, CHRONOS_PROTOCOL_STANDARD_BYTE, 0x51u, 0x13u,
        payload, sizeof(payload), out, out_size, out_length);
}

chronos_status_t chronos_core_send_battery(uint8_t level, bool charging)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_battery(level, charging, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_sync_request(void)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_sync_request(packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_notify_battery(bool enabled)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    default_core.notify_phone = enabled;
    return send_built_packet(
        chronos_core_build_notify_battery(enabled, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_music_control(chronos_control_t command)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_music_control(command, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_realtime_steps(uint32_t steps, uint32_t calories)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_realtime_steps(steps, calories, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_info(uint8_t screen)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_info(screen, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_system_info(const char *info)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_system_info(info, packet, sizeof(packet), &length),
        packet, length, true);
}

chronos_status_t chronos_core_send_esp_info(const char *info)
{
    return chronos_core_send_system_info(info);
}

chronos_status_t chronos_core_send_volume(uint8_t level)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_volume(level, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_capture_photo(void)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_capture_photo(packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_find_phone(bool enabled)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_find_phone(enabled, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_realtime_heart_rate(uint8_t heart_rate)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_realtime_heart_rate(heart_rate, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_realtime_blood_pressure(uint8_t systolic, uint8_t diastolic)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_realtime_blood_pressure(systolic, diastolic, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_realtime_blood_oxygen(uint8_t blood_oxygen)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_realtime_blood_oxygen(blood_oxygen, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_realtime_health_data(
    uint8_t heart_rate,
    uint8_t blood_oxygen,
    uint8_t systolic,
    uint8_t diastolic)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_realtime_health_data(heart_rate, blood_oxygen, systolic, diastolic, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_steps_record(
    uint32_t steps,
    uint32_t calories,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t heart_rate,
    uint8_t blood_oxygen,
    uint8_t systolic,
    uint8_t diastolic)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_steps_record(steps, calories, hour, day, month, year, heart_rate, blood_oxygen, systolic, diastolic, packet, sizeof(packet), &length),
        packet, length, true);
}

chronos_status_t chronos_core_send_heart_rate_record(
    uint8_t heart_rate,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_heart_rate_record(heart_rate, minute, hour, day, month, year, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_blood_pressure_record(
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_blood_pressure_record(systolic, diastolic, minute, hour, day, month, year, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_blood_oxygen_record(
    uint8_t blood_oxygen,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_blood_oxygen_record(blood_oxygen, minute, hour, day, month, year, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_sleep_record(
    uint16_t sleep_time,
    chronos_sleep_type_t type,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_sleep_record(sleep_time, type, minute, hour, day, month, year, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_send_temperature_record(
    float temperature,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year)
{
    uint8_t packet[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t length = 0;
    return send_built_packet(
        chronos_core_build_temperature_record(temperature, minute, hour, day, month, year, packet, sizeof(packet), &length),
        packet, length, false);
}

chronos_status_t chronos_core_tx_begin(
    chronos_tx_t *tx,
    const uint8_t *packet,
    size_t packet_length,
    bool chunked)
{
    if (tx == NULL || packet == NULL || packet_length == 0u) {
        return CHRONOS_STATUS_INVALID_ARG;
    }

    tx->packet = packet;
    tx->packet_length = packet_length;
    tx->offset = 0;
    tx->sequence = 0;
    tx->chunked = chunked && packet_length > CHRONOS_CORE_BLE_CHUNK_SIZE;
    tx->active = true;

    return CHRONOS_STATUS_OK;
}

bool chronos_core_tx_next(
    chronos_tx_t *tx,
    uint8_t *chunk,
    size_t chunk_size,
    size_t *chunk_length)
{
    if (tx == NULL || chunk == NULL || chunk_length == NULL || !tx->active) {
        return false;
    }

    *chunk_length = 0;

    if (!tx->chunked) {
        if (chunk_size < tx->packet_length) {
            tx->active = false;
            return false;
        }
        memcpy(chunk, tx->packet, tx->packet_length);
        *chunk_length = tx->packet_length;
        tx->active = false;
        return true;
    }

    if (tx->offset == 0u) {
        if (chunk_size < CHRONOS_CORE_BLE_CHUNK_SIZE) {
            tx->active = false;
            return false;
        }
        memcpy(chunk, tx->packet, CHRONOS_CORE_BLE_CHUNK_SIZE);
        *chunk_length = CHRONOS_CORE_BLE_CHUNK_SIZE;
        tx->offset = CHRONOS_CORE_BLE_CHUNK_SIZE;
        return true;
    }

    if (tx->offset >= tx->packet_length) {
        tx->active = false;
        return false;
    }

    size_t max_payload = CHRONOS_CORE_BLE_CHUNK_SIZE - 1u;
    size_t remaining = tx->packet_length - tx->offset;
    size_t payload_length = remaining < max_payload ? remaining : max_payload;
    if (chunk_size < payload_length + 1u) {
        tx->active = false;
        return false;
    }

    chunk[0] = tx->sequence++;
    memcpy(&chunk[1], &tx->packet[tx->offset], payload_length);
    *chunk_length = payload_length + 1u;
    tx->offset += payload_length;

    if (tx->offset >= tx->packet_length) {
        tx->active = false;
    }

    return true;
}
