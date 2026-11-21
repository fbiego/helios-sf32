#include "helios_time.h"
#include "helios_prefs.h"
#include "helios_ui/custom/subjects/subjects.h"
#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>

#define PREF_TIME_24_HOUR "time.hour_24"

static bool hour_24 = true;
static volatile bool format_pending;
static bool requested_hour_24;

void helios_time_restore_format(void)
{
    helios_prefs_get_bool(PREF_TIME_24_HOUR, &hour_24, true);
}

void helios_time_request_24_hour(bool enabled)
{
    rt_base_t level = rt_hw_interrupt_disable();
    requested_hour_24 = enabled;
    format_pending = true;
    rt_hw_interrupt_enable(level);
}

void helios_time_publish(const struct tm *calendar)
{
    rt_base_t level = rt_hw_interrupt_disable();
    bool pending = format_pending;
    bool requested = requested_hour_24;
    format_pending = false;
    rt_hw_interrupt_enable(level);
    if (pending) {
        hour_24 = requested;
        if (!helios_prefs_set_bool(PREF_TIME_24_HOUR, hour_24)) {
            /* Retry allocation/init failures; a newer phone setting wins. */
            level = rt_hw_interrupt_disable();
            if (!format_pending) {
                requested_hour_24 = requested;
                format_pending = true;
            }
            rt_hw_interrupt_enable(level);
        }
    }
    if (!calendar) return;

    int hour = calendar->tm_hour;
    int display_hour = hour_24 ? hour : (hour % 12 ? hour % 12 : 12);
    int am = hour_24 ? -1 : (hour >= 12 ? 1 : 0);
    char text[16];
    snprintf(text, sizeof(text), "%02d:%02d", display_hour, calendar->tm_min);
    helios_subject_set_time_hour(display_hour);
    helios_subject_set_time_am(am);
    helios_subject_set_time_am_pm(am < 0 ? "" : am ? "PM" : "AM");
    helios_subject_set_time_string(text);
}
