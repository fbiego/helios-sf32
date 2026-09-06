/**
 * @file mpesa_notifications.c
 * @brief M-Pesa BLE notification parser
 */

#include "mpesa_notifications.h"

#include <ctype.h>
#include <rtthread.h>
#include <string.h>

#include "helios_ui/custom/apps/app_manager.h"
#include "helios_ui/custom/apps/mpesa/mpesa.h"
#include "helios_ui/custom/subjects/subjects.h"
#include "helios_ui/helios_ui_gen.h"

#define HELIOS_MPESA_MESSAGE_MAX 512
#define HELIOS_MPESA_SEEN_IDS_MAX 8

static char seen_tx_ids[HELIOS_MPESA_SEEN_IDS_MAX][HELIOS_MPESA_TX_ID_MAX];
static uint8_t seen_tx_id_next;

static void constructor(void) HELIOS_CONSTRUCTOR_ATTR;
static void app_runtime_init(void);
static void copy_range(char * dst, uint32_t dst_size, const char * start, const char * end);
static void trim_text(char * text);
static bool sender_is_mpesa(const char * title);
static void normalize_message(char * dst, uint32_t dst_size, const char * src);
static const char * find_first(const char * haystack, const char * a, const char * b);
static const char * find_money_after(const char * text, const char * marker);
static void copy_money(char * dst, uint32_t dst_size, const char * start, char sign);
static void strip_zero_cents(char * amount);
static void parse_time(char * dst, uint32_t dst_size, const char * on);
static bool tx_id_seen(const char * tx_id);
static void remember_tx_id(const char * tx_id);
static void set_recent_label(bool has_transactions);

bool helios_mpesa_handle_notification(const char * app, const char * title, const char * message)
{
    char body[HELIOS_MPESA_MESSAGE_MAX];
    char tx_id[HELIOS_MPESA_TX_ID_MAX] = "";
    char amount[HELIOS_MPESA_AMOUNT_MAX] = "";
    char name[HELIOS_MPESA_NAME_MAX] = "";
    char account[HELIOS_MPESA_ACCOUNT_MAX] = "";
    char time_text[HELIOS_MPESA_TIME_MAX] = "";
    char balance[HELIOS_MPESA_AMOUNT_MAX] = "";
    char fee[HELIOS_MPESA_FEE_MAX] = "KSH 0.00";
    const char *confirmed;
    const char *p;
    const char *end;
    bool tx_in;

    if ((!sender_is_mpesa(app) && !sender_is_mpesa(title)) || !message)
        return false;

    normalize_message(body, sizeof(body), message);
    confirmed = strstr(body, "Confirmed");
    if (!confirmed)
        return false;

    if (confirmed > body) {
        end = confirmed;
        while (end > body && isspace((unsigned char)end[-1])) end--;
        copy_range(tx_id, sizeof(tx_id), body, end);
    }

    if (tx_id[0] && tx_id_seen(tx_id))
        return true;

    tx_in = strstr(body, "You have received") != NULL;
    if (tx_in) {
        p = find_money_after(body, "received Ksh");
    } else {
        p = find_money_after(body, "Confirmed. Ksh");
        if (!p) p = find_money_after(body, "Confirmed.Ksh");
    }
    if (!p)
        return false;

    copy_money(amount, sizeof(amount), p, '\0'); // tx_in ? '+' : '-');

    if (tx_in) {
        p = strstr(body, " from ");
        if (!p) return false;
        p += strlen(" from ");
        end = strstr(p, " on ");
        copy_range(name, sizeof(name), p, end);
    } else {
        p = strstr(body, " sent to ");
        if (p) {
            p += strlen(" sent to ");
        } else {
            p = strstr(body, " paid to ");
            if (!p) return false;
            p += strlen(" paid to ");
        }

        end = strstr(p, " for account ");
        if (end) {
            const char *account_start = end + strlen(" for account ");
            const char *account_end = strstr(account_start, " on ");
            copy_range(name, sizeof(name), p, end);
            copy_range(account, sizeof(account), account_start, account_end);
        } else {
            end = find_first(p, ". on ", " on ");
            copy_range(name, sizeof(name), p, end);
        }
    }

    p = strstr(body, " on ");
    if (p)
        parse_time(time_text, sizeof(time_text), p + strlen(" on "));

    p = find_money_after(body, "New M-PESA balance is Ksh");
    if (p) {
        char raw_balance[HELIOS_MPESA_AMOUNT_MAX];
        copy_money(raw_balance, sizeof(raw_balance), p, '\0');
        rt_snprintf(balance, sizeof(balance), "KSH %s", raw_balance);
        helios_mpesa_set_balance(balance);
    }

    p = find_money_after(body, "Transaction cost, Ksh");
    if (p) {
        char raw_fee[HELIOS_MPESA_FEE_MAX];
        copy_money(raw_fee, sizeof(raw_fee), p, '\0');
        rt_snprintf(fee, sizeof(fee), "KSH %s", raw_fee);
    }

    if (time_text[0] == '\0')
        copy_range(time_text, sizeof(time_text), "M-PESA", NULL);

    if (!helios_mpesa_add_and_open(name,
                                   time_text,
                                   amount,
                                   account,
                                   tx_id,
                                   tx_in,
                                   fee,
                                   HELIOS_MPESA_DETAILS_TIMEOUT_MS)) {
        return false;
    }

    remember_tx_id(tx_id);
    set_recent_label(true);
    return true;
}

static void constructor(void)
{
    helios_apps_initializer_add(app_runtime_init);
}

