#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LORA_TX);

#define MAX_DATA_LEN 10

#define LORA_NODE DT_ALIAS(lora0)
#define LED0_NODE GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios)

char data[MAX_DATA_LEN] = { 'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd' };

/**
 * @brief
 * @param led
 */
static void _led_init(const struct gpio_dt_spec& led, uint32_t gpio_mode)
{
    /* check if GPIO is ready */
    if (!gpio_is_ready_dt(&led)) [[unlikely]] {
        LOG_ERR("LED GPIO is not ready!");
        return;
    }

    /* configure as output */
    if (gpio_pin_configure_dt(&led, gpio_mode) < 0) [[unlikely]] {
        LOG_ERR("Cannot configure LED GPIO!");
        return;
    }
}

/**
 * @brief
 * @param led
 */
static void _led_toggle(const struct gpio_dt_spec& led)
{
    if (!gpio_is_ready_dt(&led)) [[unlikely]] {
        LOG_ERR("LED GPIO is not ready!");
        return;
    }

    if (gpio_pin_toggle_dt(&led) < 0) [[unlikely]] {
        LOG_ERR("Cannot toggle LED GPIO!");
        return;
    }
}

/**
 * @brief
 * @param led
 * @param period
 */
static void _led_blink(const struct gpio_dt_spec& led, const k_timeout_t& period)
{
    _led_toggle(led);
    k_sleep(period);
    _led_toggle(led);
    k_sleep(period);
}

int main()
{
    _led_init(LED0_NODE, GPIO_OUTPUT_INACTIVE);

    const struct device* lora_dev = DEVICE_DT_GET(LORA_NODE);
    struct lora_modem_config config;
    int ret;

    if (!device_is_ready(lora_dev)) {
        LOG_ERR("%s Device not ready", lora_dev->name);
        return 0;
    }

    config.frequency = 915000000;
    config.bandwidth = BW_125_KHZ;
    config.datarate = SF_7;
    config.preamble_len = 8;
    config.coding_rate = CR_4_5;
    config.iq_inverted = false;
    config.public_network = false;
    config.tx_power = 4;
    config.tx = true;

    ret = lora_config(lora_dev, &config);
    if (ret < 0) {
        LOG_ERR("LoRa config failed");
        return 0;
    }

    while (1) {
        /* blink LED */
        _led_blink(LED0_NODE, K_MSEC(100u));

        /* send a LoRa TX */
        ret = lora_send(lora_dev, reinterpret_cast<uint8_t*>(data), MAX_DATA_LEN);
        if (ret < 0) {
            LOG_ERR("LoRa send failed");
            return 0;
        }

        /* Send data at 1s interval */
        LOG_INF("Data sent!");
        k_sleep(K_SECONDS(10));
    }

    return 0;
}