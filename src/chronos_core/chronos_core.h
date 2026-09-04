#ifndef CHRONOS_CORE_H
#define CHRONOS_CORE_H

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

#ifndef CHRONOS_CORE_MAX_PACKET_SIZE
#define CHRONOS_CORE_MAX_PACKET_SIZE 512u
#endif

#ifndef CHRONOS_CORE_BLE_CHUNK_SIZE
#define CHRONOS_CORE_BLE_CHUNK_SIZE 20u
#endif

#ifndef CHRONOS_NOTIF_SIZE
#define CHRONOS_NOTIF_SIZE 10u
#endif

#ifndef CHRONOS_CORE_VERSION_MAJOR
#define CHRONOS_CORE_VERSION_MAJOR 1u
#endif

#ifndef CHRONOS_CORE_VERSION_MINOR
#define CHRONOS_CORE_VERSION_MINOR 0u
#endif

#ifndef CHRONOS_CORE_VERSION_PATCH
#define CHRONOS_CORE_VERSION_PATCH 0u
#endif

#define CHRONOS_WEATHER_SIZE 7u
#define CHRONOS_ALARM_SIZE 8u
#define CHRONOS_FORECAST_SIZE 24u
#define CHRONOS_QR_SIZE 9u
#define CHRONOS_ICON_SIZE 48u
#define CHRONOS_ICON_DATA_SIZE ((CHRONOS_ICON_SIZE * CHRONOS_ICON_SIZE) / 8u)
#define CHRONOS_CONTACTS_SIZE 255u

#ifndef CHRONOS_TEXT_SMALL_SIZE
#define CHRONOS_TEXT_SMALL_SIZE 32u
#endif

#ifndef CHRONOS_TEXT_MEDIUM_SIZE
#define CHRONOS_TEXT_MEDIUM_SIZE 96u
#endif

#ifndef CHRONOS_TEXT_LARGE_SIZE
#define CHRONOS_TEXT_LARGE_SIZE 256u
#endif

#ifndef CHRONOS_TEXT_XLARGE_SIZE
#define CHRONOS_TEXT_XLARGE_SIZE 496u
#endif

#define CHRONOS_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHRONOS_CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHRONOS_CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    CHRONOS_STATUS_OK = 0,
    CHRONOS_STATUS_PENDING,
    CHRONOS_STATUS_PACKET_READY,
    CHRONOS_STATUS_INVALID_ARG,
    CHRONOS_STATUS_INVALID_PACKET,
    CHRONOS_STATUS_OVERFLOW,
    CHRONOS_STATUS_OUT_TOO_SMALL,
    CHRONOS_STATUS_NO_ACTIVE_PACKET
} chronos_status_t;

typedef enum {
    CHRONOS_PROTOCOL_EXTENSION = 0xFE,
    CHRONOS_PROTOCOL_STANDARD = 0xFF
} chronos_protocol_t;

typedef enum {
    CHRONOS_EVENT_UNKNOWN = 0,
    CHRONOS_EVENT_RAW_PACKET,
    CHRONOS_EVENT_SYNCED,
    CHRONOS_EVENT_RESET_REQUEST,
    CHRONOS_EVENT_CONFIG,
    CHRONOS_EVENT_HEALTH_REQUEST,
    CHRONOS_EVENT_NOTIFICATION,
    CHRONOS_EVENT_RINGER,
    CHRONOS_EVENT_NAVIGATION,
    CHRONOS_EVENT_PHONE_BATTERY,
    CHRONOS_EVENT_CHUNKED_TRANSFER,
    CHRONOS_EVENT_TOUCH,
    CHRONOS_EVENT_APP_INFO,
    CHRONOS_EVENT_PHONE_INFO
} chronos_event_type_t;

typedef enum {
    CHRONOS_HEALTH_STEPS_RECORDS = 0,
    CHRONOS_HEALTH_SLEEP_RECORDS,
    CHRONOS_HEALTH_HEART_RATE_MEASURE,
    CHRONOS_HEALTH_BLOOD_OXYGEN_MEASURE,
    CHRONOS_HEALTH_BLOOD_PRESSURE_MEASURE,
    CHRONOS_HEALTH_MEASURE_ALL
} chronos_health_request_t;

