#ifndef HELIOS_BMI270_H
#define HELIOS_BMI270_H

#include <rtthread.h>
#include <stdint.h>
#include "helios_features.h"

typedef enum {
    HELIOS_BMI270_ACTIVITY_STILL = 0,
    HELIOS_BMI270_ACTIVITY_WALKING = 1,
    HELIOS_BMI270_ACTIVITY_RUNNING = 2,
    HELIOS_BMI270_ACTIVITY_UNKNOWN = 3
} helios_bmi270_activity_t;

typedef enum {
    HELIOS_BMI270_GESTURE_NONE = 0,
    HELIOS_BMI270_GESTURE_ARM_DOWN = 1,
    HELIOS_BMI270_GESTURE_PIVOT_UP = 2,
    HELIOS_BMI270_GESTURE_SHAKE = 3,
    HELIOS_BMI270_GESTURE_FLICK_IN = 4,
    HELIOS_BMI270_GESTURE_FLICK_OUT = 5
} helios_bmi270_gesture_t;

typedef struct
{
    int16_t accel[3]; /* Raw counts, +/-2 g. */
    int16_t gyro[3];  /* Raw counts, +/-2000 degrees/s range; zero when off. */
    float temperature_c; /* BMI270 die temperature, valid after the first reading. */
    uint8_t temperature_valid;
    uint32_t steps;   /* Since sensor initialization; resets at reboot. */
    uint32_t step_events; /* Observed detector events; steps is the total count. */
    uint32_t wrist_wake_events;
    uint8_t gyro_enabled;
    int8_t gyro_error;
    uint8_t activity, previous_activity; /* helios_bmi270_activity_t values. */
    uint8_t activity_valid;
    uint32_t activity_changes; /* Observed transitions, excluding initial baseline. */
    uint8_t wrist_gesture; /* Last helios_bmi270_gesture_t; retained between events. */
    uint32_t wrist_gesture_events; /* Repeated identical gestures count separately. */
    uint32_t wrist_gesture_tick; /* RT-Thread tick of the last successfully read event. */
    uint32_t wrist_pivot_up_events;
    uint32_t wrist_pivot_up_tick;
    int8_t activity_error, wrist_gesture_error;
} helios_bmi270_sample_t;

typedef struct
{
    const char *stage; /* Static stage name; no caller-owned storage. */
    int error;       /* Bosch API result, or RT-Thread startup error. */
    uint32_t attempts;
    uint32_t bus_hz;
    uint8_t bus_ready;
    uint8_t address;
    uint8_t probe_attempted[2];
    uint8_t probe_ack[2];
    uint8_t probe_id[2];
    uint8_t io_read;
    uint8_t io_register;
    uint32_t io_length;
    int io_result;   /* Completed I2C messages; success is 1. */
} helios_bmi270_status_t;

rt_err_t helios_bmi270_start(void);
rt_err_t helios_bmi270_read(helios_bmi270_sample_t *sample);
void helios_bmi270_get_status(helios_bmi270_status_t *status);
/* Consume a recent wrist-wake event from the UI thread. Never accesses I2C. */
uint8_t helios_bmi270_take_wake(void);
/* Poll gesture status more often while the display is awake. */
void helios_bmi270_set_gesture_polling(uint8_t enabled);
/* 0 is left, 1 is right. The sensor worker applies the change. */
void helios_bmi270_set_wearable_arm(uint8_t arm);
/* Worker applies this request; no I2C is performed by the caller. */
void helios_bmi270_set_diagnostics(uint8_t enabled);
const char *helios_bmi270_activity_name(uint8_t activity);
const char *helios_bmi270_gesture_name(uint8_t gesture);
/* Diagnostic estimate from remapped gravity and wrist-wake tilt limits. */
const char *helios_bmi270_focus_name(const helios_bmi270_sample_t *sample);

#if HELIOS_IMU_DEBUG_ORIENTATION
/* Gravity-based tilt in tenths of a degree. Not a compass/heading estimate. */
rt_err_t helios_bmi270_orientation(const helios_bmi270_sample_t *sample,
                                  int16_t *roll, int16_t *pitch);
#endif

#endif
