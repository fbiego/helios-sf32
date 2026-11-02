#include "helios_features.h"

#if HELIOS_HAS_BMI270
#include "helios_bmi270.h"
#include "helios_platform.h"
#include "drivers/bmi270/bmi270.h"
#include <board.h>
#include <rthw.h>
#if HELIOS_IMU_DEBUG_ORIENTATION
#include <math.h>
#endif
#include <string.h>

#define LOG_TAG "helios.bmi270"
#include "log.h"

#if !defined(RT_USING_I2C) || !defined(BSP_USING_I2C2)
#error BMI270 requires RT_USING_I2C and BSP_USING_I2C2
#endif

#define BMI270_TRANSFER_LEN 32
#define BMI270_WAKE_DEBOUNCE_MS 1500
#define BMI270_WAKE_EXPIRY_MS 2000
#ifndef HELIOS_BMI270_I2C_HZ
#define HELIOS_BMI270_I2C_HZ 100000
#endif

static struct rt_i2c_bus_device *g_bus;
static uint8_t g_address;
static struct bmi2_dev g_device;
static helios_bmi270_sample_t g_sample;
static uint8_t g_valid;
static rt_thread_t g_thread;
static struct rt_semaphore g_event_sem;
static uint8_t g_diagnostics_requested;
static uint8_t g_gyro_enabled;
static helios_bmi270_status_t g_status = {
    .stage = "not started", .error = -RT_ERROR, .bus_hz = HELIOS_BMI270_I2C_HZ
};

static void bmi_status_stage(const char *stage, int error)
{
    rt_base_t level = rt_hw_interrupt_disable();
    g_status.stage = stage;
    g_status.error = error;
    rt_hw_interrupt_enable(level);
}

void helios_bmi270_get_status(helios_bmi270_status_t *status)
{
    if (!status)
        return;
    rt_base_t level = rt_hw_interrupt_disable();
    *status = g_status;
    rt_hw_interrupt_enable(level);
}

static void bmi_record_io(uint8_t read, uint8_t reg, uint32_t length, int result)
{
    rt_base_t level = rt_hw_interrupt_disable();
    g_status.io_read = read;
    g_status.io_register = reg;
    g_status.io_length = length;
    g_status.io_result = result;
    rt_hw_interrupt_enable(level);
}
#if HELIOS_IMU_RAISE_TO_WAKE
static uint8_t g_wake_pending;
static uint8_t g_have_last_wake;
static rt_tick_t g_last_wake;
#endif

#if HELIOS_IMU_RAISE_TO_WAKE || HELIOS_IMU_STEPS
static void bmi_irq(void *context)
{
    (void)context;
    /* I2C and LVGL calls belong to worker/UI threads, never this ISR. */
    rt_sem_release(&g_event_sem);
}

static rt_err_t bmi_irq_start(void)
{
    rt_err_t result;
    HAL_PIN_Set(HELIOS_BMI270_INT1_PAD, HELIOS_BMI270_INT1_GPIO, PIN_PULLDOWN, 1);
    rt_pin_mode(HELIOS_BMI270_INT1_PIN, PIN_MODE_INPUT);
    result = rt_pin_attach_irq(HELIOS_BMI270_INT1_PIN, PIN_IRQ_MODE_RISING, bmi_irq, RT_NULL);
    if (result != RT_EOK)
        return result;
    result = rt_pin_irq_enable(HELIOS_BMI270_INT1_PIN, PIN_IRQ_ENABLE);
    if (result != RT_EOK)
        rt_pin_detach_irq(HELIOS_BMI270_INT1_PIN);
    return result;
}
#endif

static void bmi_handle_events(uint16_t status, helios_bmi270_sample_t *sample)
{
#if HELIOS_IMU_STEPS
    if (status & BMI270_STEP_CNT_STATUS_MASK)
        sample->step_events++;
#endif
#if HELIOS_IMU_RAISE_TO_WAKE
    if (status & BMI270_WRIST_WAKE_UP_STATUS_MASK)
    {
        rt_tick_t now = rt_tick_get();
        rt_base_t level = rt_hw_interrupt_disable();
        if (!g_have_last_wake ||
            (rt_tick_t)(now - g_last_wake) >= rt_tick_from_millisecond(BMI270_WAKE_DEBOUNCE_MS))
        {
            g_have_last_wake = 1;
            g_last_wake = now;
            g_wake_pending = 1;
            sample->wrist_wake_events++;
        }
        rt_hw_interrupt_enable(level);
    }
#endif
    (void)status;
    (void)sample;
}


