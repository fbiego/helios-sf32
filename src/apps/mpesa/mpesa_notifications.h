/**
 * @file mpesa_notifications.h
 * @brief M-Pesa BLE notification parser
 */

#ifndef HELIOS_MPESA_NOTIFICATIONS_H
#define HELIOS_MPESA_NOTIFICATIONS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool helios_mpesa_handle_notification(const char * app,
                                      const char * title,
                                      const char * message);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_MPESA_NOTIFICATIONS_H*/
