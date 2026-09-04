/**
 * @file events.h
 *
 */

#ifndef EVENTS_H
#define EVENTS_H

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

/**********************
*      TYPEDEFS
**********************/

/**********************
* GLOBAL PROTOTYPES
**********************/
void screen_applications_events_cb(lv_event_t *e);
void screen_contacts_events_cb(lv_event_t *e);
void screen_notifications_events_cb(lv_event_t *e);
void screen_stopwatch_events_cb(lv_event_t *e);
void screen_weather_events_cb(lv_event_t *e);
void on_hs_title_pill_cb(lv_event_t * e);

/**********************
*      MACROS
**********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*EVENTS_H*/
