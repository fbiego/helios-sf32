#include "helios_ios.h"

#include <rtthread.h>
#include <stdlib.h>
#include <string.h>

#include "bf0_ble_ancs.h"
#include "bf0_ble_ams.h"
#include "bf0_sibles.h"
#include "helios_app.h"
#include "helios_ui/helios_ui.h"

#define LOG_TAG "helios.ios"
#include "log.h"

#define HELIOS_IOS_TEXT_MAX 128
#define HELIOS_IOS_LOG_TEXT_MAX 32

#define HELIOS_NOTIFY_ICON_CHAT 0x03
#define HELIOS_NOTIFY_ICON_MAIL 0x04
#define HELIOS_NOTIFY_ICON_SKYPE 0x08
#define HELIOS_NOTIFY_ICON_WECHAT 0x09
#define HELIOS_NOTIFY_ICON_WHATSAPP 0x0A
#define HELIOS_NOTIFY_ICON_GMAIL 0x0B
#define HELIOS_NOTIFY_ICON_LINE 0x0E
#define HELIOS_NOTIFY_ICON_TWITTER 0x0F
#define HELIOS_NOTIFY_ICON_FACEBOOK 0x10
#define HELIOS_NOTIFY_ICON_MESSENGER 0x11
#define HELIOS_NOTIFY_ICON_INSTAGRAM 0x12
#define HELIOS_NOTIFY_ICON_TELEGRAM 0x18
#define HELIOS_NOTIFY_ICON_WHATSAPP_BUSINESS 0x20
#define HELIOS_NOTIFY_ICON_CALENDAR 0x30
#define HELIOS_NOTIFY_ICON_SNAPCHAT 0x31
#define HELIOS_NOTIFY_ICON_TIKTOK 0x32
#define HELIOS_NOTIFY_ICON_PAYPAL 0x33

typedef struct
{
    const char *app_id;
    uint8_t icon;
    const char *name;
} helios_ios_app_icon_t;

static const helios_ios_app_icon_t g_ios_app_icons[] = {
    {"net.whatsapp.WhatsApp", HELIOS_NOTIFY_ICON_WHATSAPP, "WhatsApp"},
    {"net.whatsapp.WhatsAppSMB", HELIOS_NOTIFY_ICON_WHATSAPP_BUSINESS, "WhatsApp Business"},
    {"ph.telegra.Telegraph", HELIOS_NOTIFY_ICON_TELEGRAM, "Telegram"},
    {"com.burbn.instagram", HELIOS_NOTIFY_ICON_INSTAGRAM, "Instagram"},
    {"com.facebook.Messenger", HELIOS_NOTIFY_ICON_MESSENGER, "Messenger"},
    {"com.facebook.Facebook", HELIOS_NOTIFY_ICON_FACEBOOK, "Facebook"},
    {"com.atebits.Tweetie2", HELIOS_NOTIFY_ICON_TWITTER, "Twitter"},
    {"com.google.Gmail", HELIOS_NOTIFY_ICON_GMAIL, "Gmail"},
    {"com.microsoft.Office.Outlook", HELIOS_NOTIFY_ICON_MAIL, "Outlook"},
    {"com.apple.mobilemail", HELIOS_NOTIFY_ICON_MAIL, "Mail"},
    {"com.apple.MobileSMS", HELIOS_NOTIFY_ICON_CHAT, "Messages"},
    {"com.apple.mobilecal", HELIOS_NOTIFY_ICON_CALENDAR, "Calendar"},
    {"com.apple.reminders", HELIOS_NOTIFY_ICON_CALENDAR, "Reminders"},
    {"com.apple.mobilephone", HELIOS_NOTIFY_ICON_CHAT, "Phone"},
    {"com.toyopagroup.picaboo", HELIOS_NOTIFY_ICON_SNAPCHAT, "Snapchat"},
    {"com.zhiliaoapp.musically", HELIOS_NOTIFY_ICON_TIKTOK, "TikTok"},
    {"com.paypal.ppmobile", HELIOS_NOTIFY_ICON_PAYPAL, "PayPal"},
    {"com.skype.skype", HELIOS_NOTIFY_ICON_SKYPE, "Skype"},
    {"jp.naver.line", HELIOS_NOTIFY_ICON_LINE, "Line"},
    {"com.tencent.xin", HELIOS_NOTIFY_ICON_WECHAT, "WeChat"},
};

