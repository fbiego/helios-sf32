#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Include the real adapter so tests exercise its private event and init paths. */
#include "core/helios_bmi270.c"

static rt_tick_t now;
static struct rt_i2c_bus_device bus;
static uint8_t present_address = 0x69;
static uint64_t enabled_sensors, mapped_features;
static int8_t init_result;
static int transfers, releases, detaches, deletes, create_fails, startup_result;
static int irq_attach_result, irq_enable_result;
static int8_t gyro_config_error, gyro_disable_error;
static uint8_t bus_present = 1, chip_id = BMI270_CHIP_ID;
static uint8_t last_payload[32];

rt_tick_t rt_tick_get(void) { return now; }
rt_tick_t rt_tick_from_millisecond(int32_t ms) { return (rt_tick_t)ms; }
rt_base_t rt_hw_interrupt_disable(void) { return 0; }
void rt_hw_interrupt_enable(rt_base_t value) { (void)value; }
void rt_hw_us_delay(uint32_t us) { (void)us; }
void rt_thread_mdelay(int32_t ms) { (void)ms; }
rt_err_t rt_sem_init(struct rt_semaphore *s, const char *n, uint32_t v, uint8_t f)
{ (void)s; (void)n; (void)v; (void)f; return RT_EOK; }
rt_err_t rt_sem_take(struct rt_semaphore *s, int32_t ticks)
{ (void)s; (void)ticks; return RT_EOK; }
rt_err_t rt_sem_detach(struct rt_semaphore *s) { (void)s; detaches++; return RT_EOK; }
rt_err_t rt_sem_release(struct rt_semaphore *s) { (void)s; releases++; return RT_EOK; }
rt_thread_t rt_thread_create(const char *n, void (*entry)(void *), void *p,
                            uint32_t stack, uint8_t priority, uint32_t ticks)
{
    (void)n; (void)entry; (void)p; (void)stack; (void)priority; (void)ticks;
    return create_fails ? RT_NULL : (rt_thread_t)&bus;
}
rt_err_t rt_thread_startup(rt_thread_t thread) { (void)thread; return startup_result; }
rt_err_t rt_thread_delete(rt_thread_t thread) { (void)thread; deletes++; return RT_EOK; }

struct rt_i2c_bus_device *helios_i2c_bus(const char *name, uint32_t hz)
{ assert(strcmp(name, "i2c2") == 0); assert(hz == HELIOS_BMI270_I2C_HZ); return bus_present ? &bus : NULL; }
rt_size_t rt_i2c_transfer(struct rt_i2c_bus_device *device, struct rt_i2c_msg *msg, uint32_t n)
{
    assert(device == &bus);
    transfers++;
    if (msg[0].addr != present_address) return 0;
    assert(n == 1 && msg[0].mem_addr_size == 8 && (msg[0].flags & RT_I2C_MEM_ACCESS));
    if (msg[0].flags & RT_I2C_RD) {
        memset(msg[0].buf, 0, msg[0].len);
        if (msg[0].mem_addr == BMI2_CHIP_ID_ADDR)
            msg[0].buf[0] = chip_id;
    } else {
        assert(msg[0].len <= sizeof(last_payload));
        memcpy(last_payload, msg[0].buf, msg[0].len);
    }
    return n;
}
void HAL_PIN_Set(int pad, int gpio, int pull, int cpu)
{ assert(pad == 44 && gpio == 44 && pull == PIN_PULLDOWN && cpu == 1); }
void rt_pin_mode(rt_base_t pin, rt_base_t mode)
{ assert(pin == 44 && mode == PIN_MODE_INPUT); }
rt_err_t rt_pin_attach_irq(rt_int32_t pin, rt_uint32_t mode, void (*cb)(void *), void *context)
{ assert(pin == 44 && mode == PIN_IRQ_MODE_RISING && cb != NULL && context == NULL); return irq_attach_result; }
rt_err_t rt_pin_irq_enable(rt_base_t pin, rt_uint32_t enable)
{ assert(pin == 44 && enable == PIN_IRQ_ENABLE); return irq_enable_result; }
rt_err_t rt_pin_detach_irq(rt_int32_t pin) { assert(pin == 44); return RT_EOK; }

