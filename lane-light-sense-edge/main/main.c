#include "wifi_helper.h"
#include "coap_helper.h"
#include "led_helper.h"

bool light_strip = false;

// call on the coap receiver
void coap_receive_task() {
    while (true) {
        recieve_message();
        // some very small delay to give a change for other things to run
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void coap_msg_receiver(char* data, size_t length) {

}

void ble_scan_task() {
    
}

void app_main() {
    // init wifi
    wifi_sta_init();
    // init coap
    coap_init();
    set_receive_func(&coap_msg_receiver);
    // init led strip
    led_strip_init();


}