typedef struct
{
    uint8_t conn_idx;
    uint8_t ancs_ready;
    uint8_t ams_ready;
    uint16_t ams_cmd_mask;
    chronos_music_info_t music;
} helios_ios_env_t;

static helios_ios_env_t g_ios = {
    .conn_idx = INVALID_CONN_IDX,
};

static void helios_ios_copy_attr(char *dst, size_t dst_size,
                                 const uint8_t *src, uint16_t src_len)
{
    size_t copy_len;

    if (!dst || dst_size == 0)
        return;

    if (!src || src_len == 0)
    {
        dst[0] = '\0';
        return;
    }

    copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
    memcpy(dst, src, copy_len);
    dst[copy_len] = '\0';
}

static const char *helios_ios_ancs_event_name(uint8_t event)
{
    switch (event)
    {
    case BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED:
        return "added";
    case BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED:
        return "modified";
    case BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED:
        return "removed";
    default:
        return "unknown";
    }
}

static const char *helios_ios_ancs_category_name(uint8_t category)
{
    switch (category)
    {
    case BLE_ANCS_CATEGORY_ID_INCOMING_CALL:
        return "Incoming call";
    case BLE_ANCS_CATEGORY_ID_MISSED_CALL:
        return "Missed call";
    case BLE_ANCS_CATEGORY_ID_VOICE_MAIL:
        return "Voicemail";
    case BLE_ANCS_CATEGORY_ID_SOCIAL:
        return "Social";
    case BLE_ANCS_CATEGORY_ID_SCHEDULE:
        return "Calendar";
    case BLE_ANCS_CATEGORY_ID_EMAIL:
        return "Email";
    case BLE_ANCS_CATEGORY_ID_NEWS:
        return "News";
    case BLE_ANCS_CATEGORY_ID_HEALTH_AND_FITNESS:
        return "Health";
    case BLE_ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE:
        return "Finance";
    case BLE_ANCS_CATEGORY_ID_LOCATION:
        return "Location";
    case BLE_ANCS_CATEGORY_ID_ENTERTAINMENT:
        return "Entertainment";
    default:
        return "Notification";
    }
}

static uint8_t helios_ios_icon_for_category(uint8_t category)
{
    switch (category)
    {
    case BLE_ANCS_CATEGORY_ID_SOCIAL:
        return HELIOS_NOTIFY_ICON_CHAT;
    case BLE_ANCS_CATEGORY_ID_SCHEDULE:
        return HELIOS_NOTIFY_ICON_CALENDAR;
    case BLE_ANCS_CATEGORY_ID_EMAIL:
        return HELIOS_NOTIFY_ICON_MAIL;
    case BLE_ANCS_CATEGORY_ID_INCOMING_CALL:
    case BLE_ANCS_CATEGORY_ID_MISSED_CALL:
    case BLE_ANCS_CATEGORY_ID_VOICE_MAIL:
        return HELIOS_NOTIFY_ICON_CHAT;
    default:
        return HELIOS_NOTIFY_ICON_CHAT;
    }
}

static void helios_ios_apply_app_mapping(chronos_notification_t *notification)
{
    size_t i;

    if (!notification || notification->app[0] == '\0')
        return;

    for (i = 0; i < sizeof(g_ios_app_icons) / sizeof(g_ios_app_icons[0]); i++)
    {
        if (strcmp(notification->app, g_ios_app_icons[i].app_id) == 0)
        {
            notification->icon = g_ios_app_icons[i].icon;
            helios_ios_copy_attr(notification->app, sizeof(notification->app),
                                 (const uint8_t *)g_ios_app_icons[i].name,
                                 (uint16_t)strlen(g_ios_app_icons[i].name));
            return;
        }
    }
}