int8_t bmi270_init(struct bmi2_dev *device)
{ assert(device->intf == BMI2_I2C_INTF && device->read_write_len == 32); return init_result; }
int8_t bmi2_set_remap_axes(const struct bmi2_remap *axes, struct bmi2_dev *device)
{
    (void)device;
    assert(axes->x == BMI2_Y && axes->y == BMI2_NEG_X && axes->z == BMI2_Z);
    return BMI2_OK;
}
int8_t bmi270_get_sensor_config(struct bmi2_sens_config *config, uint8_t n, struct bmi2_dev *device)
{
    (void)device;
    if (config[0].type == BMI2_ACCEL)
        assert(n == 1 + HELIOS_IMU_DEBUG_GYRO);
    else
        assert(config[0].type == BMI2_GYRO && n == 1);
    return BMI2_OK;
}
int8_t bmi270_set_sensor_config(struct bmi2_sens_config *config, uint8_t n, struct bmi2_dev *device)
{
    (void)n; (void)device;
    if (config[0].type == BMI2_ACCEL) {
        assert(config[0].cfg.acc.odr == BMI2_ACC_ODR_100HZ);
        assert(config[0].cfg.acc.range == BMI2_ACC_RANGE_2G);
        assert(config[0].cfg.acc.filter_perf == BMI2_POWER_OPT_MODE);
    } else {
        assert(config[0].type == BMI2_GYRO);
        assert(config[0].cfg.gyr.range == BMI2_GYR_RANGE_2000);
        return gyro_config_error;
    }
    return BMI2_OK;
}
int8_t bmi270_sensor_enable(const uint8_t *sensors, uint8_t n, struct bmi2_dev *device)
{ (void)device; for (uint8_t i = 0; i < n; i++) enabled_sensors |= UINT64_C(1) << sensors[i]; return BMI2_OK; }
int8_t bmi270_sensor_disable(const uint8_t *sensors, uint8_t n, struct bmi2_dev *device)
{
    (void)device;
    assert(n == 1 && sensors[0] == BMI2_GYRO);
    if (!gyro_disable_error)
        enabled_sensors &= ~(UINT64_C(1) << BMI2_GYRO);
    return gyro_disable_error;
}
int8_t bmi2_set_int_pin_config(const struct bmi2_int_pin_config *config, struct bmi2_dev *device)
{
    (void)device;
    assert(config->pin_type == BMI2_INT1 && config->int_latch == BMI2_INT_LATCH);
    assert(config->pin_cfg[0].lvl == BMI2_INT_ACTIVE_HIGH && config->pin_cfg[0].output_en);
    return BMI2_OK;
}
int8_t bmi270_map_feat_int(const struct bmi2_sens_int_config *config, uint8_t n, struct bmi2_dev *device)
{
    (void)device;
    for (uint8_t i = 0; i < n; i++) {
        assert(config[i].hw_int_pin == BMI2_INT1);
        mapped_features |= UINT64_C(1) << config[i].type;
    }
    return BMI2_OK;
}
int8_t bmi2_get_int_status(uint16_t *status, struct bmi2_dev *device)
{ (void)device; *status = 0; return BMI2_OK; }
int8_t bmi2_get_sensor_data(struct bmi2_sens_data *data, struct bmi2_dev *device)
{ (void)device; memset(data, 0, sizeof(*data)); return BMI2_OK; }
int8_t bmi270_get_feature_data(struct bmi2_feat_sensor_data *data, uint8_t n, struct bmi2_dev *device)
{ (void)device; (void)n; data->sens_data.step_counter_output = 123; return BMI2_OK; }

static void test_initialization(void)
{
    assert(bmi_begin() == BMI2_OK && g_address == 0x69);
    assert(enabled_sensors & (UINT64_C(1) << BMI2_ACCEL));
    assert(!!(enabled_sensors & (UINT64_C(1) << BMI2_GYRO)) == HELIOS_IMU_DEBUG_GYRO);
    assert(!!(enabled_sensors & (UINT64_C(1) << BMI2_STEP_COUNTER)) == HELIOS_IMU_STEPS);
    assert(!!(enabled_sensors & (UINT64_C(1) << BMI2_STEP_DETECTOR)) == HELIOS_IMU_STEPS);
    assert(!!(mapped_features & (UINT64_C(1) << BMI2_STEP_DETECTOR)) == HELIOS_IMU_STEPS);
    assert(!!(mapped_features & (UINT64_C(1) << BMI2_WRIST_WEAR_WAKE_UP)) == HELIOS_IMU_RAISE_TO_WAKE);
    present_address = 0;
    assert(bmi_begin() == BMI2_E_DEV_NOT_FOUND);
    present_address = 0x68;
    init_result = BMI2_E_COM_FAIL;
    assert(bmi_begin() == BMI2_E_COM_FAIL);
    init_result = BMI2_OK;
#if HELIOS_IMU_RAISE_TO_WAKE || HELIOS_IMU_STEPS
    assert(bmi_irq_start() == RT_EOK);
    int previous_transfers = transfers;
    bmi_irq(NULL);
    assert(releases == 1 && transfers == previous_transfers);
    irq_attach_result = -RT_ERROR;
    assert(bmi_irq_start() == -RT_ERROR);
    irq_attach_result = 0;
    irq_enable_result = -RT_ERROR;
    assert(bmi_irq_start() == -RT_ERROR);
#endif
    create_fails = 1;
    assert(helios_bmi270_start() == -RT_ENOMEM && detaches == 1);
    create_fails = 0;
    startup_result = -RT_ERROR;
    assert(helios_bmi270_start() == -RT_ERROR && g_thread == NULL && deletes == 1 && detaches == 2);
}

