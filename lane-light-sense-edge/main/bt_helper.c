#include "esp_system.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char* TAG = "BT_HELPER";
static void (*bt_device_callback)(char*, int);

static const esp_ble_scan_params_t bt_scan_params = {
    .scan_type = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval = 0x100, // 100ms interval
    .scan_window = 0x50, // 50ms scan duration
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE
};

void init_bt() {
    ESP_LOGI(TAG, "Initalizing BT.");
    esp_err_t ret_val = nvs_flash_init();
    if (ret_val == ESP_ERR_NVS_NO_FREE_PAGES || ret_val == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret_val = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret_val);

    ESP_LOGI(TAG, "Flashed NVS, initializing ESP BT controller.");
    esp_bt_controller_config_t bt_config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_config));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BTDM));

    ESP_LOGI(TAG, "Initalized ESP BT controller, initializing bluedroid.");
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_LOGI(TAG, "Done BT init.");
}

void bt_scan_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *params) {
    if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {
        if (params->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
            char mac[18];
            mac[17] = '\0';
            int rssi = params->scan_rst.rssi;
            snprintf(mac, 18, "%02x:%02x:%02x:%02x:%02x:%02x", params->scan_rst.bda[0], params->scan_rst.bda[1], params->scan_rst.bda[2], params->scan_rst.bda[3], params->scan_rst.bda[4], params->scan_rst.bda[5]);
            (*bt_device_callback)(mac, rssi);
        }
    }
}

void start_bt_scanning() {
    ESP_LOGI(TAG, "Starting BT scanning...");
    ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&bt_scan_params));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(bt_scan_callback));
    ESP_ERROR_CHECK(esp_ble_gap_start_scanning(30)); // scan for the max value of uint32_t seconds (2^32 - 1)s
}

void set_bt_device_callback(void (*function)(char*, int)) {
    bt_device_callback = function;
}