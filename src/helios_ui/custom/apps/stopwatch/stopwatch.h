/**
 * @file stopwatch.h
 * @brief Stopwatch runtime API
 */

#ifndef HELIOS_STOPWATCH_H
#define HELIOS_STOPWATCH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define HELIOS_STOPWATCH_LAPS_MAX      32
#define HELIOS_STOPWATCH_TITLE_MAX     16
#define HELIOS_STOPWATCH_TIME_MAX      12

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    HELIOS_STOPWATCH_STATE_RESET = 0,
    HELIOS_STOPWATCH_STATE_RUNNING = 1,
    HELIOS_STOPWATCH_STATE_PAUSED = 2,
} helios_stopwatch_state_t;

typedef struct {
    char title[HELIOS_STOPWATCH_TITLE_MAX];
    char time[HELIOS_STOPWATCH_TIME_MAX];
} helios_stopwatch_lap_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void helios_stopwatch_init(void);

void helios_stopwatch_start(void);
void helios_stopwatch_pause(void);
void helios_stopwatch_toggle(void);
void helios_stopwatch_lap_or_reset(void);
void helios_stopwatch_reset(void);
void helios_stopwatch_lap(void);

helios_stopwatch_state_t helios_stopwatch_state_get(void);
uint32_t helios_stopwatch_lap_count(void);
const helios_stopwatch_lap_t * helios_stopwatch_lap_get(uint32_t index);

void helios_stopwatch_sync_subjects(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_STOPWATCH_H*/
