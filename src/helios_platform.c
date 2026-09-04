#include "helios_platform.h"

#include <string.h>

#include "board.h"

#ifdef RT_USING_DFS
#include "dfs_fs.h"
#endif

#ifdef RT_USING_I2C
#include "drivers/i2c.h"
#endif

#ifdef RT_USING_SPI
#include "drivers/spi.h"
#endif

#ifdef USING_BUTTON_LIB
#include "button.h"
#endif

#define LOG_TAG "helios.hw"
#include "log.h"

#define HELIOS_ADC_DEV_NAME "bat1"
#define HELIOS_ADC_VBAT_CH 7
#define HELIOS_I2C2_SCL_PAD PAD_PA40
#define HELIOS_I2C2_SDA_PAD PAD_PA39

static rt_device_t g_adc_dev;

static void helios_i2c2_pinmux_init(void)
{
#if defined(RT_USING_I2C) && defined(BSP_USING_I2C2)
#if defined(BSP_LCDC_USING_DBI)
    LOG_W("i2c2 pins not configured: PA39/PA40 are used by DBI LCD");
#else
    HAL_PIN_Set(HELIOS_I2C2_SCL_PAD, I2C2_SCL, PIN_PULLUP, 1);
    HAL_PIN_Set(HELIOS_I2C2_SDA_PAD, I2C2_SDA, PIN_PULLUP, 1);
    LOG_I("i2c2 pinmux ready: scl=PA40 sda=PA39");
#endif
#endif
}

#ifdef USING_BUTTON_LIB
static volatile uint8_t g_button_state[2];
static volatile uint32_t g_button_press_count[2];

static void helios_button_handler(int32_t pin, button_action_t action)
{
    uint8_t pressed = (action == BUTTON_PRESSED || action == BUTTON_LONG_PRESSED);
    int8_t id = -1;

#ifdef BSP_KEY1_PIN
    if (pin == BSP_KEY1_PIN)
        id = 0;
#endif
#ifdef BSP_KEY2_PIN
    if (pin == BSP_KEY2_PIN)
        id = 1;
#endif

    if (id >= 0)
    {
        g_button_state[id] = pressed;
        if (action == BUTTON_PRESSED)
            g_button_press_count[id]++;
    }

    LOG_I("button pin=%d action=%d", pin, action);
}

static void helios_button_init_one(int32_t pin, uint8_t active_high)
{
    button_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.pin = pin;
    cfg.active_state = active_high ? BUTTON_ACTIVE_HIGH : BUTTON_ACTIVE_LOW;
    cfg.mode = PIN_MODE_INPUT;
    cfg.button_handler = helios_button_handler;

    int32_t id = button_init(&cfg);
    if (id >= 0)
        button_enable(id);
}
#endif

void helios_platform_init(void)
{
    helios_i2c2_pinmux_init();

#ifdef USING_BUTTON_LIB
#ifdef BSP_KEY1_PIN
#ifdef BSP_KEY1_ACTIVE_HIGH
    helios_button_init_one(BSP_KEY1_PIN, 1);
#else
    helios_button_init_one(BSP_KEY1_PIN, 0);
#endif
#endif
#ifdef BSP_KEY2_PIN
#ifdef BSP_KEY2_ACTIVE_HIGH
    helios_button_init_one(BSP_KEY2_PIN, 1);
#else
    helios_button_init_one(BSP_KEY2_PIN, 0);
#endif
#endif
#endif

#ifdef RT_USING_ADC
    g_adc_dev = rt_device_find(HELIOS_ADC_DEV_NAME);
    if (g_adc_dev)
        LOG_I("battery adc ready: %s channel %d", HELIOS_ADC_DEV_NAME, HELIOS_ADC_VBAT_CH);
    else
        LOG_W("battery adc device %s not found", HELIOS_ADC_DEV_NAME);
#endif
}

rt_err_t helios_battery_read(helios_battery_t *battery)
{
    if (!battery)
        return -RT_EINVAL;

    memset(battery, 0, sizeof(*battery));

#ifdef RT_USING_ADC
    if (!g_adc_dev)
        g_adc_dev = rt_device_find(HELIOS_ADC_DEV_NAME);
    if (!g_adc_dev)
        return -RT_ERROR;

    rt_err_t err = rt_adc_enable((rt_adc_device_t)g_adc_dev, HELIOS_ADC_VBAT_CH);
    if (err != RT_EOK)
        return err;

    rt_uint32_t raw_01mv = rt_adc_read((rt_adc_device_t)g_adc_dev, HELIOS_ADC_VBAT_CH);
    rt_adc_disable((rt_adc_device_t)g_adc_dev, HELIOS_ADC_VBAT_CH);

    battery->millivolts = raw_01mv / 10;

    if (battery->millivolts >= 4200)
        battery->percent = 100;
    else if (battery->millivolts <= 3300)
        battery->percent = 0;
    else
        battery->percent = (uint8_t)((battery->millivolts - 3300) * 100 / (4200 - 3300));

    return RT_EOK;
#else
    return -RT_ENOSYS;
#endif
}

rt_device_t helios_storage_open_root(void)
{
#ifdef RT_USING_DFS
    return rt_device_find("flash2");
#else
    return RT_NULL;
#endif
}

struct rt_i2c_bus_device *helios_i2c_bus(const char *name, uint32_t hz)
{
#ifdef RT_USING_I2C
    struct rt_i2c_bus_device *bus = rt_i2c_bus_device_find(name ? name : "i2c2");
    if (!bus)
        return RT_NULL;

    struct rt_i2c_configuration cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.timeout = 500;
    cfg.max_hz = hz ? hz : 400000;
    rt_i2c_configure(bus, &cfg);
    return bus;
#else
    (void)name;
    (void)hz;
    return RT_NULL;
#endif
}

struct rt_spi_device *helios_spi_device(const char *name)
{
#ifdef RT_USING_SPI
    return (struct rt_spi_device *)rt_device_find(name ? name : "spi10");
#else
    (void)name;
    return RT_NULL;
#endif
}

uint8_t helios_button_pressed(uint8_t id)
{
#ifdef USING_BUTTON_LIB
    if (id < sizeof(g_button_state))
        return g_button_state[id];
#else
    (void)id;
#endif
    return 0;
}

uint8_t helios_button_take_press(uint8_t id)
{
#ifdef USING_BUTTON_LIB
    uint8_t pressed = 0;
    if (id < sizeof(g_button_press_count) / sizeof(g_button_press_count[0]))
    {
        rt_base_t level = rt_hw_interrupt_disable();
        if (g_button_press_count[id])
        {
            g_button_press_count[id]--;
            pressed = 1;
        }
        rt_hw_interrupt_enable(level);
    }
    return pressed;
#else
    (void)id;
    return 0;
#endif
}
