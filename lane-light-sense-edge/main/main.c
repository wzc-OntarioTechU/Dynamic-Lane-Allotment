#include "bt_helper.h"
#include "esp_01_helper.h"
#include "led_helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_log.h"

#define ST_ID CONFIG_CONFIG_STATION_NUM

static const char* TAG = "APP_MAIN";

void bt_handle_func(char* mac, int rssi) {
    char msg[32];
    memset(msg, '\0', 32);
    snprintf(msg, 32, "%d;%s,%d;", ST_ID, mac, rssi);
    send_udp_packet_esp01(msg, strlen(msg));
}

void udp_handle_func(char* data, size_t length) {
    char* in_st_id = strtok(data, ",;");
    char* val = strtok(NULL, ",;");

    if (in_st_id == NULL || val == NULL) {
        ESP_LOGE(TAG, "Failed to tokenize received message.");
        return;
    }
    if (atoi(in_st_id) == ST_ID) {
        if (atoi(val) == 1) {
            set_all_colour(0, 255, 80);
        } else {
            set_all_colour(0, 0, 0);
        }
    }
}

void udp_task(void* params) {
    while (true) {
        recv_udp_packet_esp01();
    }
}

void bt_task(void* params) {
    while (true) {
        start_bt_scanning();
        vTaskDelay(pdMS_TO_TICKS(30 * 1000));
    }
}

void app_main() {
    // start bt first
    set_bt_device_callback(bt_handle_func);
    init_bt();

    // start wifi next
    set_recv_handler(udp_handle_func);
    esp_01_uart_init();

    // start led strip last
    led_strip_init();

    xTaskCreate(udp_task, "UDP_RECV_TASK", 2048, NULL, 2, NULL);
    xTaskCreate(bt_task, "BT_SCAN_TASK", 2048, NULL, 2, NULL);
}