static void test_diagnostic_gyro(void)
{
    g_gyro_enabled = HELIOS_IMU_DEBUG_GYRO;
    g_thread = (rt_thread_t)&bus;
    int old_releases = releases, old_transfers = transfers;
    helios_bmi270_set_diagnostics(1);
    helios_bmi270_set_diagnostics(1);
    assert(releases == old_releases + 1 && transfers == old_transfers);
    assert(bmi_update_gyro() == BMI2_OK && g_gyro_enabled);
    helios_bmi270_set_diagnostics(0);
#if !HELIOS_IMU_DEBUG_GYRO
    gyro_disable_error = BMI2_E_COM_FAIL;
    assert(bmi_update_gyro() == BMI2_E_COM_FAIL && g_gyro_enabled);
    gyro_disable_error = 0;
#endif
    assert(bmi_update_gyro() == BMI2_OK && g_gyro_enabled == HELIOS_IMU_DEBUG_GYRO);
#if !HELIOS_IMU_DEBUG_GYRO
    helios_bmi270_set_diagnostics(1);
    gyro_config_error = BMI2_E_COM_FAIL;
    assert(bmi_update_gyro() == BMI2_E_COM_FAIL && !g_gyro_enabled);
    gyro_config_error = 0;
    assert(bmi_update_gyro() == BMI2_OK && g_gyro_enabled);
    helios_bmi270_set_diagnostics(0);
    assert(bmi_update_gyro() == BMI2_OK && !g_gyro_enabled);
#endif
    g_thread = NULL;
}

static void test_failure_diagnostics(void)
{
    helios_bmi270_status_t status;
    helios_bmi270_sample_t sample;
    g_thread = NULL;
    bus_present = 0;
    assert(bmi_initialize_once() == BMI2_E_COM_FAIL);
    helios_bmi270_get_status(&status);
    assert(!status.bus_ready && strcmp(status.stage, "find I2C bus") == 0);
    assert(helios_bmi270_read(&sample) == BMI2_E_COM_FAIL);
    uint32_t attempts = status.attempts;

    bus_present = 1; present_address = 0;
    assert(bmi_initialize_once() == BMI2_E_DEV_NOT_FOUND);
    helios_bmi270_get_status(&status);
    assert(status.attempts == attempts + 1 && status.bus_ready && !status.address);
    assert(!status.probe_ack[0] && !status.probe_ack[1]);
    assert(strcmp(status.stage, "probe chip ID") == 0);
    assert(status.io_result == 0 && status.io_register == BMI2_CHIP_ID_ADDR);

    present_address = 0x68; chip_id = 0x42;
    assert(bmi_initialize_once() == BMI2_E_DEV_NOT_FOUND);
    helios_bmi270_get_status(&status);
    assert(status.probe_ack[0] && status.probe_id[0] == 0x42);
    chip_id = BMI270_CHIP_ID; init_result = BMI2_E_CONFIG_LOAD;
    assert(bmi_initialize_once() == BMI2_E_CONFIG_LOAD);
    helios_bmi270_get_status(&status);
    assert(status.address == 0x68 && strcmp(status.stage, "upload config") == 0);
    assert(helios_bmi270_read(&sample) == BMI2_E_CONFIG_LOAD);

    init_result = BMI2_OK;
    assert(bmi_initialize_once() == BMI2_OK); /* Retry can recover. */
    helios_bmi270_get_status(&status);
    assert(status.error == 0 && strcmp(status.stage, "first sample") == 0);
    assert(status.probe_ack[0] && status.probe_id[0] == BMI270_CHIP_ID);
    assert(status.probe_attempted[0] && !status.probe_attempted[1]);

    uint8_t payload[32];
    memset(payload, 0xa5, sizeof(payload));
    assert(bmi_write(0x5e, payload, sizeof(payload), &g_address) == BMI2_INTF_RET_SUCCESS);
    assert(memcmp(payload, last_payload, sizeof(payload)) == 0);
    helios_bmi270_get_status(&status);
    assert(!status.io_read && status.io_register == 0x5e && status.io_length == 32 && status.io_result == 1);
    assert(bmi_write(0x5e, payload, 33, &g_address) != BMI2_INTF_RET_SUCCESS);
    helios_bmi270_get_status(&status);
    assert(status.io_result == -RT_EINVAL);
}

