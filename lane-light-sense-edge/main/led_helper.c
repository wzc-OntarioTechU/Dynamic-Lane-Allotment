#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "led_strip.h"

#define PIN CONFIG_CONFIG_LED_PIN
#define LED_COUNT CONFIG_CONFIG_LED_COUNT
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

static const char* TAG = "LED_STRIP";

led_strip_handle_t strip;
led_strip_config_t strip_config = {
    .strip_gpio_num = PIN,
    .max_leds = LED_COUNT,
    .led_model = LED_MODEL_WS2812,
    .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    .flags = {
        .invert_out = false
    }
};
led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = LED_STRIP_RMT_RES_HZ,
    .mem_block_symbols = 64,
    .flags = {
        .with_dma = false
    }
};

int led_strip_init() {
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &strip));
    ESP_LOGI(TAG, "LED strip initialized.");
    return 0;
}

int set_all_colour(uint8_t red, uint8_t green, uint8_t blue) {
    ESP_LOGE(TAG, "Setting led strip to a colour.");
    for (int i = 0; i < LED_COUNT; i++) {
        if (led_strip_set_pixel(strip, i, red, green, blue) == ESP_FAIL)
            return 1;
    }
    return (led_strip_refresh(strip) == ESP_FAIL) ? 1 : 0;
}