static BMI2_INTF_RETURN_TYPE bmi_read(uint8_t reg, uint8_t *data,
                                     uint32_t len, void *context)
{
    struct rt_i2c_msg message = {
        .addr = *(uint8_t *)context, .flags = RT_I2C_RD | RT_I2C_MEM_ACCESS,
        .mem_addr = reg, .mem_addr_size = 8, .len = (rt_uint16_t)len, .buf = data,
    };
    if (len > BMI270_TRANSFER_LEN)
    {
        bmi_record_io(1, reg, len, -RT_EINVAL);
        return -1;
    }
    int result = (int)rt_i2c_transfer(g_bus, &message, 1);
    bmi_record_io(1, reg, len, result);
    return result == 1 ? BMI2_INTF_RET_SUCCESS : -1;
}

static BMI2_INTF_RETURN_TYPE bmi_write(uint8_t reg, const uint8_t *data,
                                      uint32_t len, void *context)
{
    struct rt_i2c_msg message = {
        .addr = *(uint8_t *)context, .flags = RT_I2C_WR | RT_I2C_MEM_ACCESS,
        .mem_addr = reg, .mem_addr_size = 8,
        .len = (rt_uint16_t)len, .buf = (uint8_t *)data,
    };
    if (len > BMI270_TRANSFER_LEN)
    {
        bmi_record_io(0, reg, len, -RT_EINVAL);
        return -1;
    }
    int result = (int)rt_i2c_transfer(g_bus, &message, 1);
    bmi_record_io(0, reg, len, result);
    return result == 1 ? BMI2_INTF_RET_SUCCESS : -1;
}

static void bmi_delay(uint32_t us, void *context)
{
    (void)context;
    if (us >= 1000)
        rt_thread_mdelay((us + 999) / 1000);
    else
        rt_hw_us_delay(us);
}