static const char *helios_ios_ancs_attr_name(uint8_t attr_id)
{
    switch (attr_id)
    {
    case BLE_ANCS_NOTIFICATION_ATTR_ID_APP_ID:
        return "app";
    case BLE_ANCS_NOTIFICATION_ATTR_ID_TITLE:
        return "title";
    case BLE_ANCS_NOTIFICATION_ATTR_ID_MESSAGE:
        return "message";
    default:
        return "other";
    }
}

static const char *helios_ios_ams_entity_name(uint8_t entity_id)
{
    switch (entity_id)
    {
    case BLE_AMS_ENTITY_ID_PLAYER:
        return "player";
    case BLE_AMS_ENTITY_ID_QUEUE:
        return "queue";
    case BLE_AMS_ENTITY_ID_TRACK:
        return "track";
    default:
        return "unknown";
    }
}

static const char *helios_ios_ams_attr_name(uint8_t entity_id, uint8_t attr_id)
{
    if (entity_id == BLE_AMS_ENTITY_ID_PLAYER)
    {
        switch (attr_id)
        {
        case BLE_AMS_PLAYER_ATTR_ID_NAME:
            return "name";
        case BLE_AMS_PLAYER_ATTR_ID_PB_INFO:
            return "playback";
        case BLE_AMS_PLAYER_ATTR_ID_VOL:
            return "volume";
        default:
            return "other";
        }
    }

    if (entity_id == BLE_AMS_ENTITY_ID_TRACK)
    {
        switch (attr_id)
        {
        case BLE_AMS_TRACK_ATTR_ID_ARTIST:
            return "artist";
        case BLE_AMS_TRACK_ATTR_ID_ALBUM:
            return "album";
        case BLE_AMS_TRACK_ATTR_ID_TILTE:
            return "title";
        case BLE_AMS_TRACK_ATTR_ID_DURATION:
            return "duration";
        default:
            return "other";
        }
    }

    return "other";
}

static void helios_ios_log_text(const char *prefix, const uint8_t *data,
                                uint16_t len)
{
    char text[HELIOS_IOS_LOG_TEXT_MAX + 1];

    if (!data || len == 0)
    {
        LOG_I("%s len=0", prefix);
        return;
    }

    helios_ios_copy_attr(text, sizeof(text), data, len);
    LOG_I("%s len=%u value='%s%s'", prefix, len, text,
          len >= HELIOS_IOS_LOG_TEXT_MAX ? "..." : "");
}

static void helios_ios_handle_ancs_notification(const ble_ancs_noti_attr_t *noti)
{
    chronos_notification_t notification;
    ble_ancs_attr_value_t *attr;
    uint8_t i;

    if (!noti)
    {
        LOG_W("ANCS notification ind missing payload");
        return;
    }

    LOG_I("ANCS notification event=%s uid=%u category=%s attr_count=%u flags=0x%02x",
          helios_ios_ancs_event_name(noti->evt_id),
          noti->noti_uid,
          helios_ios_ancs_category_name(noti->cate_id),
          noti->attr_count,
          noti->evt_flag);

    if (noti->evt_id == BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED)
        return;

    memset(&notification, 0, sizeof(notification));
    notification.icon = helios_ios_icon_for_category(noti->cate_id);
    helios_ios_copy_attr(notification.app, sizeof(notification.app),
                         (const uint8_t *)helios_ios_ancs_category_name(noti->cate_id),
                         (uint16_t)strlen(helios_ios_ancs_category_name(noti->cate_id)));

    attr = noti->value;
    for (i = 0; i < noti->attr_count && attr; i++)
    {
        char prefix[40];

        rt_snprintf(prefix, sizeof(prefix), "ANCS attr %s",
                    helios_ios_ancs_attr_name(attr->attr_id));
        helios_ios_log_text(prefix, attr->data, attr->len);

        switch (attr->attr_id)
        {
        case BLE_ANCS_NOTIFICATION_ATTR_ID_APP_ID:
            helios_ios_copy_attr(notification.app, sizeof(notification.app),
                                 attr->data, attr->len);
            break;
        case BLE_ANCS_NOTIFICATION_ATTR_ID_TITLE:
            helios_ios_copy_attr(notification.title, sizeof(notification.title),
                                 attr->data, attr->len);
            break;
        case BLE_ANCS_NOTIFICATION_ATTR_ID_MESSAGE:
            helios_ios_copy_attr(notification.message, sizeof(notification.message),
                                 attr->data, attr->len);
            break;
        default:
            break;
        }

        attr = (ble_ancs_attr_value_t *)((uint8_t *)attr +
                                         sizeof(ble_ancs_attr_value_t) +
                                         attr->len);
    }

    helios_ios_apply_app_mapping(&notification);

    if (notification.title[0] == '\0')
        helios_ios_copy_attr(notification.title, sizeof(notification.title),
                             (const uint8_t *)notification.app,
                             (uint16_t)strlen(notification.app));

    LOG_I("ANCS publish icon=0x%02x app='%s' title='%s' msg_len=%u",
          notification.icon,
          notification.app,
          notification.title,
          (uint16_t)strlen(notification.message));
    helios_app_chronos_notification(&notification);
}

