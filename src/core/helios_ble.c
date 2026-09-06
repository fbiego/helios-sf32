#include "helios_ble.h"

#include <stdlib.h>
#include <string.h>

#include "bf0_ble_gap.h"
#include "bf0_sibles.h"
#include "bf0_sibles_advertising.h"
#include "ble_connection_manager.h"

#include "helios_chronos.h"
#include "helios_ios.h"
#include "helios_ui/helios_ui.h"
#define LOG_TAG "helios.ble"
#include "log.h"

#define HELIOS_NUS_MAX_VALUE_LEN 244
#define HELIOS_UUID_16(x) {((uint8_t)((x) & 0xff)), ((uint8_t)((x) >> 8))}

/*
 * SiFli stores ATT UUIDs LSB-first.
 * Nordic UART Service UUIDs:
 *   Service: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *   RX:      6E400002-B5A3-F393-E0A9-E50E24DCCA9E
 *   TX:      6E400003-B5A3-F393-E0A9-E50E24DCCA9E
 */
#define HELIOS_NUS_SERVICE_UUID \
    {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, \
     0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E}

#define HELIOS_NUS_RX_UUID \
    {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, \
     0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E}

#define HELIOS_NUS_TX_UUID \
    {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, \
     0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E}

typedef enum
{
    HELIOS_NUS_SVC,
    HELIOS_NUS_RX_CHAR,
    HELIOS_NUS_RX_VALUE,
    HELIOS_NUS_TX_CHAR,
    HELIOS_NUS_TX_VALUE,
    HELIOS_NUS_TX_CCCD,
    HELIOS_NUS_ATT_NB,
} helios_nus_att_t;

typedef struct
{
    rt_mailbox_t mailbox;
    sibles_hdl svc_handle;
    uint8_t conn_idx;
    uint8_t connected;
    uint8_t service_ready;
    uint8_t adv_ready;
    uint16_t mtu;
    uint16_t tx_cccd;
    helios_ble_state_t state;
} helios_ble_env_t;

static helios_ble_env_t g_ble;
static uint8_t g_nus_svc_uuid[ATT_UUID_128_LEN] = HELIOS_NUS_SERVICE_UUID;

BLE_GATT_SERVICE_DEFINE_128(g_nus_att_db)
{
    BLE_GATT_SERVICE_DECLARE(HELIOS_NUS_SVC, HELIOS_UUID_16(ATT_DECL_PRIMARY_SERVICE),
                             BLE_GATT_PERM_READ_ENABLE),
    BLE_GATT_CHAR_DECLARE(HELIOS_NUS_RX_CHAR, HELIOS_UUID_16(ATT_DECL_CHARACTERISTIC),
                          BLE_GATT_PERM_READ_ENABLE),
    BLE_GATT_CHAR_VALUE_DECLARE(HELIOS_NUS_RX_VALUE, HELIOS_NUS_RX_UUID,
                                BLE_GATT_PERM_WRITE_REQ_ENABLE | BLE_GATT_PERM_WRITE_COMMAND_ENABLE,
                                BLE_GATT_VALUE_PERM_UUID_128 | BLE_GATT_VALUE_PERM_RI_ENABLE,
                                HELIOS_NUS_MAX_VALUE_LEN),
    BLE_GATT_CHAR_DECLARE(HELIOS_NUS_TX_CHAR, HELIOS_UUID_16(ATT_DECL_CHARACTERISTIC),
                          BLE_GATT_PERM_READ_ENABLE),
    BLE_GATT_CHAR_VALUE_DECLARE(HELIOS_NUS_TX_VALUE, HELIOS_NUS_TX_UUID,
                                BLE_GATT_PERM_NOTIFY_ENABLE,
                                BLE_GATT_VALUE_PERM_UUID_128 | BLE_GATT_VALUE_PERM_RI_ENABLE,
                                HELIOS_NUS_MAX_VALUE_LEN),
    BLE_GATT_DESCRIPTOR_DECLARE(HELIOS_NUS_TX_CCCD, HELIOS_UUID_16(ATT_DESC_CLIENT_CHAR_CFG),
                                BLE_GATT_PERM_READ_ENABLE | BLE_GATT_PERM_WRITE_REQ_ENABLE,
                                BLE_GATT_VALUE_PERM_RI_ENABLE, 2),
};

