#ifndef HELIOS_TIME_H
#define HELIOS_TIME_H

#include <stdbool.h>
#include <time.h>

/* UI thread, after preferences are initialized. Defaults to 24-hour mode. */
void helios_time_restore_format(void);
/* Thread-safe phone update. Applied and saved by the next UI clock refresh. */
void helios_time_request_24_hour(bool enabled);
/* UI thread. Publishes hour, AM/PM indicator, AM/PM text and clock string.
 * Calendar/RTC values remain in their native 24-hour representation. */
void helios_time_publish(const struct tm *calendar);

#endif