typedef enum {
    CHRONOS_CONFIG_TIME = 0,
    CHRONOS_CONFIG_RAISE_TO_WAKE,
    CHRONOS_CONFIG_24_HOUR,
    CHRONOS_CONFIG_LANGUAGE,
    CHRONOS_CONFIG_RESET,
    CHRONOS_CONFIG_CLEAR_DATA,
    CHRONOS_CONFIG_HOURLY,
    CHRONOS_CONFIG_FIND,
    CHRONOS_CONFIG_USER,
    CHRONOS_CONFIG_ALARM,
    CHRONOS_CONFIG_FONT,
    CHRONOS_CONFIG_SEDENTARY,
    CHRONOS_CONFIG_SLEEP,
    CHRONOS_CONFIG_QUIET,
    CHRONOS_CONFIG_WATER,
    CHRONOS_CONFIG_WEATHER,
    CHRONOS_CONFIG_CAMERA,
    CHRONOS_CONFIG_PHONE_BATTERY,
    CHRONOS_CONFIG_APP,
    CHRONOS_CONFIG_QR,
    CHRONOS_CONFIG_NAV_DATA,
    CHRONOS_CONFIG_NAV_ICON,
    CHRONOS_CONFIG_CONTACT,
    CHRONOS_CONFIG_SYNCED,
    CHRONOS_CONFIG_MUSIC,
    CHRONOS_CONFIG_DEVICE_INFO_REQUEST
} chronos_config_t;

typedef enum {
    CHRONOS_CONTROL_MUSIC_PLAY = 0x9D00,
    CHRONOS_CONTROL_MUSIC_PAUSE = 0x9D01,
    CHRONOS_CONTROL_MUSIC_PREVIOUS = 0x9D02,
    CHRONOS_CONTROL_MUSIC_NEXT = 0x9D03,
    CHRONOS_CONTROL_MUSIC_TOGGLE = 0x9900,
    CHRONOS_CONTROL_VOLUME_UP = 0x99A1,
    CHRONOS_CONTROL_VOLUME_DOWN = 0x99A2,
    CHRONOS_CONTROL_VOLUME_MUTE = 0x99A3
} chronos_control_t;

typedef void (*chronos_packet_callback_t)(uint8_t *data, size_t length);
typedef void (*chronos_connection_callback_t)(bool connected);
typedef void (*chronos_ringer_callback_t)(const char *caller, bool active);
typedef void (*chronos_health_request_callback_t)(chronos_health_request_t request, bool active);
typedef uint32_t (*chronos_time_callback_t)(void);

typedef enum {
    CHRONOS_SLEEP_AWAKE = 0,
    CHRONOS_SLEEP_LIGHT = 1,
    CHRONOS_SLEEP_DEEP = 2
} chronos_sleep_type_t;

typedef struct {
    int icon;
    char app[CHRONOS_TEXT_SMALL_SIZE];
    uint32_t timestamp;
    char title[CHRONOS_TEXT_MEDIUM_SIZE];
    char message[CHRONOS_TEXT_XLARGE_SIZE];
} chronos_notification_t;

typedef void (*chronos_notification_callback_t)(chronos_notification_t *notification);

typedef struct {
    int icon;
    int day;
    int temp;
    int high;
    int low;
    int pressure;
    int uv;
} chronos_weather_t;

typedef struct {
    char city[CHRONOS_TEXT_MEDIUM_SIZE];
    char region[CHRONOS_TEXT_MEDIUM_SIZE];
    char country[CHRONOS_TEXT_MEDIUM_SIZE];
    float latitude;
    float longitude;
} chronos_weather_location_t;

typedef struct {
    int day;
    int hour;
    int icon;
    int temp;
    int uv;
    int humidity;
    int wind;
} chronos_hourly_forecast_t;