SIBLES_ADVERTISING_CONTEXT_DECLAR(g_helios_adv_context);

static void helios_ble_advertising_start(void);

static uint8_t *helios_nus_get_cbk(uint8_t conn_idx, uint8_t idx, uint16_t *len)
{
    (void)conn_idx;

    if (!len)
        return NULL;

    switch (idx)
    {
    case HELIOS_NUS_TX_CCCD:
        *len = sizeof(g_ble.tx_cccd);
        return (uint8_t *)&g_ble.tx_cccd;
    default:
        *len = 0;
        return NULL;
    }
}

static uint8_t helios_nus_set_cbk(uint8_t conn_idx, sibles_set_cbk_t *para)
{
    (void)conn_idx;

    if (!para)
        return 0;

    switch (para->idx)
    {
    case HELIOS_NUS_RX_VALUE:
        LOG_HEX("nus rx", 16, para->value, para->len);
        helios_chronos_on_rx(para->value, para->len);
        break;
    case HELIOS_NUS_TX_CCCD:
        if (para->value && para->len >= 2)
            memcpy(&g_ble.tx_cccd, para->value, sizeof(g_ble.tx_cccd));
        else if (para->value && para->len == 1)
            g_ble.tx_cccd = para->value[0];
        LOG_I("tx cccd=0x%04x", g_ble.tx_cccd);
        if (g_ble.tx_cccd & 0x0001)
            helios_chronos_on_subscribed();
        break;
    default:
        break;
    }

    return 0;
}

static void helios_nus_service_init(void)
{
    if (g_ble.service_ready)
        return;

    BLE_GATT_SERVICE_INIT_128(svc, g_nus_att_db, HELIOS_NUS_ATT_NB,
                              BLE_GATT_SERVICE_PERM_NOAUTH |
                              BLE_GATT_SERVICE_PERM_UUID_128 |
                              BLE_GATT_SERVICE_PERM_MULTI_LINK,
                              g_nus_svc_uuid);

    g_ble.svc_handle = sibles_register_svc_128(&svc);
    if (g_ble.svc_handle)
    {
        sibles_register_cbk(g_ble.svc_handle, helios_nus_get_cbk, helios_nus_set_cbk);
        g_ble.service_ready = 1;
        LOG_I("Nordic UART Service registered");
    }
    else
    {
        LOG_E("Nordic UART Service register failed");
    }
}

static uint8_t helios_ble_adv_event(uint8_t event, void *context, void *data)
{
    (void)context;

    switch (event)
    {
    case SIBLES_ADV_EVT_ADV_STARTED:
    {
        sibles_adv_evt_startted_t *evt = (sibles_adv_evt_startted_t *)data;
        LOG_I("advertising start status=%d mode=%d", evt->status, evt->adv_mode);
        if (evt->status == SIBLES_ADV_NO_ERR && !g_ble.connected)
            g_ble.state = HELIOS_BLE_STATE_ADVERTISING;
        break;
    }
    case SIBLES_ADV_EVT_ADV_STOPPED:
    {
        sibles_adv_evt_stopped_t *evt = (sibles_adv_evt_stopped_t *)data;
        LOG_I("advertising stopped reason=%d mode=%d", evt->reason, evt->adv_mode);
        break;
    }
    default:
        break;
    }

    return 0;
}

static void helios_ble_set_name(char *local_name, size_t local_name_size)
{
    bd_addr_t addr;
    uint8_t ret = ble_get_public_address(&addr);

    if (ret == HL_ERR_NO_ERROR)
    {
        rt_snprintf(local_name, local_name_size, "HELIOS-%02X%02X%02X",
                    addr.addr[2], addr.addr[1], addr.addr[0]);
        
        char mac_str[18];
        rt_snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                    addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0]);
        helios_subject_set_board_mac(mac_str);
    }
    else
    {
        rt_snprintf(local_name, local_name_size, "HELIOS-SF32");
    }

    ble_gap_dev_name_t *dev_name = malloc(sizeof(ble_gap_dev_name_t) + strlen(local_name));
    if (dev_name)
    {
        dev_name->len = strlen(local_name);
        memcpy(dev_name->name, local_name, dev_name->len);
        ble_gap_set_dev_name(dev_name);
        free(dev_name);
    }
}

