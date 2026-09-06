#include "helios_max30100.h"

#include <math.h>
#include <rtdevice.h>
#include <string.h>

#include "helios_platform.h"

#define LOG_TAG "helios.max30100"
#include "log.h"

#define MAX30100_I2C_ADDR 0x57

#define MAX30100_REG_INT_STATUS 0x00
#define MAX30100_REG_INT_ENABLE 0x01
#define MAX30100_REG_FIFO_WR_PTR 0x02
#define MAX30100_REG_OVF_COUNTER 0x03
#define MAX30100_REG_FIFO_RD_PTR 0x04
#define MAX30100_REG_FIFO_DATA 0x05
#define MAX30100_REG_MODE_CONFIG 0x06
#define MAX30100_REG_SPO2_CONFIG 0x07
#define MAX30100_REG_LED_CONFIG 0x09
#define MAX30100_REG_PART_ID 0xFF

#define MAX30100_PART_ID 0x11
#define MAX30100_MODE_SHUTDOWN 0x80
#define MAX30100_MODE_RESET 0x40
#define MAX30100_MODE_SPO2_HR 0x03

#define MAX30100_INT_FIFO_ALMOST_FULL 0x80
#define MAX30100_INT_SPO2_READY 0x10
#define MAX30100_INT_HR_READY 0x20

#define MAX30100_POLL_MS 20
#define MAX30100_FIFO_DEPTH 16
#define MAX30100_THREAD_STACK_SIZE 4096
#define MAX30100_THREAD_PRIORITY 24
#define MAX30100_THREAD_TICK 10

#define MAX30100_SAMPLING_PERIOD_MS 10
#define MAX30100_INIT_HOLDOFF_MS 2000
#define MAX30100_MASKING_HOLDOFF_MS 200
#define MAX30100_INVALID_READOUT_DELAY_MS 2000
#define MAX30100_BEAT_PERIOD_ALPHA 0.6f
#define MAX30100_MIN_THRESHOLD 20.0f
#define MAX30100_MAX_THRESHOLD 800.0f
#define MAX30100_STEP_RESILIENCY 30.0f
#define MAX30100_THRESHOLD_FALLOFF_TARGET 0.3f
#define MAX30100_THRESHOLD_DECAY_FACTOR 0.99f
#define MAX30100_DC_REMOVER_ALPHA 0.95f
#define MAX30100_CURRENT_ADJUSTMENT_PERIOD_MS 500
#define MAX30100_CURRENT_DC_DELTA 70000.0f
#define MAX30100_IR_LED_CURRENT 0x0F
#define MAX30100_RED_LED_CURRENT_START 0x08
#define MAX30100_LED_CURRENT_MAX 0x0F
#define MAX30100_SPO2_BEATS 3
#define MAX30100_SPO2_LUT_SIZE 43
#define MAX30100_DEFAULT_CACHE_TIMEOUT_MS 8000
#define MAX30100_DEFAULT_FINGER_IR_THRESHOLD 10000

typedef enum
{
    MAX30100_BEAT_INIT,
    MAX30100_BEAT_WAITING,
    MAX30100_BEAT_FOLLOWING_SLOPE,
    MAX30100_BEAT_MAYBE_DETECTED,
    MAX30100_BEAT_MASKING
} max30100_beat_state_t;

typedef struct
{
    struct rt_i2c_bus_device *bus;
    rt_thread_t thread;
    volatile uint8_t measuring;
    volatile uint8_t thread_running;
    helios_max30100_update_cb_t callback;
    void *user_data;
    uint32_t start_ms;
    uint32_t last_beat_ms;
    max30100_beat_state_t beat_state;
    float beat_threshold;
    float beat_period_ms;
    float last_max_value;
    float ir_dcw;
    float red_dcw;
    float lpf_v[2];
    float ir_ac_sq_sum;
    float red_ac_sq_sum;
    uint32_t spo2_samples_recorded;
    uint8_t spo2_beats_detected;
    uint8_t spo2;
    uint8_t detecting;
    uint8_t red_led_current;
    uint32_t last_bias_check_ms;
    uint32_t cache_timeout_ms;
    uint16_t finger_ir_threshold;
    uint32_t last_valid_ms;
    int16_t cached_bpm;
    uint8_t cached_spo2;
} helios_max30100_state_t;