static void helios_ios_parse_playback_info(const char *value)
{
    int state;

    if (!value || value[0] == '\0')
        return;

    state = atoi(value);
    g_ios.music.state = state == 1 ? 1 : 0;
    LOG_I("AMS playback info='%s' state=%u", value, g_ios.music.state);
}

static void helios_ios_publish_music(void)
{
    helios_app_music_update(&g_ios.music);
}

static void helios_ios_configure_ancs(void)
{
    ble_ancs_category_mask_set(BLE_ANCS_CATEGORY_ID_MASK_ALL);
    ble_ancs_attr_enable(BLE_ANCS_NOTIFICATION_ATTR_ID_APP_ID, 1, 64);
    ble_ancs_attr_enable(BLE_ANCS_NOTIFICATION_ATTR_ID_TITLE, 1, 96);
    ble_ancs_attr_enable(BLE_ANCS_NOTIFICATION_ATTR_ID_MESSAGE, 1, 256);
}

static void helios_ios_configure_ams(void)
{
    ble_ams_player_attr_enable(BLE_AMS_PLAYER_ATTR_ID_NAME_MASK |
                               BLE_AMS_PLAYER_ATTR_ID_PB_INFO_MASK);
    ble_ams_track_attr_enable(BLE_AMS_TRACK_ATTR_ID_ARTIST_MASK |
                              BLE_AMS_TRACK_ATTR_ID_TILTE_MASK);
}

static void helios_ios_handle_ams_entity(const ble_ams_entity_attr_value_t *attr)
{
    char value[HELIOS_IOS_TEXT_MAX];

    if (!attr)
        return;

    helios_ios_copy_attr(value, sizeof(value), attr->value, attr->len);
    LOG_I("AMS entity=%s attr=%s len=%u",
          helios_ios_ams_entity_name(attr->entity_id),
          helios_ios_ams_attr_name(attr->entity_id, attr->attr_id),
          attr->len);

    switch (attr->entity_id)
    {
    case BLE_AMS_ENTITY_ID_PLAYER:
        if (attr->attr_id == BLE_AMS_PLAYER_ATTR_ID_NAME)
        {
            helios_ios_copy_attr(g_ios.music.app_name,
                                 sizeof(g_ios.music.app_name),
                                 (const uint8_t *)value,
                                 (uint16_t)strlen(value));
            helios_ios_copy_attr(g_ios.music.package_name,
                                 sizeof(g_ios.music.package_name),
                                 (const uint8_t *)"com.apple.ams",
                                 (uint16_t)strlen("com.apple.ams"));
        }
        else if (attr->attr_id == BLE_AMS_PLAYER_ATTR_ID_PB_INFO)
        {
            helios_ios_parse_playback_info(value);
        }
        break;
    case BLE_AMS_ENTITY_ID_TRACK:
        if (attr->attr_id == BLE_AMS_TRACK_ATTR_ID_ARTIST)
        {
            helios_ios_copy_attr(g_ios.music.artist,
                                 sizeof(g_ios.music.artist),
                                 (const uint8_t *)value,
                                 (uint16_t)strlen(value));
        }
        else if (attr->attr_id == BLE_AMS_TRACK_ATTR_ID_TILTE)
        {
            helios_ios_copy_attr(g_ios.music.title,
                                 sizeof(g_ios.music.title),
                                 (const uint8_t *)value,
                                 (uint16_t)strlen(value));
        }
        break;
    default:
        break;
    }

    helios_ios_publish_music();
}