typedef struct {
    unsigned long time;
    long duration;
    bool active;
} chronos_timer_t;

typedef struct {
    uint8_t *data;
    size_t length;
} chronos_packet_t;

typedef struct {
    const uint8_t *data;
    size_t length;
} chronos_const_packet_t;

typedef void (*chronos_config_callback_t)(chronos_config_t config, uint32_t a, uint32_t b);

typedef struct {
    int length;
    uint8_t data[CHRONOS_CORE_MAX_PACKET_SIZE];
} chronos_data_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t repeat;
    bool enabled;
} chronos_alarm_t;

typedef chronos_alarm_t chronos_setting_t;

typedef struct {
    bool state;
    uint32_t x;
    uint32_t y;
} chronos_remote_touch_t;

typedef struct {
    bool active;
    bool is_navigation;
    bool has_icon;
    char distance[CHRONOS_TEXT_SMALL_SIZE];
    char duration[CHRONOS_TEXT_SMALL_SIZE];
    char eta[CHRONOS_TEXT_SMALL_SIZE];
    char title[CHRONOS_TEXT_MEDIUM_SIZE];
    char directions[CHRONOS_TEXT_LARGE_SIZE];
    char speed[CHRONOS_TEXT_SMALL_SIZE];
    uint8_t icon[CHRONOS_ICON_DATA_SIZE];
    uint32_t icon_crc;
} chronos_navigation_t;

typedef struct {
    char name[CHRONOS_TEXT_MEDIUM_SIZE];
    char number[CHRONOS_TEXT_SMALL_SIZE];
} chronos_contact_t;

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} chronos_date_time_t;

typedef struct {
    bool is_charging;
    uint8_t battery_level;
    int app_code;
    int sdk_version;
    char app_version[CHRONOS_TEXT_SMALL_SIZE];
    char manufacturer[CHRONOS_TEXT_MEDIUM_SIZE];
    char model[CHRONOS_TEXT_MEDIUM_SIZE];
} chronos_phone_info_t;

typedef struct {
    uint8_t state;
    uint32_t text_color;
    uint32_t background_color;
    char title[CHRONOS_TEXT_LARGE_SIZE];
    char artist[CHRONOS_TEXT_MEDIUM_SIZE];
    char app_name[CHRONOS_TEXT_MEDIUM_SIZE];
    char package_name[CHRONOS_TEXT_MEDIUM_SIZE];
} chronos_music_info_t;

typedef struct {
    uint8_t data[CHRONOS_CORE_MAX_PACKET_SIZE];
    size_t expected_length;
    size_t received_length;
    bool active;

    bool connected;
    bool subscribed;
    bool hour_24;
    bool camera_ready;
    bool quiet_enabled;
    bool sleep_enabled;
    uint16_t quiet_start;
    uint16_t quiet_end;
    uint16_t sleep_start;
    uint16_t sleep_end;
    chronos_date_time_t date_time;
    bool notify_phone;
    bool chunked;

    chronos_notification_t notifications[CHRONOS_NOTIF_SIZE];
    int notification_index;
    chronos_weather_t weather[CHRONOS_WEATHER_SIZE];
    int weather_size;
    char weather_city[CHRONOS_TEXT_MEDIUM_SIZE];
    uint32_t weather_time;
    chronos_weather_location_t weather_location;
    chronos_hourly_forecast_t hourly_forecast[CHRONOS_FORECAST_SIZE];
    bool hourly_forecast_valid[CHRONOS_FORECAST_SIZE];
    chronos_remote_touch_t touch;
    chronos_alarm_t alarms[CHRONOS_ALARM_SIZE];
    char qr_links[CHRONOS_QR_SIZE][CHRONOS_TEXT_LARGE_SIZE];
    char ringer_caller[CHRONOS_TEXT_LARGE_SIZE];
    chronos_contact_t contacts[CHRONOS_CONTACTS_SIZE];
    int sos_contact;
    int contact_size;
    chronos_data_t incoming_data;
    chronos_data_t outgoing_data;
    chronos_navigation_t navigation;
    chronos_phone_info_t phone_info;
    chronos_music_info_t music_info;
} chronos_core_t;

