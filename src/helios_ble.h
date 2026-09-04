#ifndef HELIOS_BLE_H
#define HELIOS_BLE_H

#include <rtthread.h>
#include <stdint.h>

typedef enum
{
    HELIOS_BLE_STATE_OFF = 0,
    HELIOS_BLE_STATE_ADVERTISING,
    HELIOS_BLE_STATE_CONNECTED,
} helios_ble_state_t;

void helios_ble_init(rt_mailbox_t mailbox);
void helios_ble_start(void);
int helios_ble_send(const uint8_t *data, uint16_t len);
helios_ble_state_t helios_ble_get_state(void);
uint16_t helios_ble_get_mtu(void);
uint8_t helios_ble_is_connected(void);

#endif
