#ifndef BT_HELPER_H_
#define BT_HELPER_H_
#include "esp_gap_ble_api.h"

static void init_bt();

static void start_bt_scanning();

static void set_bt_device_callback(void (*function)(char*, double));

static void bt_scan_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *params);

#endif