typedef struct {
    const uint8_t *packet;
    size_t packet_length;
    size_t offset;
    uint8_t sequence;
    bool chunked;
    bool active;
} chronos_tx_t;

typedef struct {
    chronos_event_type_t type;
    uint8_t prefix;
    uint8_t protocol;
    uint8_t command;
    uint8_t subtype;
    const uint8_t *payload;
    size_t payload_length;
    bool has_config;
    chronos_config_t config;
    uint32_t config_a;
    uint32_t config_b;

    union {
        struct {
            chronos_health_request_t request;
            bool enabled;
        } health;

        struct {
            chronos_notification_t *notification;
        } notification;

        struct {
            const char *caller;
            bool active;
        } ringer;

        struct {
            bool charging;
            uint8_t level;
        } phone_battery;

        struct {
            bool enabled;
        } chunked_transfer;

        struct {
            bool pressed;
            uint16_t x;
            uint16_t y;
        } touch;

        struct {
            uint16_t code;
            const char *version;
            size_t version_length;
        } app_info;

        struct {
            uint16_t sdk_version;
            const char *manufacturer;
            size_t manufacturer_length;
            const char *model;
            size_t model_length;
        } phone_info;

    } value;
} chronos_event_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 * LIFECYCLE AND STATE
 **********************/

/* Initialize the default global core state and seed default values. */
void chronos_core_init(void);

/* Return the default global core state for advanced integrations. */
chronos_core_t *chronos_core_get_state(void);

/* Initialize a caller-owned core context without touching global callbacks. */
void chronos_core_context_init(chronos_core_t *core);

/* Reset only the receive assembly state for a caller-owned context. */
void chronos_core_reset_rx(chronos_core_t *core);

/**********************
 * CALLBACKS AND TIME SOURCE
 **********************/

/* Set the transport callback used by chronos_core_send_* wrappers. */
void chronos_core_set_tx_cb(chronos_packet_callback_t callback);

/* Set the callback fired when injected connection state changes. */
void chronos_core_set_connection_cb(chronos_connection_callback_t callback);

/* Set the callback fired when a notification packet is decoded and stored. */
void chronos_core_set_notification_cb(chronos_notification_callback_t callback);

/* Set the callback fired for incoming call/ringer packets. */
void chronos_core_set_ringer_cb(chronos_ringer_callback_t callback);

/* Set the callback fired for every raw BLE RX chunk before assembly. */
void chronos_core_set_raw_rx_cb(chronos_packet_callback_t callback);

/* Set the callback fired for each complete assembled packet. */
void chronos_core_set_data_cb(chronos_packet_callback_t callback);

/* Alias for chronos_core_set_data_cb(); kept for naming clarity. */
void chronos_core_set_rx_packet_cb(chronos_packet_callback_t callback);

/* Set the callback fired for decoded configuration/state updates. */
void chronos_core_set_config_cb(chronos_config_callback_t callback);

/* Set the callback fired for decoded health measurement/record requests. */
void chronos_core_set_health_request_cb(chronos_health_request_callback_t callback);

/* Set the platform Unix-time callback used for notification and weather timestamps. */
void chronos_core_set_time_cb(chronos_time_callback_t callback);

/**********************
 * PLATFORM CONNECTION STATE
 **********************/

/* Inject the current BLE connection state; fires the connection callback on change. */
void chronos_core_set_connected(bool connected);

/* Inject the current BLE subscription/notification state. */
void chronos_core_set_subscribed(bool subscribed);

/* Inject connection and subscription state together; fires on connection changes. */
void chronos_core_set_connection_status(bool connected, bool subscribed);

/* Return whether the platform has marked the default core as connected. */
bool chronos_core_is_connected(void);

/* Return whether the platform has marked the default core as subscribed. */
bool chronos_core_is_subscribed(void);