static helios_max30100_state_t g_max30100;

static const uint8_t g_max30100_spo2_lut[MAX30100_SPO2_LUT_SIZE] = {
    100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98,
    98, 97, 97, 97, 97, 97, 97, 96, 96, 96, 96, 96, 96, 95,
    95, 95, 95, 95, 95, 94, 94, 94, 94, 94, 93, 93, 93, 93,
    93};

static rt_err_t max30100_write_reg(uint8_t reg, uint8_t value)
{
#ifdef RT_USING_I2C
    if (!g_max30100.bus)
        return -RT_ERROR;

    return rt_i2c_mem_write(g_max30100.bus, MAX30100_I2C_ADDR, reg, 8, &value, 1) == 1
               ? RT_EOK
               : -RT_ERROR;
#else
    (void)reg;
    (void)value;
    return -RT_ENOSYS;
#endif
}

static rt_err_t max30100_read_reg(uint8_t reg, uint8_t *value)
{
#ifdef RT_USING_I2C
    if (!g_max30100.bus || !value)
        return -RT_ERROR;

    return rt_i2c_mem_read(g_max30100.bus, MAX30100_I2C_ADDR, reg, 8, value, 1) == 1
               ? RT_EOK
               : -RT_ERROR;
#else
    (void)reg;
    (void)value;
    return -RT_ENOSYS;
#endif
}

static rt_err_t max30100_read_fifo(uint16_t *ir, uint16_t *red)
{
#ifdef RT_USING_I2C
    uint8_t fifo[4] = {0};

    if (!g_max30100.bus || !ir || !red)
        return -RT_ERROR;

    if (rt_i2c_mem_read(g_max30100.bus, MAX30100_I2C_ADDR, MAX30100_REG_FIFO_DATA, 8,
                        fifo, sizeof(fifo)) != sizeof(fifo))
        return -RT_ERROR;

    *ir = ((uint16_t)fifo[0] << 8) | fifo[1];
    *red = ((uint16_t)fifo[2] << 8) | fifo[3];
    return RT_EOK;
#else
    (void)ir;
    (void)red;
    return -RT_ENOSYS;
#endif
}

static void max30100_set_default_options(void)
{
    if (g_max30100.cache_timeout_ms == 0)
        g_max30100.cache_timeout_ms = MAX30100_DEFAULT_CACHE_TIMEOUT_MS;
    if (g_max30100.finger_ir_threshold == 0)
        g_max30100.finger_ir_threshold = MAX30100_DEFAULT_FINGER_IR_THRESHOLD;
}

static void max30100_reset_processing(void)
{
    g_max30100.start_ms = rt_tick_get_millisecond();
    g_max30100.last_beat_ms = 0;
    g_max30100.beat_state = MAX30100_BEAT_INIT;
    g_max30100.beat_threshold = MAX30100_MIN_THRESHOLD;
    g_max30100.beat_period_ms = 0.0f;
    g_max30100.last_max_value = 0.0f;
    g_max30100.ir_dcw = 0.0f;
    g_max30100.red_dcw = 0.0f;
    g_max30100.lpf_v[0] = 0.0f;
    g_max30100.lpf_v[1] = 0.0f;
    g_max30100.ir_ac_sq_sum = 0.0f;
    g_max30100.red_ac_sq_sum = 0.0f;
    g_max30100.spo2_samples_recorded = 0;
    g_max30100.spo2_beats_detected = 0;
    g_max30100.spo2 = 0;
    g_max30100.detecting = 0;
    g_max30100.last_bias_check_ms = g_max30100.start_ms;
}

static void max30100_reset_filter(void)
{
    max30100_reset_processing();
    g_max30100.red_led_current = MAX30100_RED_LED_CURRENT_START;
    g_max30100.last_valid_ms = 0;
    g_max30100.cached_bpm = 0;
    g_max30100.cached_spo2 = 0;
}

static float max30100_min_float(float a, float b)
{
    return a < b ? a : b;
}

static float max30100_dc_remove(float sample, float *dcw)
{
    float previous = *dcw;

    *dcw = sample + MAX30100_DC_REMOVER_ALPHA * (*dcw);
    return *dcw - previous;
}

