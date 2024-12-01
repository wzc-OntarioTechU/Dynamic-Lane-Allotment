#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "led_strip.h"

#define PIN CONFIG_CONFIG_LED_PIN
#define LED_COUNT CONFIG_CONFIG_LED_COUNT

static const char* TAG = "LED_STRIP";

static led_strip_handle_t* strip = NULL;

static int led_strip_init() {
    strip = led_strip_new(PIN, LED_COUNT);
    if (strip == NULL) {
        ESP_LOGE(TAG, "Failed to initiate LED strip.");
        return 1;
    }
    ESP_LOGI(TAG, "LED strip initialized.");
    return 0;
}

static int set_all_colour(uint8_t red, uint8_t green, uint8_t blue) {
    ESP_LOGE(TAG, "Setting led strip to a colour.");
    for (int i = 0; i < LED_COUNT; i++) {
        if (led_strip_set_pixel(strip, i, red, green, blue) == ESP_FAIL)
            return 1;
    }
    return (led_strip_refresh(strip) == ESP_FAIL) ? 1 : 0;
}