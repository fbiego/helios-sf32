#ifndef HELIOS_SOUNDS_H
#define HELIOS_SOUNDS_H

#include <time.h>
#include <stdbool.h>
#include <stdint.h>

/* BLE/thread safe. Hourly tones default off until enabled by the phone.
 * Sleep bounds are local minutes since midnight, start inclusive/end exclusive.
 * Equal bounds suppress the hourly tone all day when sleep is enabled. */
void helios_sounds_set_hourly(bool enabled);
void helios_sounds_set_sleep(bool enabled, uint16_t start, uint16_t end);

/* UI thread; poll once a second, including while the display is off. */
void helios_sounds_poll(time_t now, const struct tm *calendar);
void helios_sounds_set_notification_sound(bool enabled);
void helios_sounds_set_notification_vibration(bool enabled);
void helios_sounds_notification(void);
/* Play once after startup initialization, before entering the UI loop. */
void helios_sounds_startup(void);
/* UI thread. true emits an alarm beep; false stops it immediately.
 * Other automatic tones are suppressed until false is called. */
void helios_sounds_alarm(bool ringing);
/* UI thread. Repeat the enabled sound and vibration while a call is visible. */
void helios_sounds_ringtone(bool ringing);
void helios_sounds_ringtone_poll(uint32_t now_ms);

#endif