static float max30100_lowpass_step(float sample)
{
    g_max30100.lpf_v[0] = g_max30100.lpf_v[1];
    g_max30100.lpf_v[1] = 0.2452372752527856f * sample +
                           0.5095254494944288f * g_max30100.lpf_v[0];
    return g_max30100.lpf_v[0] + g_max30100.lpf_v[1];
}

static void max30100_decrease_threshold(void)
{
    if (g_max30100.last_max_value > 0.0f && g_max30100.beat_period_ms > 0.0f)
    {
        g_max30100.beat_threshold -=
            g_max30100.last_max_value * (1.0f - MAX30100_THRESHOLD_FALLOFF_TARGET) /
            (g_max30100.beat_period_ms / MAX30100_SAMPLING_PERIOD_MS);
    }
    else
    {
        g_max30100.beat_threshold *= MAX30100_THRESHOLD_DECAY_FACTOR;
    }

    if (g_max30100.beat_threshold < MAX30100_MIN_THRESHOLD)
        g_max30100.beat_threshold = MAX30100_MIN_THRESHOLD;
}

static uint8_t max30100_check_for_beat(float sample, uint32_t now_ms)
{
    uint8_t beat_detected = 0;

    switch (g_max30100.beat_state)
    {
    case MAX30100_BEAT_INIT:
        if (now_ms - g_max30100.start_ms > MAX30100_INIT_HOLDOFF_MS)
            g_max30100.beat_state = MAX30100_BEAT_WAITING;
        break;
    case MAX30100_BEAT_WAITING:
        if (sample > g_max30100.beat_threshold)
        {
            g_max30100.beat_threshold =
                max30100_min_float(sample, MAX30100_MAX_THRESHOLD);
            g_max30100.beat_state = MAX30100_BEAT_FOLLOWING_SLOPE;
        }

        if (g_max30100.last_beat_ms &&
            now_ms - g_max30100.last_beat_ms > MAX30100_INVALID_READOUT_DELAY_MS)
        {
            g_max30100.beat_period_ms = 0.0f;
            g_max30100.last_max_value = 0.0f;
        }

        max30100_decrease_threshold();
        break;
    case MAX30100_BEAT_FOLLOWING_SLOPE:
        if (sample < g_max30100.beat_threshold)
            g_max30100.beat_state = MAX30100_BEAT_MAYBE_DETECTED;
        else
            g_max30100.beat_threshold =
                max30100_min_float(sample, MAX30100_MAX_THRESHOLD);
        break;
    case MAX30100_BEAT_MAYBE_DETECTED:
        if (sample + MAX30100_STEP_RESILIENCY < g_max30100.beat_threshold)
        {
            beat_detected = 1;
            g_max30100.last_max_value = sample;
            g_max30100.beat_state = MAX30100_BEAT_MASKING;
            if (g_max30100.last_beat_ms)
            {
                float delta = (float)(now_ms - g_max30100.last_beat_ms);

                g_max30100.beat_period_ms =
                    MAX30100_BEAT_PERIOD_ALPHA * delta +
                    (1.0f - MAX30100_BEAT_PERIOD_ALPHA) * g_max30100.beat_period_ms;
            }
            g_max30100.last_beat_ms = now_ms;
        }
        else
        {
            g_max30100.beat_state = MAX30100_BEAT_FOLLOWING_SLOPE;
        }
        break;
    case MAX30100_BEAT_MASKING:
        if (now_ms - g_max30100.last_beat_ms > MAX30100_MASKING_HOLDOFF_MS)
            g_max30100.beat_state = MAX30100_BEAT_WAITING;
        max30100_decrease_threshold();
        break;
    }

    return beat_detected;
}

static void max30100_reset_spo2(void)
{
    g_max30100.spo2_samples_recorded = 0;
    g_max30100.red_ac_sq_sum = 0.0f;
    g_max30100.ir_ac_sq_sum = 0.0f;
    g_max30100.spo2_beats_detected = 0;
    g_max30100.spo2 = 0;
}

