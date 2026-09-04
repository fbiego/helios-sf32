#include "chronos_core.h"

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void on_tx(uint8_t *data, size_t length)
{
    printf("tx %zu bytes:", length);
    for (size_t i = 0; i < length; i++) {
        printf(" %02X", data[i]);
    }
    printf("\n");
}

static void on_connection(bool connected)
{
    printf("connection: %s\n", connected ? "connected" : "disconnected");
}

static void on_notification(chronos_notification_t *notification)
{
    printf("notification: [%s] %s - %s\n",
           notification->app,
           notification->title,
           notification->message);
}

static void on_ringer(const char *caller, bool active)
{
    printf("ringer: %s %s\n", caller, active ? "started" : "stopped");
}

static void on_config(chronos_config_t config, void *data)
{
    chronos_config_data_t *config_data = (chronos_config_data_t *)data;

    if (config == CHRONOS_CONFIG_PHONE_BATTERY) {
        chronos_phone_info_t *phone = (chronos_phone_info_t *)config_data->data;
        printf("phone battery: %u%% charging=%s\n",
               phone->battery_level,
               phone->is_charging ? "true" : "false");
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(void)
{
    chronos_core_init();
    chronos_core_set_tx_cb(on_tx);
    chronos_core_set_connection_cb(on_connection);
    chronos_core_set_notification_cb(on_notification);
    chronos_core_set_ringer_cb(on_ringer);
    chronos_core_set_config_cb(on_config);

    chronos_core_set_connection_status(true, true);
    chronos_core_send_battery(84, false);
    chronos_core_send_sync_request();

    uint8_t notification[] = {
        0xAB, 0x00, 0x1D, 0xFF, 0x72, 0x80, 0x0A, 0x02,
        'A', 'l', 'i', 'c', 'e', ':', 'H', 'e', 'l', 'l', 'o', ' ',
        'f', 'r', 'o', 'm', ' ', 'C', 'h', 'r', 'o', 'n', 'o', 's'
    };
    chronos_core_receive_ble_packet(notification, sizeof(notification));

    uint8_t phone_battery[] = {0xAB, 0x00, 0x05, 0xFE, 0x91, 0x80, 0x01, 0x63};
    chronos_core_receive_ble_packet(phone_battery, sizeof(phone_battery));

    printf("getter battery: %u%%\n", chronos_core_get_phone_battery());

    return 0;
}