static int8_t bmi_begin(void)
{
    struct bmi2_sens_config config[2] = {{0}};
    uint8_t sensors[] = {
        BMI2_ACCEL,
#if HELIOS_IMU_DEBUG_GYRO
        BMI2_GYRO,
#endif
#if HELIOS_IMU_STEPS
        BMI2_STEP_COUNTER, BMI2_STEP_DETECTOR,
#endif
#if HELIOS_IMU_RAISE_TO_WAKE
        BMI2_WRIST_WEAR_WAKE_UP,
#endif
    };
    struct bmi2_remap axes = {
        .x = HELIOS_BMI270_MAP_X, .y = HELIOS_BMI270_MAP_Y, .z = HELIOS_BMI270_MAP_Z
    };
    uint8_t config_count = 1;
    uint8_t id = 0;
    int8_t result;

    bmi_status_stage("find I2C bus", 0);
    g_bus = helios_i2c_bus(HELIOS_BMI270_I2C_BUS, HELIOS_BMI270_I2C_HZ);
    rt_base_t level = rt_hw_interrupt_disable();
    g_status.bus_ready = g_bus != RT_NULL;
    g_status.address = 0;
    memset(g_status.probe_attempted, 0, sizeof(g_status.probe_attempted));
    memset(g_status.probe_ack, 0, sizeof(g_status.probe_ack));
    memset(g_status.probe_id, 0, sizeof(g_status.probe_id));
    rt_hw_interrupt_enable(level);
    if (!g_bus)
        return BMI2_E_COM_FAIL;

    /* SDO strap is unspecified: accept either documented I2C address. */
    bmi_status_stage("probe chip ID", 0);
    uint8_t found_address = 0;
    for (g_address = 0x68; g_address <= 0x69; g_address++)
    {
        id = 0;
        uint8_t ack = bmi_read(BMI2_CHIP_ID_ADDR, &id, 1, &g_address) == BMI2_INTF_RET_SUCCESS;
        level = rt_hw_interrupt_disable();
        g_status.probe_attempted[g_address - 0x68] = 1;
        g_status.probe_ack[g_address - 0x68] = ack;
        g_status.probe_id[g_address - 0x68] = id;
        rt_hw_interrupt_enable(level);
        if (ack && id == BMI270_CHIP_ID)
        {
            found_address = g_address;
            break; /* Avoid resetting a healthy bus by probing an absent peer. */
        }
    }
    if (!found_address)
        return BMI2_E_DEV_NOT_FOUND;
    g_address = found_address;
    level = rt_hw_interrupt_disable();
    g_status.address = found_address;
    rt_hw_interrupt_enable(level);

    memset(&g_device, 0, sizeof(g_device));
    g_device.intf = BMI2_I2C_INTF;
    g_device.intf_ptr = &g_address;
    g_device.read = bmi_read;
    g_device.write = bmi_write;
    g_device.delay_us = bmi_delay;
    g_device.read_write_len = BMI270_TRANSFER_LEN;
    /* Bosch initialization uploads the required configuration image. */
    bmi_status_stage("upload config", 0);
    result = bmi270_init(&g_device);
    if (result != BMI2_OK)
        return result;

    bmi_status_stage("axis mapping", 0);
    result = bmi2_set_remap_axes(&axes, &g_device);
    if (result != BMI2_OK)
        return result;

    config[0].type = BMI2_ACCEL;
#if HELIOS_IMU_DEBUG_GYRO
    config[1].type = BMI2_GYRO;
    config_count++;
#endif
    bmi_status_stage("read sensor config", 0);
    result = bmi270_get_sensor_config(config, config_count, &g_device);
    if (result != BMI2_OK)
        return result;
    config[0].cfg.acc.odr = BMI2_ACC_ODR_100HZ;
    config[0].cfg.acc.range = BMI2_ACC_RANGE_2G;
    config[0].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;
    config[0].cfg.acc.filter_perf = BMI2_POWER_OPT_MODE;
#if HELIOS_IMU_DEBUG_GYRO
    config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
    config[1].cfg.gyr.range = BMI2_GYR_RANGE_2000;
    config[1].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;
    config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
#endif
    bmi_status_stage("set sensor config", 0);
    result = bmi270_set_sensor_config(config, config_count, &g_device);
    if (result != BMI2_OK)
        return result;
    bmi_status_stage("enable sensors", 0);
    result = bmi270_sensor_enable(sensors, sizeof(sensors), &g_device);
    if (result != BMI2_OK)
        return result;

#if HELIOS_IMU_RAISE_TO_WAKE || HELIOS_IMU_STEPS
    struct bmi2_int_pin_config interrupt = {0};
    struct bmi2_sens_int_config mappings[] = {
#if HELIOS_IMU_RAISE_TO_WAKE
        {.type = BMI2_WRIST_WEAR_WAKE_UP, .hw_int_pin = BMI2_INT1},
#endif
#if HELIOS_IMU_STEPS
        {.type = BMI2_STEP_DETECTOR, .hw_int_pin = BMI2_INT1},
#endif
    };
    interrupt.pin_type = BMI2_INT1;
    interrupt.int_latch = BMI2_INT_LATCH;
    interrupt.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
    interrupt.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
    interrupt.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
    interrupt.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;
    bmi_status_stage("configure INT1", 0);
    result = bmi2_set_int_pin_config(&interrupt, &g_device);
    if (result != BMI2_OK)
        return result;
    bmi_status_stage("map interrupts", 0);
    result = bmi270_map_feat_int(mappings, sizeof(mappings) / sizeof(mappings[0]), &g_device);
#endif
    return result;
}

static int8_t bmi_update_gyro(void)
{
    uint8_t sensor = BMI2_GYRO;
    rt_base_t level = rt_hw_interrupt_disable();
    uint8_t wanted = HELIOS_IMU_DEBUG_GYRO || g_diagnostics_requested;
    rt_hw_interrupt_enable(level);
    if (wanted == g_gyro_enabled)
        return BMI2_OK;

    int8_t result;
    if (wanted)
    {
        struct bmi2_sens_config config = {.type = BMI2_GYRO};
        result = bmi270_get_sensor_config(&config, 1, &g_device);
        if (result != BMI2_OK)
            return result;
        config.cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
        config.cfg.gyr.range = BMI2_GYR_RANGE_2000;
        config.cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;
        config.cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
        result = bmi270_set_sensor_config(&config, 1, &g_device);
        if (result == BMI2_OK)
            result = bmi270_sensor_enable(&sensor, 1, &g_device);
    }
    else
    {
        result = bmi270_sensor_disable(&sensor, 1, &g_device);
    }
    if (result == BMI2_OK)
        g_gyro_enabled = wanted;
    return result;
}