/**********************
 * SETTINGS GETTERS
 **********************/

/* Return whether outgoing sends use Chronos chunked transfer mode. */
bool chronos_core_is_chunked(void);

/* Return true when 24-hour time display is enabled. */
bool chronos_core_is_24_hour(void);

/* Return true when the phone camera feature is ready/active. */
bool chronos_core_is_camera_ready(void);

/* Return true when quiet hours are enabled. */
bool chronos_core_is_quiet_enabled(void);

/* Return true when sleep time is enabled. */
bool chronos_core_is_sleep_enabled(void);

/* Return packed quiet-hours start time as hour/minute in the high/low bytes. */
uint16_t chronos_core_get_quiet_start(void);

/* Return packed quiet-hours end time as hour/minute in the high/low bytes. */
uint16_t chronos_core_get_quiet_end(void);

/* Return packed sleep start time as hour/minute in the high/low bytes. */
uint16_t chronos_core_get_sleep_start(void);

/* Return packed sleep end time as hour/minute in the high/low bytes. */
uint16_t chronos_core_get_sleep_end(void);

/* Return the last date/time received from Chronos time sync packets. */
const chronos_date_time_t *chronos_core_get_date_time(void);

/**********************
 * PHONE AND APP GETTERS
 **********************/

/* Return true when the connected phone reports charging. */
bool chronos_core_is_phone_charging(void);

/* Return the last connected phone battery level, 0-100. */
uint8_t chronos_core_get_phone_battery(void);

/* Return the Chronos app code reported by the phone. */
int chronos_core_get_app_code(void);

/* Return the phone SDK/API version reported by the phone. */
int chronos_core_get_sdk_version(void);

/* Return the Chronos app version string stored in the default core. */
const char *chronos_core_get_app_version(void);

/* Return the phone manufacturer string stored in the default core. */
const char *chronos_core_get_phone_manufacturer(void);

/* Return the phone model string stored in the default core. */
const char *chronos_core_get_phone_model(void);

/* Return the full stored phone/app info struct. */
const chronos_phone_info_t *chronos_core_get_phone_info(void);

/**********************
 * FEATURE STATE GETTERS
 **********************/

/* Return the latest remote touch state. */
const chronos_remote_touch_t *chronos_core_get_touch(void);

/* Return the latest navigation state, including any received icon data. */
const chronos_navigation_t *chronos_core_get_navigation(void);

/* Return the latest music/player state. */
const chronos_music_info_t *chronos_core_get_music_info(void);

/* Return the number of notifications retained in the circular buffer. */
int chronos_core_get_notification_count(void);

/* Return notification by recency, where index 0 is the latest; NULL if invalid. */
const chronos_notification_t *chronos_core_get_notification_at(int index);

/* Return the number of daily weather entries currently stored. */
int chronos_core_get_weather_count(void);

/* Return a daily weather entry by index; NULL if out of range. */
const chronos_weather_t *chronos_core_get_weather_at(int index);

/* Return the last weather city string received from the phone. */
const char *chronos_core_get_weather_city(void);

/* Return the Unix timestamp for the last stored weather update, or 0 if unknown. */
uint32_t chronos_core_get_weather_time(void);

/* Return the last weather location metadata received from the phone. */
const chronos_weather_location_t *chronos_core_get_weather_location(void);

/* Return an hourly forecast by hour index 0-23; NULL if out of range. */
const chronos_hourly_forecast_t *chronos_core_get_forecast_hour(int hour);

/* Return true when an hourly forecast slot has been received from the phone. */
bool chronos_core_has_forecast_hour(int hour);

/* Return an alarm by index; NULL if out of range. */
const chronos_alarm_t *chronos_core_get_alarm(int index);

/* Return a QR link by index; NULL if out of range. */
const char *chronos_core_get_qr_at(int index);

/* Return a contact by index; NULL if out of range. */
const chronos_contact_t *chronos_core_get_contact(int index);

/* Return the number of contacts reported by the phone. */
int chronos_core_get_contact_count(void);