static void max30100_update_spo2(float ir_ac, float red_ac, uint8_t beat_detected)
{
    g_max30100.ir_ac_sq_sum += ir_ac * ir_ac;
    g_max30100.red_ac_sq_sum += red_ac * red_ac;
    g_max30100.spo2_samples_recorded++;

    if (beat_detected)
    {
        g_max30100.spo2_beats_detected++;
        if (g_max30100.spo2_beats_detected == MAX30100_SPO2_BEATS)
        {
            uint8_t spo2 = 0;
            float ir_mean = g_max30100.ir_ac_sq_sum /
                            (float)g_max30100.spo2_samples_recorded;
            float red_mean = g_max30100.red_ac_sq_sum /
                             (float)g_max30100.spo2_samples_recorded;
            float ac_sq_ratio;
            uint8_t index = 0;

            if (ir_mean > 1.0f && red_mean > 1.0f)
            {
                ac_sq_ratio = 100.0f * logf(red_mean) / logf(ir_mean);
                if (ac_sq_ratio > 66.0f)
                    index = (uint8_t)(ac_sq_ratio - 66.0f);
                else if (ac_sq_ratio > 50.0f)
                    index = (uint8_t)(ac_sq_ratio - 50.0f);

                if (index >= MAX30100_SPO2_LUT_SIZE)
                    index = MAX30100_SPO2_LUT_SIZE - 1;

                spo2 = g_max30100_spo2_lut[index];
            }

            max30100_reset_spo2();
            g_max30100.spo2 = spo2;
        }
    }
}

static void max30100_adjust_current_bias(uint32_t now_ms)
{
    float delta;
    uint8_t changed = 0;

    if (now_ms - g_max30100.last_bias_check_ms <= MAX30100_CURRENT_ADJUSTMENT_PERIOD_MS)
        return;

    delta = g_max30100.ir_dcw - g_max30100.red_dcw;
    if (delta > MAX30100_CURRENT_DC_DELTA &&
        g_max30100.red_led_current < MAX30100_LED_CURRENT_MAX)
    {
        g_max30100.red_led_current++;
        changed = 1;
    }
    else if (-delta > MAX30100_CURRENT_DC_DELTA &&
             g_max30100.red_led_current > 0)
    {
        g_max30100.red_led_current--;
        changed = 1;
    }

    if (changed)
    {
        max30100_write_reg(MAX30100_REG_LED_CONFIG,
                           (g_max30100.red_led_current << 4) |
                               MAX30100_IR_LED_CURRENT);
    }

    g_max30100.last_bias_check_ms = now_ms;
}

static void max30100_process_sample(helios_max30100_sample_t *sample)
{
    float ir_ac;
    float red_ac;
    float filtered_pulse;
    uint8_t beat_detected;

    if (!sample)
        return;

    sample->finger_detected =
        sample->ir >= g_max30100.finger_ir_threshold ? 1 : 0;

    if (!sample->finger_detected)
    {
        uint32_t age_ms = g_max30100.last_valid_ms
                              ? sample->timestamp_ms - g_max30100.last_valid_ms
                              : g_max30100.cache_timeout_ms + 1;

        max30100_reset_processing();
        sample->beat_detected = 0;
        if (g_max30100.last_valid_ms && age_ms <= g_max30100.cache_timeout_ms)
        {
            sample->cached = 1;
            sample->heart_rate_bpm = g_max30100.cached_bpm;
            sample->spo2_percent = g_max30100.cached_spo2;
        }
        else
        {
            sample->cached = 0;
            sample->heart_rate_bpm = 0;
            sample->spo2_percent = 0;
        }
        return;
    }

    ir_ac = max30100_dc_remove((float)sample->ir, &g_max30100.ir_dcw);
    red_ac = max30100_dc_remove((float)sample->red, &g_max30100.red_dcw);
    filtered_pulse = max30100_lowpass_step(-ir_ac);
    beat_detected = max30100_check_for_beat(filtered_pulse, sample->timestamp_ms);

    sample->beat_detected = beat_detected;
    sample->heart_rate_bpm = g_max30100.beat_period_ms > 0.0f
                                 ? (int16_t)(60000.0f / g_max30100.beat_period_ms)
                                 : 0;

    if (sample->heart_rate_bpm > 0)
    {
        g_max30100.detecting = 1;
        max30100_update_spo2(ir_ac, red_ac, beat_detected);
    }
    else if (g_max30100.detecting)
    {
        g_max30100.detecting = 0;
        max30100_reset_spo2();
    }

    sample->spo2_percent = g_max30100.spo2;
    sample->cached = 0;
    if (sample->heart_rate_bpm > 0 || sample->spo2_percent > 0)
    {
        g_max30100.last_valid_ms = sample->timestamp_ms;
        if (sample->heart_rate_bpm > 0)
            g_max30100.cached_bpm = sample->heart_rate_bpm;
        if (sample->spo2_percent > 0)
            g_max30100.cached_spo2 = sample->spo2_percent;
    }
    max30100_adjust_current_bias(sample->timestamp_ms);
}