static void helios_ble_advertising_start(void)
{
    sibles_advertising_para_t para = {0};
    char local_name[31] = {0};
    const char short_name[] = "HELIOS";
    uint8_t ret;

    helios_ble_set_name(local_name, sizeof(local_name));

    if (g_ble.adv_ready)
    {
        ret = sibles_advertising_start(g_helios_adv_context);
        LOG_I("advertising restart ret=%d", ret);
        return;
    }

    para.own_addr_type = GAPM_STATIC_ADDR;
    para.config.adv_mode = SIBLES_ADV_CONNECT_MODE;
    para.config.mode_config.conn_config.duration = 0;
    para.config.mode_config.conn_config.interval = 0x30;
    para.config.max_tx_pwr = 0x7F;
    para.config.is_auto_restart = 1;

    para.adv_data.completed_uuid = rt_malloc(sizeof(sibles_adv_type_srv_uuid_t) + sizeof(sibles_adv_uuid_t));
    if (para.adv_data.completed_uuid)
    {
        para.adv_data.completed_uuid->count = 1;
        para.adv_data.completed_uuid->uuid_list[0].uuid_len = ATT_UUID_128_LEN;
        rt_memcpy(para.adv_data.completed_uuid->uuid_list[0].uuid.uuid_128,
                  g_nus_svc_uuid, ATT_UUID_128_LEN);
    }

    para.adv_data.shortened_name = rt_malloc(rt_strlen(short_name) + sizeof(sibles_adv_type_name_t));
    if (para.adv_data.shortened_name)
    {
        para.adv_data.shortened_name->name_len = rt_strlen(short_name);
        rt_memcpy(para.adv_data.shortened_name->name, short_name, para.adv_data.shortened_name->name_len);
    }

    para.rsp_data.completed_name = rt_malloc(rt_strlen(local_name) + sizeof(sibles_adv_type_name_t));
    if (para.rsp_data.completed_name)
    {
        para.rsp_data.completed_name->name_len = rt_strlen(local_name);
        rt_memcpy(para.rsp_data.completed_name->name, local_name, para.rsp_data.completed_name->name_len);
    }

    para.evt_handler = helios_ble_adv_event;

    ret = sibles_advertising_init(g_helios_adv_context, &para);
    if (ret == SIBLES_ADV_NO_ERR)
    {
        g_ble.adv_ready = 1;
        ret = sibles_advertising_start(g_helios_adv_context);
        LOG_I("advertising start ret=%d name=%s", ret, local_name);
    }
    else
        LOG_E("advertising init failed %d", ret);

    if (para.adv_data.completed_uuid)
        rt_free(para.adv_data.completed_uuid);
    if (para.adv_data.shortened_name)
        rt_free(para.adv_data.shortened_name);
    if (para.rsp_data.completed_name)
        rt_free(para.rsp_data.completed_name);
}

void helios_ble_init(rt_mailbox_t mailbox)
{
    memset(&g_ble, 0, sizeof(g_ble));
    g_ble.mailbox = mailbox;
    g_ble.conn_idx = INVALID_CONN_IDX;
    g_ble.mtu = 23;
    g_ble.state = HELIOS_BLE_STATE_OFF;
}

void helios_ble_start(void)
{
    LOG_I("BLE power on event");
    helios_nus_service_init();
    helios_ble_advertising_start();
}

int helios_ble_send(const uint8_t *data, uint16_t len)
{
    if (!g_ble.connected || !g_ble.svc_handle || !data || len == 0)
        return -RT_ERROR;

    if ((g_ble.tx_cccd & 0x0001) == 0)
    {
        LOG_W("TX notify not enabled");
        return -RT_EBUSY;
    }

    sibles_value_t value;
    value.hdl = g_ble.svc_handle;
    value.idx = HELIOS_NUS_TX_VALUE;
    value.len = len;
    value.value = (uint8_t *)data;

    return sibles_write_value(g_ble.conn_idx, &value);
}

