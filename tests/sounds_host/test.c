#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <rtthread.h>

rt_base_t rt_hw_interrupt_disable(void) { return 0; }
void rt_hw_interrupt_enable(rt_base_t level) { (void)level; }

/* Replace only the hardware boundary; exercise the real sound policy. */
#define HELIOS_PLATFORM_H
static unsigned tones;
static uint32_t duration;
static unsigned silences;
static unsigned motor_pulses, motor_silences;
static uint32_t motor_duration;
static int helios_motor_pulse(uint32_t milliseconds)
{
    ++motor_pulses;
    motor_duration = milliseconds;
    return 0;
}
static void helios_motor_silence(void) { ++motor_silences; }
static void helios_buzzer_silence(void) { ++silences; }
static int helios_buzzer_tone(uint32_t milliseconds)
{
    ++tones;
    duration = milliseconds;
    return 0;
}
#include "core/helios_sounds.c"

static void poll(time_t now)
{
    struct tm *calendar = gmtime(&now);
    helios_sounds_poll(now, calendar);
}

int main(void)
{
    helios_sounds_notification();
    helios_sounds_ringtone(true);
    helios_sounds_ringtone_poll(0);
    assert(!tones && !motor_pulses);
    helios_sounds_ringtone(false);
    helios_sounds_set_notification_sound(true);
    const time_t midnight = 1767225600; /* 2026-01-01 00:00 UTC */
    poll(midnight); assert(tones == 0); /* Boot exactly on the hour. */
    poll(midnight + 1); assert(tones == 0);
    poll(midnight + 3599); assert(tones == 0);
    poll(midnight + 3600); assert(tones == 0); /* Off until phone enables it. */
    helios_sounds_set_hourly(true);
    poll(midnight + 3601); assert(tones == 0); /* No replay after enabling. */
    clock_ready = chimed = false;
    poll(midnight + 3599);
    poll(midnight + 3600); assert(tones == 1 && duration == 400);
    poll(midnight + 3600); poll(midnight + 3601); assert(tones == 1);
    poll(midnight + 3599); poll(midnight + 3600); assert(tones == 1);
    poll(midnight + 7198); poll(midnight + 7202); assert(tones == 2);
    poll(midnight + 7203); assert(tones == 2);
    poll(midnight + 10800); assert(tones == 2); /* Large forward sync. */
    poll(midnight + 14399); poll(midnight + 14406); assert(tones == 2); /* Too late. */
    poll(midnight + 86399); poll(midnight + 86400); assert(tones == 3); /* Midnight. */
    helios_sounds_poll(midnight, NULL);
    poll(midnight + 90000); assert(tones == 3); /* Invalid clock resets baseline. */
    poll(0); poll(midnight + 93600); assert(tones == 3);
    helios_sounds_notification(); assert(tones == 4 && duration == 120);
    helios_sounds_notification(); assert(tones == 5 && duration == 120);
    helios_sounds_alarm(true); assert(tones == 6 && duration == 150);
    helios_sounds_notification(); helios_sounds_startup(); assert(tones == 6);
    helios_sounds_alarm(false); assert(silences == 1);
    helios_sounds_notification(); assert(tones == 7 && duration == 120);
    /* Every boundary below has a fresh scheduling baseline. */
    unsigned before = tones;
    helios_sounds_set_sleep(true, 22 * 60, 7 * 60);
    for (unsigned hour = 0; hour < 24; ++hour) {
        time_t boundary = midnight + 86400 * 2 + hour * 3600;
        poll(boundary - 1); poll(boundary);
        if (hour >= 7 && hour < 22) before++;
        assert(tones == before);
    }
    helios_sounds_set_sleep(true, 9 * 60 + 30, 12 * 60);
    for (unsigned hour = 9; hour <= 12; ++hour) {
        time_t boundary = midnight + 86400 * 3 + hour * 3600;
        poll(boundary - 1); poll(boundary);
        if (hour == 9 || hour == 12) before++;
        assert(tones == before);
    }
    helios_sounds_set_sleep(true, 0, 0); /* Equal bounds mean all day. */
    poll(midnight + 86400 * 4 - 1); poll(midnight + 86400 * 4);
    assert(tones == before);
    helios_sounds_set_sleep(false, 0, 0);
    poll(midnight + 86400 * 4 + 3599); poll(midnight + 86400 * 4 + 3600);
    assert(tones == ++before);
    helios_sounds_set_hourly(false);
    poll(midnight + 86400 * 4 + 7199); poll(midnight + 86400 * 4 + 7200);
    assert(tones == before);

    /* First pulse plays immediately, even just before the tick wraps. */
    uint32_t first_pulse = UINT32_MAX - 100;
    helios_sounds_ringtone(true);
    helios_sounds_ringtone_poll(first_pulse);
    assert(tones == ++before && duration == 220);
    helios_sounds_ringtone(true);
    helios_sounds_notification();
    helios_sounds_ringtone_poll(first_pulse + 329);
    assert(tones == before);
    helios_sounds_ringtone_poll(first_pulse + 330);
    assert(tones == ++before);
    helios_sounds_ringtone_poll(first_pulse + 1829);
    assert(tones == before);
    helios_sounds_ringtone_poll(first_pulse + 1830);
    assert(tones == ++before);
    helios_sounds_ringtone(false);
    assert(silences == 2);
    helios_sounds_ringtone_poll(first_pulse + 2160);
    assert(tones == before);

    helios_sounds_alarm(true);
    assert(tones == ++before && duration == 150);
    helios_sounds_ringtone(true);
    helios_sounds_ringtone_poll(3000);
    assert(tones == before);
    helios_sounds_ringtone(false);
    assert(silences == 2); /* Dismissing the call must leave the alarm alone. */
    helios_sounds_ringtone(true);
    helios_sounds_alarm(false);
    helios_sounds_ringtone_poll(3001);
    assert(silences == 3 && tones == ++before && duration == 220);
    helios_sounds_set_notification_sound(false);
    assert(silences == 4);
    helios_sounds_set_notification_vibration(true);
    helios_sounds_ringtone_poll(3331);
    assert(tones == before && motor_pulses == 1 && motor_duration == 220);
    helios_sounds_ringtone(false);
    assert(silences == 4 && motor_silences == 1);
    helios_sounds_notification();
    assert(tones == before && motor_pulses == 2 && motor_duration == 120);
    helios_sounds_set_notification_vibration(false);
    helios_sounds_notification();
    assert(motor_pulses == 2 && motor_silences == 2);
    puts("Sound policy: hourly chimes, notifications, ringtone cadence and alarm priority passed");
    return 0;
}