static uint8_t max30100_fifo_available(uint8_t wr_ptr, uint8_t rd_ptr, uint8_t overflow)
{
    wr_ptr &= 0x0F;
    rd_ptr &= 0x0F;

    if (overflow)
        return MAX30100_FIFO_DEPTH;

    return (wr_ptr - rd_ptr) & 0x0F;
}

static rt_err_t max30100_configure_measurement(void)
{
    rt_err_t err;

    err = max30100_write_reg(MAX30100_REG_MODE_CONFIG, MAX30100_MODE_RESET);
    if (err != RT_EOK)
        return err;

    rt_thread_mdelay(10);

    max30100_write_reg(MAX30100_REG_FIFO_WR_PTR, 0);
    max30100_write_reg(MAX30100_REG_OVF_COUNTER, 0);
    max30100_write_reg(MAX30100_REG_FIFO_RD_PTR, 0);

    err = max30100_write_reg(MAX30100_REG_SPO2_CONFIG, 0x47);
    if (err != RT_EOK)
        return err;

    err = max30100_write_reg(MAX30100_REG_LED_CONFIG,
                             (MAX30100_RED_LED_CURRENT_START << 4) |
                                 MAX30100_IR_LED_CURRENT);
    if (err != RT_EOK)
        return err;

    err = max30100_write_reg(MAX30100_REG_INT_ENABLE,
                             MAX30100_INT_FIFO_ALMOST_FULL |
                                 MAX30100_INT_SPO2_READY |
                                 MAX30100_INT_HR_READY);
    if (err != RT_EOK)
        return err;

    return max30100_write_reg(MAX30100_REG_MODE_CONFIG, MAX30100_MODE_SPO2_HR);
}

static void max30100_thread_entry(void *parameter)
{
    (void)parameter;

    while (g_max30100.measuring)
    {
        helios_max30100_update(RT_NULL);
        rt_thread_mdelay(MAX30100_POLL_MS);
    }
    g_max30100.thread_running = 0;
    g_max30100.thread = RT_NULL;
}

rt_bool_t helios_max30100_begin(const char *i2c_name)
{
    uint8_t part_id = 0;
    uint32_t cache_timeout_ms = g_max30100.cache_timeout_ms;
    uint16_t finger_ir_threshold = g_max30100.finger_ir_threshold;

    memset(&g_max30100, 0, sizeof(g_max30100));
    g_max30100.cache_timeout_ms = cache_timeout_ms;
    g_max30100.finger_ir_threshold = finger_ir_threshold;
    max30100_set_default_options();

    g_max30100.bus = helios_i2c_bus(i2c_name, 400000);
    if (!g_max30100.bus)
    {
        LOG_W("i2c bus not found");
        return RT_FALSE;
    }

    if (max30100_read_reg(MAX30100_REG_PART_ID, &part_id) != RT_EOK ||
        part_id != MAX30100_PART_ID)
    {
        LOG_W("MAX30100 not found, part id=0x%02x", part_id);
        memset(&g_max30100, 0, sizeof(g_max30100));
        return RT_FALSE;
    }

    max30100_write_reg(MAX30100_REG_MODE_CONFIG, MAX30100_MODE_RESET);
    rt_thread_mdelay(10);
    max30100_write_reg(MAX30100_REG_MODE_CONFIG, MAX30100_MODE_SHUTDOWN);
    max30100_reset_filter();

    LOG_I("MAX30100 ready");
    return RT_TRUE;
}

void helios_max30100_set_cache_timeout_ms(uint32_t timeout_ms)
{
    g_max30100.cache_timeout_ms = timeout_ms;
}