static void app_runtime_init(void)
{
    helios_mpesa_clear();
    set_recent_label(false);
    helios_mpesa_set_balance("No data");
}

static void copy_range(char * dst, uint32_t dst_size, const char * start, const char * end)
{
    uint32_t len;

    if (!dst || dst_size == 0) return;
    dst[0] = '\0';
    if (!start) return;
    if (!end || end < start) end = start + strlen(start);

    while (start < end && isspace((unsigned char)*start)) start++;
    while (end > start && isspace((unsigned char)end[-1])) end--;
    while (end > start && (end[-1] == '.' || end[-1] == ';')) end--;

    len = (uint32_t)(end - start);
    if (len >= dst_size) len = dst_size - 1;
    memcpy(dst, start, len);
    dst[len] = '\0';
    trim_text(dst);
}

static void trim_text(char * text)
{
    char *read;
    char *write;
    bool prev_space = true;

    if (!text) return;

    read = text;
    write = text;
    while (*read) {
        unsigned char ch = (unsigned char)*read++;
        if (isspace(ch)) {
            if (!prev_space) *write++ = ' ';
            prev_space = true;
        } else {
            *write++ = (char)ch;
            prev_space = false;
        }
    }

    if (write > text && write[-1] == ' ') write--;
    *write = '\0';
}

static bool sender_is_mpesa(const char * title)
{
    char compact[8];
    uint32_t out = 0;

    if (!title) return false;

    while (*title && out < sizeof(compact) - 1) {
        unsigned char ch = (unsigned char)*title++;
        if (ch == '-' || ch == '_' || isspace(ch))
            continue;
        compact[out++] = (char)toupper(ch);
    }
    compact[out] = '\0';

    return strcmp(compact, "MPESA") == 0;
}

static void normalize_message(char * dst, uint32_t dst_size, const char * src)
{
    uint32_t out = 0;
    bool prev_space = false;

    if (!dst || dst_size == 0) return;

    while (src && *src && out < dst_size - 1) {
        unsigned char ch = (unsigned char)*src++;
        if (ch == 0xC2 && (unsigned char)*src == 0xA0) {
            src++;
            ch = ' ';
        }

        if (isspace(ch)) {
            if (!prev_space) dst[out++] = ' ';
            prev_space = true;
        } else {
            dst[out++] = (char)ch;
            prev_space = false;
        }
    }

    if (out > 0 && dst[out - 1] == ' ') out--;
    dst[out] = '\0';
}

static const char * find_first(const char * haystack, const char * a, const char * b)
{
    const char *pa = a ? strstr(haystack, a) : NULL;
    const char *pb = b ? strstr(haystack, b) : NULL;

    if (!pa) return pb;
    if (!pb) return pa;
    return pa < pb ? pa : pb;
}

static const char * find_money_after(const char * text, const char * marker)
{
    const char *p = text && marker ? strstr(text, marker) : NULL;
    return p ? p + strlen(marker) : NULL;
}

static void copy_money(char * dst, uint32_t dst_size, const char * start, char sign)
{
    char *out = dst;
    uint32_t left = dst_size;

    if (!dst || dst_size == 0) return;
    dst[0] = '\0';
    if (!start) return;

    if (sign && left > 1) {
        *out++ = sign;
        left--;
    }

    while (*start && left > 1) {
        unsigned char ch = (unsigned char)*start;
        if (ch == '.' && !isdigit((unsigned char)start[1]))
            break;
        if (!(isdigit(ch) || ch == ',' || ch == '.'))
            break;
        *out++ = (char)*start++;
        left--;
    }
    *out = '\0';
    strip_zero_cents(dst);
}

static void strip_zero_cents(char * amount)
{
    size_t len;

    if (!amount) return;

    len = strlen(amount);
    if (len >= 3 && amount[len - 3] == '.' &&
        amount[len - 2] == '0' &&
        amount[len - 1] == '0') {
        amount[len - 3] = '\0';
    }
}

static void parse_time(char * dst, uint32_t dst_size, const char * on)
{
    char date[12];
    char tm[12];
    const char *at;
    const char *ampm;

    if (!dst || dst_size == 0) return;
    dst[0] = '\0';
    if (!on) return;

    at = strstr(on, " at ");
    if (!at) {
        copy_range(dst, dst_size, on, strstr(on, " New "));
        return;
    }

    copy_range(date, sizeof(date), on, at);
    at += strlen(" at ");
    ampm = strstr(at, " AM");
    if (!ampm) ampm = strstr(at, " PM");
    if (ampm)
        copy_range(tm, sizeof(tm), at, ampm + 3);
    else
        copy_range(tm, sizeof(tm), at, strstr(at, " New "));

    rt_snprintf(dst, dst_size, "%s • %s", tm, date);
}

static bool tx_id_seen(const char * tx_id)
{
    if (!tx_id || !tx_id[0]) return false;

    for (uint32_t i = 0; i < HELIOS_MPESA_SEEN_IDS_MAX; i++) {
        if (strcmp(seen_tx_ids[i], tx_id) == 0)
            return true;
    }

    return false;
}

static void remember_tx_id(const char * tx_id)
{
    if (!tx_id || !tx_id[0]) return;

    rt_snprintf(seen_tx_ids[seen_tx_id_next],
                sizeof(seen_tx_ids[seen_tx_id_next]),
                "%s",
                tx_id);
    seen_tx_id_next = (uint8_t)((seen_tx_id_next + 1u) % HELIOS_MPESA_SEEN_IDS_MAX);
}

static void set_recent_label(bool has_transactions)
{
    helios_subject_set_mpesa_recent_text(has_transactions ? "Recent" : "No recent transactions");
}
