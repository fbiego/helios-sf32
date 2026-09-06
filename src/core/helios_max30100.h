#ifndef HELIOS_MAX30100_H
#define HELIOS_MAX30100_H

#include <rtthread.h>
#include <stdint.h>

typedef struct
{
    uint16_t ir;
    uint16_t red;
    uint32_t timestamp_ms;
    uint8_t finger_detected;
    uint8_t cached;
    uint8_t beat_detected;
    int16_t heart_rate_bpm;
    uint8_t spo2_percent;
} helios_max30100_sample_t;

typedef void (*helios_max30100_update_cb_t)(const helios_max30100_sample_t *sample,
                                            void *user_data);

rt_bool_t helios_max30100_begin(const char *i2c_name);
rt_err_t helios_max30100_start_measuring(helios_max30100_update_cb_t callback,
                                         void *user_data);
void helios_max30100_set_cache_timeout_ms(uint32_t timeout_ms);
void helios_max30100_set_finger_threshold(uint16_t ir_threshold);
void helios_max30100_stop_measuring(void);
rt_bool_t helios_max30100_update(helios_max30100_sample_t *sample);
void helios_max30100_close(void);
rt_bool_t helios_max30100_is_measuring(void);

#endif