void helios_max30100_set_finger_threshold(uint16_t ir_threshold)
{
    g_max30100.finger_ir_threshold = ir_threshold;
}

rt_err_t helios_max30100_start_measuring(helios_max30100_update_cb_t callback,
                                         void *user_data)
{
    rt_err_t err;

    if (!g_max30100.bus)
        return -RT_ERROR;

    if (g_max30100.measuring)
        return RT_EOK;

    g_max30100.callback = callback;
    g_max30100.user_data = user_data;
    max30100_set_default_options();
    max30100_reset_filter();

    err = max30100_configure_measurement();
    if (err != RT_EOK)
        return err;

    g_max30100.measuring = 1;
    if (callback)
    {
        g_max30100.thread = rt_thread_create("max30100", max30100_thread_entry, RT_NULL,
                                             MAX30100_THREAD_STACK_SIZE,
                                             MAX30100_THREAD_PRIORITY,
                                             MAX30100_THREAD_TICK);
        if (!g_max30100.thread)
        {
            g_max30100.measuring = 0;
            g_max30100.thread_running = 0;
            max30100_write_reg(MAX30100_REG_MODE_CONFIG, MAX30100_MODE_SHUTDOWN);
            return -RT_ENOMEM;
        }
        g_max30100.thread_running = 1;
        rt_thread_startup(g_max30100.thread);
    }

    return RT_EOK;
}

void helios_max30100_stop_measuring(void)
{
    if (!g_max30100.bus)
        return;

    g_max30100.measuring = 0;
    while (g_max30100.thread_running)
        rt_thread_mdelay(MAX30100_POLL_MS);

    max30100_write_reg(MAX30100_REG_INT_ENABLE, 0);
    max30100_write_reg(MAX30100_REG_MODE_CONFIG, MAX30100_MODE_SHUTDOWN);
}

rt_bool_t helios_max30100_update(helios_max30100_sample_t *sample)
{
    uint8_t int_status = 0;
    uint8_t wr_ptr = 0;
    uint8_t rd_ptr = 0;
    uint8_t overflow = 0;
    uint8_t available;
    uint8_t index;
    rt_bool_t got_sample = RT_FALSE;
    helios_max30100_sample_t current;

    if (!g_max30100.bus || !g_max30100.measuring)
        return RT_FALSE;

    max30100_read_reg(MAX30100_REG_INT_STATUS, &int_status);
    (void)int_status;
    if (max30100_read_reg(MAX30100_REG_FIFO_WR_PTR, &wr_ptr) != RT_EOK ||
        max30100_read_reg(MAX30100_REG_FIFO_RD_PTR, &rd_ptr) != RT_EOK ||
        max30100_read_reg(MAX30100_REG_OVF_COUNTER, &overflow) != RT_EOK)
        return RT_FALSE;

    available = max30100_fifo_available(wr_ptr, rd_ptr, overflow);
    if (!available)
        return RT_FALSE;

    if (overflow)
    {
        LOG_W("MAX30100 fifo overflow=%u, draining", overflow);
        max30100_write_reg(MAX30100_REG_OVF_COUNTER, 0);
    }

    for (index = 0; index < available; index++)
    {
        memset(&current, 0, sizeof(current));
        if (max30100_read_fifo(&current.ir, &current.red) != RT_EOK)
            break;

        current.timestamp_ms = rt_tick_get_millisecond();
        current.heart_rate_bpm = 0;
        current.spo2_percent = g_max30100.spo2;
        max30100_process_sample(&current);

        if (sample)
            *sample = current;

        if (g_max30100.callback)
            g_max30100.callback(&current, g_max30100.user_data);

        got_sample = RT_TRUE;
    }

    return got_sample;
}

void helios_max30100_close(void)
{
    uint32_t cache_timeout_ms = g_max30100.cache_timeout_ms;
    uint16_t finger_ir_threshold = g_max30100.finger_ir_threshold;

    helios_max30100_stop_measuring();
    memset(&g_max30100, 0, sizeof(g_max30100));
    g_max30100.cache_timeout_ms = cache_timeout_ms;
    g_max30100.finger_ir_threshold = finger_ir_threshold;
}

rt_bool_t helios_max30100_is_measuring(void)
{
    return g_max30100.measuring ? RT_TRUE : RT_FALSE;
}
