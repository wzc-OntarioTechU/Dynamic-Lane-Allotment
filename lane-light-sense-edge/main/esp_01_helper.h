#ifndef ESP_01_HELPER_H_
#define ESP_01_HELPER_H_
#include <strings.h>

static void esp_01_uart_init();

static void send_udp_packet_esp01(char* data, size_t len);

static void recv_udp_packet_esp01();

static void set_recv_handler(void (*function)(char*, size_t));

#endif