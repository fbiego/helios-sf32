#ifndef HELIOS_ALARMS_H
#define HELIOS_ALARMS_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/* UI thread, after storage and Chronos initialization. */
void helios_alarms_init(void);
/* Thread-safe phone callback; no allocation, UI work or filesystem access. */
void helios_alarms_request(uint32_t slot, uint32_t packed_alarm);
/* UI thread, also while the display is off. Returns true when a button
 * dismisses ringing, allowing the caller to consume its normal UI action. */
bool helios_alarms_poll(const struct tm *calendar, uint32_t button_sequence);

#endif
