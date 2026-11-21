#include "helios_sounds.h"
#include "helios_platform.h"
#include <stdbool.h>
#include <rtthread.h>
#include <rthw.h>

#define RINGTONE_PULSE_MS 220
#define RINGTONE_NEXT_PULSE_MS 330
#define RINGTONE_NEXT_PAIR_MS 1500

static bool hourly_enabled, sleep_enabled;
static uint16_t sleep_start, sleep_end;

void helios_sounds_set_hourly(bool enabled)
{
    rt_base_t level = rt_hw_interrupt_disable();
    hourly_enabled = enabled;
    rt_hw_interrupt_enable(level);
}

void helios_sounds_set_sleep(bool enabled, uint16_t start, uint16_t end)
{
    if (start >= 1440 || end >= 1440) return;
    rt_base_t level = rt_hw_interrupt_disable();
    sleep_enabled = enabled;
    sleep_start = start;
    sleep_end = end;
    rt_hw_interrupt_enable(level);
}

static bool hourly_allowed(const struct tm *calendar)
{
    rt_base_t level = rt_hw_interrupt_disable();
    bool enabled = hourly_enabled, sleeping = sleep_enabled;
    uint16_t start = sleep_start, end = sleep_end;
    rt_hw_interrupt_enable(level);
    unsigned minute = calendar->tm_hour * 60 + calendar->tm_min;
    bool in_sleep = start < end ? minute >= start && minute < end :
                                 minute >= start || minute < end;
    return enabled && !(sleeping && in_sleep);
}

static bool clock_ready;
static time_t previous_time;
static bool chimed;
static time_t last_chime;
static bool alarm_ringing;
static bool ringtone_ringing;
static bool notification_sound, notification_vibration, call_motor_on;
static enum { ALERT_TONE_NONE, ALERT_TONE_NOTIFICATION, ALERT_TONE_CALL } alert_tone;
static uint32_t ringtone_last_pulse_ms;
static uint32_t ringtone_interval_ms;

void helios_sounds_poll(time_t now, const struct tm *calendar)
{
    if (!calendar || calendar->tm_year < 120 || calendar->tm_year > 199) {
        clock_ready = false;
        return;
    }

    /* Allow a few seconds of scheduling delay, but do not replay missed
     * hours after startup or a clock correction. Use native 24-hour time. */
    double elapsed = difftime(now, previous_time);
    if (clock_ready && elapsed > 0 && elapsed <= 5 &&
        calendar->tm_min == 0 && calendar->tm_sec >= 0 &&
        calendar->tm_sec < 5 && elapsed > calendar->tm_sec &&
        (!chimed || last_chime != now - calendar->tm_sec)) {
        if (!alarm_ringing && (!ringtone_ringing || !notification_sound) && hourly_allowed(calendar)) {
            alert_tone = ALERT_TONE_NONE;
            (void)helios_buzzer_tone(400);
        }
        last_chime = now - calendar->tm_sec;
        chimed = true;
    }

    previous_time = now;
    clock_ready = true;
}

void helios_sounds_set_notification_sound(bool enabled)
{
    notification_sound = enabled;
    if (!enabled && alert_tone != ALERT_TONE_NONE) {
        helios_buzzer_silence();
        alert_tone = ALERT_TONE_NONE;
    }
}

void helios_sounds_set_notification_vibration(bool enabled)
{
    if (notification_vibration && !enabled) {
        helios_motor_silence();
        call_motor_on = false;
    }
    notification_vibration = enabled;
}

void helios_sounds_notification(void)
{
    /* Bursts merge into a single alert. */
    if (ringtone_ringing)
        return;
    if (notification_sound && !alarm_ringing) {
        alert_tone = ALERT_TONE_NOTIFICATION;
        (void)helios_buzzer_tone(120);
    }
    if (notification_vibration)
        (void)helios_motor_pulse(120);
}

void helios_sounds_startup(void)
{
    if (!alarm_ringing && (!ringtone_ringing || !notification_sound)) {
        alert_tone = ALERT_TONE_NONE;
        (void)helios_buzzer_tone(250);
    }
}

void helios_sounds_alarm(bool ringing)
{
    alarm_ringing = ringing;
    alert_tone = ALERT_TONE_NONE;
    if (!ringing || !notification_vibration)
        ringtone_interval_ms = 0;
    if (ringing)
        (void)helios_buzzer_tone(150);
    else
        helios_buzzer_silence();
}

void helios_sounds_ringtone(bool ringing)
{
    if (ringtone_ringing == ringing)
        return;

    ringtone_ringing = ringing;
    ringtone_interval_ms = 0;
    if (!ringing) {
        if (alert_tone == ALERT_TONE_CALL) {
            helios_buzzer_silence();
            alert_tone = ALERT_TONE_NONE;
        }
        if (call_motor_on) {
            helios_motor_silence();
            call_motor_on = false;
        }
    }
}

void helios_sounds_ringtone_poll(uint32_t now_ms)
{
    bool play_sound = notification_sound && !alarm_ringing;
    if (!ringtone_ringing || (!play_sound && !notification_vibration) ||
        (uint32_t)(now_ms - ringtone_last_pulse_ms) < ringtone_interval_ms)
        return;

    if (play_sound) {
        alert_tone = ALERT_TONE_CALL;
        (void)helios_buzzer_tone(RINGTONE_PULSE_MS);
    }
    if (notification_vibration) {
        call_motor_on = true;
        (void)helios_motor_pulse(RINGTONE_PULSE_MS);
    }
    ringtone_last_pulse_ms = now_ms;
    ringtone_interval_ms = ringtone_interval_ms == RINGTONE_NEXT_PULSE_MS
                          ? RINGTONE_NEXT_PAIR_MS : RINGTONE_NEXT_PULSE_MS;
}
