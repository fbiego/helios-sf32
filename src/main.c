/*
 * Minimal Helios SF32 watch app.
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>

#include "bf0_ble_gap.h"
#include "bf0_sibles.h"
#include "bf0_sibles_advertising.h"
#include "ble_connection_manager.h"

#ifdef BT_FINSH
#include "bts2_app_interface.h"
#endif

#include "helios_ble.h"
#include "helios_platform.h"

#define LOG_TAG "helios"
#include "log.h"

static rt_mailbox_t g_helios_mb;

#ifndef NVDS_AUTO_UPDATE_MAC_ADDRESS_ENABLE
ble_common_update_type_t ble_request_public_address(bd_addr_t *addr)
{
    int ret = bt_mac_addr_generate_via_uid_v2(addr);
    if (ret != 0)
    {
        LOG_W("generate mac address failed %d", ret);
        return BLE_UPDATE_NO_UPDATE;
    }
    return BLE_UPDATE_ONCE;
}
#endif

int main(void)
{
    LOG_I("=== Helios SF32 start ===");

    helios_platform_init();

    g_helios_mb = rt_mb_create("helios", 8, RT_IPC_FLAG_FIFO);
    RT_ASSERT(g_helios_mb);

    helios_ble_init(g_helios_mb);
    sifli_ble_enable();

#if defined(BT_FINSH) && defined(SF32LB52X_58)
    bt_interface_acl_accept_role_set(0);
    bt_interface_set_linkpolicy(1, 1);
#endif

    while (1)
    {
        rt_uint32_t value;
        if (rt_mb_recv(g_helios_mb, &value, RT_WAITING_FOREVER) == RT_EOK)
        {
            if (value == BLE_POWER_ON_IND)
            {
                LOG_I("BLE power on");
                helios_ble_start();
            }
        }
    }
}

int helios_main_ble_event_handler(uint16_t event_id, uint8_t *data, uint16_t len, uint32_t context)
{
    (void)data;
    (void)len;
    (void)context;

    if (event_id == BLE_POWER_ON_IND && g_helios_mb)
    {
        rt_mb_send(g_helios_mb, BLE_POWER_ON_IND);
    }

    return 0;
}
BLE_EVENT_REGISTER(helios_main_ble_event_handler, NULL);

#ifdef SF32LB52X_58
uint16_t g_em_offset[HAL_LCPU_CONFIG_EM_BUF_MAX_NUM] =
{
    0x178, 0x178, 0x740, 0x7A0, 0x810, 0x880, 0xA00, 0xBB0, 0xD48,
    0x133C, 0x13A4, 0x19BC, 0x21BC, 0x21BC, 0x21BC, 0x21BC, 0x21BC, 0x21BC,
    0x21BC, 0x21BC, 0x263C, 0x265C, 0x2734, 0x2784, 0x28D4, 0x28E8, 0x28FC,
    0x29EC, 0x29FC, 0x2BBC, 0x2BD8, 0x3BE8, 0x5804, 0x5804, 0x5804
};

void lcpu_rom_config(void)
{
    hal_lcpu_bluetooth_em_config_t em_offset;
    rt_memcpy((void *)em_offset.em_buf, (void *)g_em_offset, HAL_LCPU_CONFIG_EM_BUF_MAX_NUM * 2);
    em_offset.is_valid = 1;
    HAL_LCPU_CONFIG_set(HAL_LCPU_CONFIG_BT_EM_BUF, &em_offset, sizeof(hal_lcpu_bluetooth_em_config_t));

    hal_lcpu_bluetooth_act_configt_t act_cfg;
    act_cfg.ble_max_act = 6;
    act_cfg.ble_max_iso = 0;
    act_cfg.ble_max_ral = 3;
    act_cfg.bt_max_acl = 7;
    act_cfg.bt_max_sco = 0;
    act_cfg.bit_valid = CO_BIT(0) | CO_BIT(1) | CO_BIT(2) | CO_BIT(3) | CO_BIT(4);
    HAL_LCPU_CONFIG_set(HAL_LCPU_CONFIG_BT_ACT_CFG, &act_cfg, sizeof(hal_lcpu_bluetooth_act_configt_t));
}
#endif
