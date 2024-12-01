#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"

#define PORT CONFIG_CONFIG_ESP_01_PORT
#define TX_PIN CONFIG_CONFIG_ESP_01_TX_PIN
#define RX_PIN CONFIG_CONFIG_ESP_01_RX_PIN
#define BAUD CONFIG_CONFIG_ESP_01_BAUD
#define BUFFER_SIZE CONFIG_CONFIG_ESP_01_BUFFER_SIZE
#define WIFI_SSID CONFIG_ESP_WIFI_SSID
#define WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define COAP_PORT CONFIG_CONFIG_COAP_PORT
#define ADDRESS CONFIG_CONFIG_COAP_SERVER_PORT

static const char* TAG = "ESP-01_Helper";
static void (*recvHandler)(char* data, size_t length);

void esp_01_uart_init() {
    const uart_config_t config = {
        .baud_rate = BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_driver_install(PORT, BUFFER_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(PORT, &config));
    ESP_ERROR_CHECK(uart_set_pin(PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    char* station_cmd = "AT+CWMODE=1\r\n";
    uart_write_bytes(PORT, station_cmd, strlen(station_cmd));
    char station_connect_cmd[1 + strlen("AT+CWJAP=\"\",\"\"\r\n") + strlen(WIFI_SSID) + strlen(WIFI_PASS)];
    snprintf(station_connect_cmd, sizeof(station_connect_cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASS);
    uart_write_bytes(PORT, station_connect_cmd, strlen(station_connect_cmd));
    char* multi_cmd = "AT+CIPMUX=1\r\n";
    uart_write_bytes(PORT, multi_cmd, strlen(multi_cmd));
    char server_connect_cmd[1 + strlen("AT+CIPSTART=\"UDP\",\"\",\r\n") + strlen(ADDRESS) + 6];
    snprintf(server_connect_cmd, sizeof(server_connect_cmd), "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n", ADDRESS, COAP_PORT);
    uart_write_bytes(PORT, server_connect_cmd, strlen(server_connect_cmd));
}

void send_udp_packet_esp01(char* data, size_t len) {
    char send_len_cmd[512];
    snprintf(send_len_cmd, sizeof(send_len_cmd), "AT+CIPSEND=%d\r\n", len);
    uart_write_bytes(PORT, send_len_cmd, strlen(send_len_cmd));
    uart_write_bytes(PORT, data, len);
}

void recv_udp_packet_esp01() {
    char* data = malloc(BUFFER_SIZE);
    int packet_len = 0;
    char* packet;

    while (packet_len == 0) {
        int len = uart_read_bytes(PORT, data, BUFFER_SIZE, pdMS_TO_TICKS(50));
        if (len > 0) {
            data[len] = '\0';

            packet = strstr(data, "+IPD,");
            if (packet) {
                char* start = strchr(packet, ',') + 1;
                packet_len = atoi(start);

                start = strstr(packet, ":") + 1;
                int payload_offset = start - data;

                if (len >= payload_offset + packet_len) {
                    char* pdu = malloc(packet_len);
                    memcpy(pdu, start, packet_len);
                    (*recvHandler)(pdu, packet_len);
                    free(pdu);
                }
            }
        }
    }

    free(data);
}

void set_recv_handler(void (*function)(char*, size_t)) {
    recvHandler = function;
}