helios_ble_state_t helios_ble_get_state(void)
{
    return g_ble.state;
}

uint16_t helios_ble_get_mtu(void)
{
    return g_ble.mtu;
}

uint8_t helios_ble_is_connected(void)
{
    return g_ble.connected;
}

static int helios_ble_event_handler(uint16_t event_id, uint8_t *data, uint16_t len, uint32_t context)
{
    (void)len;
    (void)context;

    switch (event_id)
    {
    case BLE_POWER_ON_IND:
        break;
    case BLE_GAP_CONNECTED_IND:
    {
        ble_gap_connect_ind_t *ind = (ble_gap_connect_ind_t *)data;
        ble_gap_sec_req_t sec_req = {
            .conn_idx = ind->conn_idx,
            .auth = GAP_AUTH_REQ_SEC_CON_BOND,
        };

        g_ble.conn_idx = ind->conn_idx;
        g_ble.connected = 1;
        g_ble.tx_cccd = 0;
        g_ble.state = HELIOS_BLE_STATE_CONNECTED;
        helios_chronos_on_connected();
        ble_gap_security_request(&sec_req);
        helios_ios_on_connected(ind->conn_idx);
        LOG_I("connected idx=%d interval=%d", ind->conn_idx, ind->con_interval);
        break;
    }
    case BLE_GAP_DISCONNECTED_IND:
    {
        ble_gap_disconnected_ind_t *ind = (ble_gap_disconnected_ind_t *)data;
        g_ble.connected = 0;
        g_ble.conn_idx = INVALID_CONN_IDX;
        g_ble.tx_cccd = 0;
        g_ble.state = HELIOS_BLE_STATE_ADVERTISING;
        helios_ios_on_disconnected(ind->conn_idx);
        helios_chronos_on_disconnected();
        LOG_I("disconnected reason=%d", ind->reason);
        helios_ble_advertising_start();
        break;
    }
    case SIBLES_MTU_EXCHANGE_IND:
    {
        sibles_mtu_exchange_ind_t *ind = (sibles_mtu_exchange_ind_t *)data;
        g_ble.mtu = ind->mtu;
        LOG_I("mtu=%d", ind->mtu);
        break;
    }
    case BLE_GAP_SECURITY_REQUEST_CNF:
    {
        ble_gap_security_request_cnf_t *cnf = (ble_gap_security_request_cnf_t *)data;
        LOG_I("security request cnf idx=%d status=%d", cnf->conn_idx, cnf->status);
        if (g_ble.connected && cnf->conn_idx == g_ble.conn_idx)
            helios_ios_on_connected(cnf->conn_idx);
        break;
    }
    case BLE_GAP_BOND_IND:
    {
        ble_gap_bond_ind_t *ind = (ble_gap_bond_ind_t *)data;
        LOG_I("bond ind idx=%d info=%d", ind->conn_idx, ind->info);
        if (g_ble.connected && ind->conn_idx == g_ble.conn_idx &&
            ind->info == GAPC_PAIRING_SUCCEED)
            helios_ios_on_bonded(ind->conn_idx);
        break;
    }
    case BLE_GAP_ENCRYPT_IND:
    {
        ble_gap_encrypt_ind_t *ind = (ble_gap_encrypt_ind_t *)data;
        LOG_I("encrypted idx=%d auth=0x%02x", ind->conn_idx, ind->auth);
        if (g_ble.connected && ind->conn_idx == g_ble.conn_idx)
            helios_ios_on_bonded(ind->conn_idx);
        break;
    }
    case SIBLES_WRITE_VALUE_RSP:
    {
        sibles_write_value_rsp_t *rsp = (sibles_write_value_rsp_t *)data;
        if (rsp->result != HL_ERR_NO_ERROR)
            LOG_W("notify rsp=%d", rsp->result);
        break;
    }
    default:
        break;
    }

    return 0;
}
BLE_EVENT_REGISTER(helios_ble_event_handler, NULL);