void helios_bmi270_set_diagnostics(uint8_t enabled)
{
    rt_base_t level = rt_hw_interrupt_disable();
    uint8_t changed = g_diagnostics_requested != !!enabled;
    g_diagnostics_requested = !!enabled;
    rt_hw_interrupt_enable(level);
    if (changed && g_thread)
        rt_sem_release(&g_event_sem);
}

static int8_t bmi_initialize_once(void)
{
    rt_base_t level = rt_hw_interrupt_disable();
    g_valid = 0;
    g_status.attempts++;
    rt_hw_interrupt_enable(level);
    int8_t result = bmi_begin();
    level = rt_hw_interrupt_disable();
    g_status.error = result;
    if (result == BMI2_OK)
        g_status.stage = "first sample";
    rt_hw_interrupt_enable(level);
    return result;
}

static void bmi_worker(void *parameter)
{
    struct bmi2_sens_data data = {0};
#if HELIOS_IMU_STEPS
    struct bmi2_feat_sensor_data feature = {.type = BMI2_STEP_COUNTER};
#endif
    helios_bmi270_sample_t sample = {0};
    uint16_t status;
    int8_t result;
    (void)parameter;

    while ((result = bmi_initialize_once()) != BMI2_OK)
    {
        helios_bmi270_status_t status;
        helios_bmi270_get_status(&status);
        LOG_E("initialization failed at %s: %d; %s probe 68=%02x/%d 69=%02x/%d; retrying",
              status.stage, result, HELIOS_BMI270_I2C_BUS,
              status.probe_id[0], status.probe_ack[0], status.probe_id[1], status.probe_ack[1]);
        rt_sem_take(&g_event_sem, rt_tick_from_millisecond(5000));
    }
    g_gyro_enabled = HELIOS_IMU_DEBUG_GYRO;
#if HELIOS_IMU_RAISE_TO_WAKE || HELIOS_IMU_STEPS
    if (bmi_irq_start() != RT_EOK)
        LOG_W("INT1 unavailable; using status polling");
#endif
    LOG_I("ready on %s address 0x%02x; steps=%d wrist-wake=%d gyro=%d",
          HELIOS_BMI270_I2C_BUS, g_address, HELIOS_IMU_STEPS,
          HELIOS_IMU_RAISE_TO_WAKE, HELIOS_IMU_DEBUG_GYRO);
    while (1)
    {
        int8_t gyro_result = bmi_update_gyro();
        status = 0;
        if (bmi2_get_int_status(&status, &g_device) == BMI2_OK)
            bmi_handle_events(status, &sample);
        const char *read_stage = "read accel/gyro";
        result = bmi2_get_sensor_data(&data, &g_device);
#if HELIOS_IMU_STEPS
        if (result == BMI2_OK)
        {
            read_stage = "read steps";
            result = bmi270_get_feature_data(&feature, 1, &g_device);
        }
#endif
        if (result == BMI2_OK)
        {
            sample.accel[0] = data.acc.x;
            sample.accel[1] = data.acc.y;
            sample.accel[2] = data.acc.z;
            sample.gyro_enabled = g_gyro_enabled && gyro_result == BMI2_OK;
            sample.gyro_error = gyro_result;
            sample.gyro[0] = sample.gyro_enabled ? data.gyr.x : 0;
            sample.gyro[1] = sample.gyro_enabled ? data.gyr.y : 0;
            sample.gyro[2] = sample.gyro_enabled ? data.gyr.z : 0;
#if HELIOS_IMU_STEPS
            sample.steps = feature.sens_data.step_counter_output;
#endif
        }
        rt_base_t level = rt_hw_interrupt_disable();
        if (result == BMI2_OK)
            g_sample = sample;
        g_valid = (result == BMI2_OK);
        g_status.stage = result == BMI2_OK ? "ready" : read_stage;
        g_status.error = result;
        rt_hw_interrupt_enable(level);
        /* INT1 wakes this thread immediately; timeout also refreshes the snapshot
         * and recovers events if an IRQ edge is missed. The sensor counts steps
         * autonomously, so the host need not sample every accelerometer frame. */
        rt_sem_take(&g_event_sem, rt_tick_from_millisecond(g_gyro_enabled ? 100 : 1000));
    }
}