void helios_ios_on_connected(uint8_t conn_idx)
{
    uint8_t ret;

    if (g_ios.conn_idx != conn_idx)
    {
        g_ios.conn_idx = conn_idx;
        g_ios.ancs_ready = 0;
        g_ios.ams_ready = 0;
        g_ios.ams_cmd_mask = 0;
        memset(&g_ios.music, 0, sizeof(g_ios.music));
        g_ios.music.background_color = 0xFFFFFF;
        LOG_I("iOS profile discovery reset conn_idx=%u", conn_idx);
    }

    if (!g_ios.ancs_ready)
    {
        LOG_I("ANCS discovery start conn_idx=%u", conn_idx);
        helios_ios_configure_ancs();
        ble_ancs_cccd_enable(1);
        ret = ble_ancs_enable(conn_idx);
        LOG_I("ANCS enable ret=%u", ret);
    }

    if (!g_ios.ams_ready)
    {
        LOG_I("AMS discovery start conn_idx=%u", conn_idx);
        ble_ams_cccd_enable(1);
        helios_ios_configure_ams();
        ret = ble_ams_enable(conn_idx);
        LOG_I("AMS enable ret=%u", ret);
    }
}

void helios_ios_on_bonded(uint8_t conn_idx)
{
    uint8_t ret;

    if (g_ios.conn_idx != conn_idx)
    {
        LOG_W("iOS bonded event ignored conn_idx=%u active=%u",
              conn_idx, g_ios.conn_idx);
        return;
    }

    LOG_I("iOS bonded refresh conn_idx=%u ancs=%u ams=%u",
          conn_idx, g_ios.ancs_ready, g_ios.ams_ready);

    if (g_ios.ancs_ready)
    {
        helios_ios_configure_ancs();
        ret = ble_ancs_cccd_enable(0);
        LOG_I("ANCS cccd disable ret=%u", ret);
        ret = ble_ancs_cccd_enable(1);
        LOG_I("ANCS cccd enable ret=%u", ret);
    }
    else
    {
        helios_ios_on_connected(conn_idx);
    }

    if (g_ios.ams_ready)
    {
        helios_ios_configure_ams();
        ret = ble_ams_cccd_enable(0);
        LOG_I("AMS cccd disable ret=%u", ret);
        ret = ble_ams_cccd_enable(1);
        LOG_I("AMS cccd enable ret=%u", ret);
    }
    else
    {
        helios_ios_on_connected(conn_idx);
    }
}

void helios_ios_on_disconnected(uint8_t conn_idx)
{
    if (g_ios.conn_idx == conn_idx)
    {
        LOG_I("iOS profiles disconnected conn_idx=%u ancs=%u ams=%u",
              conn_idx, g_ios.ancs_ready, g_ios.ams_ready);
        g_ios.conn_idx = INVALID_CONN_IDX;
        g_ios.ancs_ready = 0;
        g_ios.ams_ready = 0;
        g_ios.ams_cmd_mask = 0;
        memset(&g_ios.music, 0, sizeof(g_ios.music));
    }
}