/* Return the current SOS contact index. */
int chronos_core_get_sos_contact_index(void);

/**********************
 * RECEIVE AND EVENT HANDLING
 **********************/

/* Receive a BLE RX chunk into the default core and dispatch callbacks as needed. */
chronos_status_t chronos_core_receive_ble_packet(
    uint8_t *data,
    size_t length);

/* Return true if data begins with a supported Chronos packet prefix. */
bool chronos_core_is_packet_start(const uint8_t *data, size_t length);

/* Return the full expected packet length from a packet header, or 0 if invalid. */
size_t chronos_core_expected_length(const uint8_t *data, size_t length);

/* Feed bytes into a caller-owned RX assembler; emits a complete packet when ready. */
chronos_status_t chronos_core_receive(
    chronos_core_t *core,
    const uint8_t *data,
    size_t length,
    chronos_packet_t *packet);

/* Parse a complete packet into a lightweight event without mutating core state. */
chronos_status_t chronos_core_parse_event(
    const chronos_const_packet_t *packet,
    chronos_event_t *event);

/* Parse and apply a complete packet to a caller-owned core context. */
chronos_status_t chronos_core_handle_packet(
    chronos_core_t *core,
    const chronos_const_packet_t *packet,
    chronos_event_t *event);

/**********************
 * PACKET BUILDERS
 **********************/

