/**
 * @file stopwatch.c
 * @brief Stopwatch runtime API
 */

/*********************
 *      INCLUDES
 *********************/

#include "stopwatch.h"
#include "../../../helios_ui_gen.h"
#include "../../subjects/subjects.h"

/*********************
 *      DEFINES
 *********************/

#define HELIOS_STOPWATCH_TICK_MS 10

/***********************
 *  STATIC VARIABLES
 **********************/

static lv_timer_t * stopwatch_timer;
static helios_stopwatch_lap_t laps[HELIOS_STOPWATCH_LAPS_MAX];
static uint32_t lap_count;
static uint32_t base_elapsed_ms;
static uint32_t run_started_at;
static bool running;

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void stopwatch_timer_cb(lv_timer_t * timer);
static uint32_t elapsed_ms_get(void);
static void elapsed_time_update(void);
static void elapsed_time_update_unlocked(void);
static void stopwatch_sync_subjects_unlocked(void);
static void format_time(uint32_t elapsed_ms, char * buf, uint32_t buf_size);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void helios_stopwatch_init(void)
{
    if (!stopwatch_timer) {
        stopwatch_timer = lv_timer_create(stopwatch_timer_cb, HELIOS_STOPWATCH_TICK_MS, NULL);
        lv_timer_pause(stopwatch_timer);
    }

    helios_stopwatch_sync_subjects();
}

void helios_stopwatch_start(void)
{
    lv_lock();

    if (running) {
        lv_unlock();
        return;
    }

    running = true;
    run_started_at = lv_tick_get();

    if (stopwatch_timer) lv_timer_resume(stopwatch_timer);

    elapsed_time_update_unlocked();
    stopwatch_sync_subjects_unlocked();
    lv_unlock();
}

void helios_stopwatch_pause(void)
{
    lv_lock();

    if (!running) {
        lv_unlock();
        return;
    }

    base_elapsed_ms = elapsed_ms_get();
    running = false;

    if (stopwatch_timer) lv_timer_pause(stopwatch_timer);

    elapsed_time_update_unlocked();
    stopwatch_sync_subjects_unlocked();
    lv_unlock();
}

void helios_stopwatch_toggle(void)
{
    if (running) helios_stopwatch_pause();
    else helios_stopwatch_start();
}

void helios_stopwatch_lap_or_reset(void)
{
    if (running) helios_stopwatch_lap();
    else helios_stopwatch_reset();
}

void helios_stopwatch_reset(void)
{
    lv_lock();
    running = false;
    base_elapsed_ms = 0;
    run_started_at = 0;
    lap_count = 0;

    if (stopwatch_timer) lv_timer_pause(stopwatch_timer);

    elapsed_time_update_unlocked();
    stopwatch_sync_subjects_unlocked();
    lv_unlock();
}

void helios_stopwatch_lap(void)
{
    lv_lock();

    if (!running || lap_count >= HELIOS_STOPWATCH_LAPS_MAX) {
        lv_unlock();
        return;
    }

    helios_stopwatch_lap_t * lap = &laps[lap_count];
    lv_snprintf(lap->title, sizeof(lap->title), "Lap %lu", (unsigned long)(lap_count + 1));
    format_time(elapsed_ms_get(), lap->time, sizeof(lap->time));
    lap_count++;

    stopwatch_sync_subjects_unlocked();
    lv_unlock();
}

helios_stopwatch_state_t helios_stopwatch_state_get(void)
{
    if (running) return HELIOS_STOPWATCH_STATE_RUNNING;
    if (base_elapsed_ms > 0 || lap_count > 0) return HELIOS_STOPWATCH_STATE_PAUSED;
    return HELIOS_STOPWATCH_STATE_RESET;
}

uint32_t helios_stopwatch_lap_count(void)
{
    return lap_count;
}

const helios_stopwatch_lap_t * helios_stopwatch_lap_get(uint32_t index)
{
    if (index >= lap_count) return NULL;
    return &laps[index];
}

void helios_stopwatch_sync_subjects(void)
{
    lv_lock();
    stopwatch_sync_subjects_unlocked();
    lv_unlock();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void stopwatch_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    elapsed_time_update();
}

static uint32_t elapsed_ms_get(void)
{
    if (!running) return base_elapsed_ms;
    return base_elapsed_ms + (lv_tick_get() - run_started_at);
}

static void elapsed_time_update(void)
{
    lv_lock();
    elapsed_time_update_unlocked();
    lv_unlock();
}

static void elapsed_time_update_unlocked(void)
{
    char buf[HELIOS_STOPWATCH_TIME_MAX];
    format_time(elapsed_ms_get(), buf, sizeof(buf));
    lv_subject_copy_string(&sb_stopwatch_time, buf);
}

static void stopwatch_sync_subjects_unlocked(void)
{
    char buf[HELIOS_STOPWATCH_TIME_MAX];
    helios_stopwatch_state_t state = helios_stopwatch_state_get();

    format_time(elapsed_ms_get(), buf, sizeof(buf));

    lv_subject_copy_string(&sb_stopwatch_time, buf);
    lv_subject_set_int(&sb_stopwatch_state, state);
    lv_subject_set_int(&sb_stopwatch_button_start_state, running ? 1 : 0);
    lv_subject_set_int(&sb_stopwatch_button_reset_state, running ? 1 : 0);
    lv_subject_set_pointer(&sb_stopwatch_button_start_icon, (void *)(running ? icon_timer_pause : icon_timer_start));
    lv_subject_set_pointer(&sb_stopwatch_button_reset_icon, (void *)(running ? icon_lap_time : icon_restart));
}

static void format_time(uint32_t elapsed_ms, char * buf, uint32_t buf_size)
{
    uint32_t total_cs = elapsed_ms / 10;
    uint32_t minutes = total_cs / 6000;
    uint32_t seconds = (total_cs / 100) % 60;
    uint32_t centiseconds = total_cs % 100;

    if (minutes > 99) minutes = 99;
    lv_snprintf(buf,
                buf_size,
                "%02lu:%02lu:%02lu",
                (unsigned long)minutes,
                (unsigned long)seconds,
                (unsigned long)centiseconds);
}
