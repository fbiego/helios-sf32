#ifndef HELIOS_BMI270_H
#define HELIOS_BMI270_H

#include <rtthread.h>
#include <stdint.h>
#include "helios_features.h"

typedef struct
{
    int16_t accel[3]; /* Raw counts, +/-2 g. */
    int16_t gyro[3];  /* Raw counts, +/-2000 degrees/s range; zero when off. */
    uint32_t steps;   /* Since sensor initialization; resets at reboot. */
    uint32_t step_events; /* Observed detector events; steps is the total count. */
    uint32_t wrist_wake_events;
    uint8_t gyro_enabled;
    int8_t gyro_error;
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
/* Worker applies this request; no I2C is performed by the caller. */
void helios_bmi270_set_diagnostics(uint8_t enabled);

#if HELIOS_IMU_DEBUG_ORIENTATION
/* Gravity-based tilt in tenths of a degree. Not a compass/heading estimate. */
rt_err_t helios_bmi270_orientation(const helios_bmi270_sample_t *sample,
                                  int16_t *roll, int16_t *pitch);
#endif

#endif