uint8_t helios_ios_music_command(const char *action)
{
    ble_ams_cmd_t cmd;
    uint16_t mask;

    if (!action || !g_ios.ams_ready)
        return 0;

    if (strcmp(action, "previous") == 0)
    {
        cmd = BLE_AMS_CMD_PREV;
        mask = BLE_AMS_CMD_PREV_MASK;
    }
    else if (strcmp(action, "toggle") == 0)
    {
        cmd = BLE_AMS_CMD_TOGGLE_PLAY_PAUSE;
        mask = BLE_AMS_CMD_TOGGLE_PLAY_PAUSE_MASK;
    }
    else if (strcmp(action, "next") == 0)
    {
        cmd = BLE_AMS_CMD_NEXT;
        mask = BLE_AMS_CMD_NEXT_MASK;
    }
    else
    {
        return 0;
    }

    if ((g_ios.ams_cmd_mask & mask) == 0)
    {
        LOG_W("AMS command '%s' unsupported mask=0x%04x",
              action, g_ios.ams_cmd_mask);
        return 0;
    }

    LOG_I("AMS command '%s' send cmd=%u", action, cmd);
    return ble_ams_send_command(cmd) == BLE_AMS_ERR_NO_ERR ? 1 : 0;
}

uint8_t helios_ios_ams_ready(void)
{
    return g_ios.ams_ready;
}

uint8_t helios_ios_ancs_ready(void)
{
    return g_ios.ancs_ready;
}

static int helios_ios_event_handler(uint16_t event_id, uint8_t *data,
                                    uint16_t len, uint32_t context)
{
    (void)len;
    (void)context;

    switch (event_id)
    {
    case BLE_ANCS_ENABLE_RSP:
    {
        ble_ancs_enable_rsp_t *rsp = (ble_ancs_enable_rsp_t *)data;
        g_ios.ancs_ready = (rsp && rsp->result == BLE_ANCS_ERR_NO_ERR) ? 1 : 0;
        LOG_I("ANCS %s result=%u conn_idx=%u",
              g_ios.ancs_ready ? "ready" : "failed",
              rsp ? rsp->result : 0xFF,
              g_ios.conn_idx);
        break;
    }
    case BLE_ANCS_NOTIFICATION_IND:
        helios_ios_handle_ancs_notification((const ble_ancs_noti_attr_t *)data);
        break;
    case BLE_AMS_ENABLE_RSP:
    {
        ble_ams_enable_rsp_t *rsp = (ble_ams_enable_rsp_t *)data;
        g_ios.ams_ready = (rsp && rsp->result == BLE_AMS_ERR_NO_ERR) ? 1 : 0;
        LOG_I("AMS %s result=%u conn_idx=%u",
              g_ios.ams_ready ? "ready" : "failed",
              rsp ? rsp->result : 0xFF,
              g_ios.conn_idx);
        break;
    }
    case BLE_AMS_ENABLE_PENDING_IND:
    {
        ble_ams_enable_pending_ind_t *ind = (ble_ams_enable_pending_ind_t *)data;
        if (ind && ind->result == BLE_AMS_ERR_NO_ERR)
            g_ios.ams_ready = 1;
        LOG_I("AMS pending result=%u conn_idx=%u",
              ind ? ind->result : 0xFF,
              g_ios.conn_idx);
        break;
    }
    case BLE_AMS_SUPPORTED_CMD_NOTIFY_IND:
    {
        ble_ams_supported_cmd_notify_ind_t *ind =
            (ble_ams_supported_cmd_notify_ind_t *)data;
        if (ind)
            g_ios.ams_cmd_mask = ind->cmd_mask;
        LOG_I("AMS command mask=0x%04x", g_ios.ams_cmd_mask);
        break;
    }
    case BLE_AMS_ENTITY_ATTRIBUTE_PAIR_IND:
        helios_ios_handle_ams_entity((const ble_ams_entity_attr_value_t *)data);
        break;
    default:
        break;
    }

    return 0;
}
BLE_EVENT_REGISTER(helios_ios_event_handler, NULL);