static void test_events(void)
{
    helios_bmi270_sample_t sample = {0};
    now = 0;
    bmi_handle_events(BMI270_STEP_CNT_STATUS_MASK | BMI270_WRIST_WAKE_UP_STATUS_MASK, &sample);
    assert(sample.step_events == HELIOS_IMU_STEPS);
    assert(helios_bmi270_take_wake() == HELIOS_IMU_RAISE_TO_WAKE);
    assert(helios_bmi270_take_wake() == 0); /* Consumed once. */
    now = 100;
    bmi_handle_events(BMI270_WRIST_WAKE_UP_STATUS_MASK, &sample);
    assert(helios_bmi270_take_wake() == 0); /* Debounce repeated gestures. */
    now = 1500;
    bmi_handle_events(BMI270_WRIST_WAKE_UP_STATUS_MASK, &sample);
    assert(sample.wrist_wake_events == 2 * HELIOS_IMU_RAISE_TO_WAKE);
    now = 3501;
    assert(helios_bmi270_take_wake() == 0); /* Drop old events after a UI stall. */
#if HELIOS_IMU_RAISE_TO_WAKE
    g_have_last_wake = 0;
    now = UINT32_MAX - 500;
    bmi_handle_events(BMI270_WRIST_WAKE_UP_STATUS_MASK, &sample);
    assert(helios_bmi270_take_wake() == 1);
    now = 1000;
    bmi_handle_events(BMI270_WRIST_WAKE_UP_STATUS_MASK, &sample);
    assert(helios_bmi270_take_wake() == 1); /* Tick wraparound. */
#endif
}

#if HELIOS_IMU_DEBUG_ORIENTATION
static void test_orientation(void)
{
    const struct { int16_t x, y, z, roll, pitch; } cases[] = {
        {0, 0, 16384, 0, 0}, {0, 16384, 0, 900, 0},
        {0, -16384, 0, -900, 0}, {16384, 0, 0, 0, -900},
        {-16384, 0, 0, 0, 900}, {0, 11585, 11585, 450, 0},
    };
    helios_bmi270_sample_t sample = {0};
    int16_t roll, pitch;
    assert(helios_bmi270_orientation(NULL, &roll, &pitch) == -RT_EINVAL);
    assert(helios_bmi270_orientation(&sample, &roll, &pitch) == -RT_ERROR);
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        sample.accel[0] = cases[i].x; sample.accel[1] = cases[i].y; sample.accel[2] = cases[i].z;
        assert(helios_bmi270_orientation(&sample, &roll, &pitch) == RT_EOK);
        assert(roll == cases[i].roll && pitch == cases[i].pitch);
    }
    /* Gravity vectors reconstructed from the board's measured flat/upright
     * tilt reports, with the configured sensor Y,-X,Z mapping applied. */
    sample.accel[0] = -315; sample.accel[1] = 0; sample.accel[2] = 16381;
    assert(helios_bmi270_orientation(&sample, &roll, &pitch) == RT_EOK);
    assert(roll == 0 && pitch >= 10 && pitch <= 12);
    sample.accel[0] = -750; sample.accel[1] = 16324; sample.accel[2] = 1181;
    assert(helios_bmi270_orientation(&sample, &roll, &pitch) == RT_EOK);
    assert(roll >= 858 && roll <= 860 && pitch >= 25 && pitch <= 27);
}
#endif

int main(void)
{
    test_initialization();
    test_diagnostic_gyro();
    test_failure_diagnostics();
    test_events();
#if HELIOS_IMU_DEBUG_ORIENTATION
    test_orientation();
#endif
    return 0;
}