rt_err_t helios_bmi270_start(void)
{
    if (g_thread)
        return RT_EOK;
    bmi_status_stage("start worker", 0);
    rt_err_t result = rt_sem_init(&g_event_sem, "bmi_evt", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        bmi_status_stage("init event semaphore", result);
        return result;
    }
    g_thread = rt_thread_create("bmi270", bmi_worker, RT_NULL, 4096, 24, 10);
    if (!g_thread)
    {
        rt_sem_detach(&g_event_sem);
        bmi_status_stage("create worker", -RT_ENOMEM);
        return -RT_ENOMEM;
    }
    result = rt_thread_startup(g_thread);
    if (result != RT_EOK)
    {
        rt_thread_delete(g_thread);
        g_thread = RT_NULL;
        rt_sem_detach(&g_event_sem);
        bmi_status_stage("start worker", result);
    }
    return result;
}

rt_err_t helios_bmi270_read(helios_bmi270_sample_t *sample)
{
    uint8_t valid;
    int error;
    if (!sample)
        return -RT_EINVAL;
    rt_base_t level = rt_hw_interrupt_disable();
    valid = g_valid;
    error = g_status.error;
    if (valid)
        *sample = g_sample;
    rt_hw_interrupt_enable(level);
    return valid ? RT_EOK : (error ? error : -RT_ERROR);
}

uint8_t helios_bmi270_take_wake(void)
{
#if HELIOS_IMU_RAISE_TO_WAKE
    uint8_t pending;
    rt_base_t level = rt_hw_interrupt_disable();
    pending = g_wake_pending &&
        (rt_tick_t)(rt_tick_get() - g_last_wake) <= rt_tick_from_millisecond(BMI270_WAKE_EXPIRY_MS);
    g_wake_pending = 0;
    rt_hw_interrupt_enable(level);
    return pending;
#else
    return 0;
#endif
}

#if HELIOS_IMU_DEBUG_ORIENTATION
rt_err_t helios_bmi270_orientation(const helios_bmi270_sample_t *sample,
                                  int16_t *roll, int16_t *pitch)
{
    if (!sample || !roll || !pitch)
        return -RT_EINVAL;
    float x = sample->accel[0];
    float y = sample->accel[1];
    float z = sample->accel[2];
    if (x * x + y * y + z * z < 1.0f)
        return -RT_ERROR; /* No gravity vector, e.g. free fall or no sample. */
    *roll = (int16_t)lroundf(atan2f(y, z) * (1800.0f / 3.14159265358979323846f));
    *pitch = (int16_t)lroundf(atan2f(-x, sqrtf(y * y + z * z)) * (1800.0f / 3.14159265358979323846f));
    return RT_EOK;
}
#endif

#ifdef RT_USING_FINSH
static void bmi270_status(void)
{
    helios_bmi270_sample_t sample;
    rt_err_t err = helios_bmi270_read(&sample);
    if (err != RT_EOK)
    {
        helios_bmi270_status_t status;
        helios_bmi270_get_status(&status);
        rt_kprintf("BMI270: stage=%s error=%d attempts=%lu bus=%s/%luHz ready=%u\n",
                   status.stage, status.error, (unsigned long)status.attempts,
                   HELIOS_BMI270_I2C_BUS, (unsigned long)status.bus_hz, status.bus_ready);
        rt_kprintf("Probe 0x68: tried=%u ack=%u id=0x%02x; 0x69: tried=%u ack=%u id=0x%02x (expected id=0x24)\n",
                   status.probe_attempted[0], status.probe_ack[0], status.probe_id[0],
                   status.probe_attempted[1], status.probe_ack[1], status.probe_id[1]);
        rt_kprintf("Last I2C %s reg=0x%02x len=%lu result=%d (success=1)\n",
                   status.io_read ? "read" : "write", status.io_register,
                   (unsigned long)status.io_length, status.io_result);
        return;
    }
    rt_kprintf("accel=%d,%d,%d gyro=%d,%d,%d steps=%lu step_events=%lu wrist_wakes=%lu\n",
               sample.accel[0], sample.accel[1], sample.accel[2],
               sample.gyro[0], sample.gyro[1], sample.gyro[2],
               (unsigned long)sample.steps, (unsigned long)sample.step_events,
               (unsigned long)sample.wrist_wake_events);
#if HELIOS_IMU_DEBUG_ORIENTATION
    int16_t roll, pitch;
    if (helios_bmi270_orientation(&sample, &roll, &pitch) == RT_EOK)
        rt_kprintf("tilt roll=%d pitch=%d (tenths of a degree; no heading)\n", roll, pitch);
#endif
}
MSH_CMD_EXPORT(bmi270_status, Show BMI270 raw axes and steps);
#endif
#endif