/* Build a raw Chronos packet with the provided header fields and payload. */
chronos_status_t chronos_core_build_packet(
    uint8_t prefix,
    uint8_t protocol,
    uint8_t command,
    uint8_t subtype,
    const uint8_t *payload,
    size_t payload_length,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a phone/watch battery status packet. */
chronos_status_t chronos_core_build_battery(
    uint8_t level,
    bool charging,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a sync request packet for asking the phone to resend state/time. */
chronos_status_t chronos_core_build_sync_request(
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a packet enabling or disabling phone battery notifications. */
chronos_status_t chronos_core_build_notify_battery(
    bool enabled,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a music/volume control packet using a Chronos control command value. */
chronos_status_t chronos_core_build_music_control(
    chronos_control_t command,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a realtime steps/calories health packet. */
chronos_status_t chronos_core_build_realtime_steps(
    uint32_t steps,
    uint32_t calories,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a generic firmware/app info packet using CHRONOS_CORE_VERSION_* defines. */
chronos_status_t chronos_core_build_info(
    uint8_t screen,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a system info packet with caller-provided platform/device info text. */
chronos_status_t chronos_core_build_system_info(
    const char *info,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Compatibility alias for chronos_core_build_system_info(). */
chronos_status_t chronos_core_build_esp_info(
    const char *info,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build an absolute media volume packet; level is 0-100. */
chronos_status_t chronos_core_build_volume(
    uint8_t level,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a camera capture command packet. */
chronos_status_t chronos_core_build_capture_photo(
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a find-phone command packet. */
chronos_status_t chronos_core_build_find_phone(
    bool enabled,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a realtime heart-rate measurement packet. */
chronos_status_t chronos_core_build_realtime_heart_rate(
    uint8_t heart_rate,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a realtime blood-pressure measurement packet. */
chronos_status_t chronos_core_build_realtime_blood_pressure(
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a realtime blood-oxygen measurement packet. */
chronos_status_t chronos_core_build_realtime_blood_oxygen(
    uint8_t blood_oxygen,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a combined realtime health measurement packet. */
chronos_status_t chronos_core_build_realtime_health_data(
    uint8_t heart_rate,
    uint8_t blood_oxygen,
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a historical steps record packet. */
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
    size_t *out_length);

/* Build a historical heart-rate record packet. */
chronos_status_t chronos_core_build_heart_rate_record(
    uint8_t heart_rate,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a historical blood-pressure record packet. */
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
    size_t *out_length);

/* Build a historical blood-oxygen record packet. */
chronos_status_t chronos_core_build_blood_oxygen_record(
    uint8_t blood_oxygen,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/* Build a historical sleep record packet; sleep_time is the duration in minutes. */
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
    size_t *out_length);

/* Build a historical temperature record packet. */
chronos_status_t chronos_core_build_temperature_record(
    float temperature,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year,
    uint8_t *out,
    size_t out_size,
    size_t *out_length);

/**********************
 * SEND WRAPPERS
 **********************/

/* Send a caller-provided packet through the TX callback, optionally chunked. */
chronos_status_t chronos_core_send_raw_packet(
    uint8_t *data,
    size_t length,
    bool chunked);

/* Build and send a battery status packet. */
chronos_status_t chronos_core_send_battery(uint8_t level, bool charging);

/* Build and send a sync request packet. */
chronos_status_t chronos_core_send_sync_request(void);

/* Build and send a phone battery notification preference packet. */
chronos_status_t chronos_core_send_notify_battery(bool enabled);

/* Build and send a music/volume control packet. */
chronos_status_t chronos_core_send_music_control(chronos_control_t command);

/* Build and send realtime steps/calories data. */
chronos_status_t chronos_core_send_realtime_steps(uint32_t steps, uint32_t calories);

/* Build and send generic firmware/app info using CHRONOS_CORE_VERSION_* defines. */
chronos_status_t chronos_core_send_info(uint8_t screen);

/* Build and send system/platform info text. */
chronos_status_t chronos_core_send_system_info(const char *info);

/* Compatibility alias for chronos_core_send_system_info(). */
chronos_status_t chronos_core_send_esp_info(const char *info);

/* Build and send an absolute volume level packet; level is 0-100. */
chronos_status_t chronos_core_send_volume(uint8_t level);

/* Build and send a camera capture command. */
chronos_status_t chronos_core_send_capture_photo(void);

/* Build and send a find-phone command. */
chronos_status_t chronos_core_send_find_phone(bool enabled);

/* Build and send realtime heart-rate data. */
chronos_status_t chronos_core_send_realtime_heart_rate(uint8_t heart_rate);

/* Build and send realtime blood-pressure data. */
chronos_status_t chronos_core_send_realtime_blood_pressure(uint8_t systolic, uint8_t diastolic);

/* Build and send realtime blood-oxygen data. */
chronos_status_t chronos_core_send_realtime_blood_oxygen(uint8_t blood_oxygen);

/* Build and send combined realtime health data. */
chronos_status_t chronos_core_send_realtime_health_data(
    uint8_t heart_rate,
    uint8_t blood_oxygen,
    uint8_t systolic,
    uint8_t diastolic);

/* Build and send a historical steps record. */
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
    uint8_t diastolic);

/* Build and send a historical heart-rate record. */
chronos_status_t chronos_core_send_heart_rate_record(
    uint8_t heart_rate,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year);

/* Build and send a historical blood-pressure record. */
chronos_status_t chronos_core_send_blood_pressure_record(
    uint8_t systolic,
    uint8_t diastolic,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year);

/* Build and send a historical blood-oxygen record. */
chronos_status_t chronos_core_send_blood_oxygen_record(
    uint8_t blood_oxygen,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year);

/* Build and send a historical sleep record. */
chronos_status_t chronos_core_send_sleep_record(
    uint16_t sleep_time,
    chronos_sleep_type_t type,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year);

/* Build and send a historical temperature record. */
chronos_status_t chronos_core_send_temperature_record(
    float temperature,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint32_t year);

/**********************
 * TRANSMIT CHUNKING
 **********************/

/* Begin iterating packet chunks for a caller-managed TX operation. */
chronos_status_t chronos_core_tx_begin(
    chronos_tx_t *tx,
    const uint8_t *packet,
    size_t packet_length,
    bool chunked);

/* Write the next TX chunk; returns false after the final chunk is emitted. */
bool chronos_core_tx_next(
    chronos_tx_t *tx,
    uint8_t *chunk,
    size_t chunk_size,
    size_t *chunk_length);

#ifdef __cplusplus
}
#endif

#endif
