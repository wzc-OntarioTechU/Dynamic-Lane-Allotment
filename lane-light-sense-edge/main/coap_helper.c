#include "coap3/coap.h"
#include "coap3/libcoap.h"
#include "coap3/coap_pdu.h"
#include "esp_log.h"

#define PORT CONFIG_CONFIG_COAP_PORT
#define ADDRESS CONFIG_CONFIG_COAP_SERVER_PORT


static const char* TAG = "COAP_CLIENT";
static coap_context_t* context;
static coap_address_t* address;
static coap_session_t* session;
static void (*receiveFunction)(char* data, size_t length);

static int coap_init() {
    // set up context
    ESP_LOGI(TAG, "Retrieving COAP context.");
    context = coap_new_context(NULL);
    if (!context) {
        ESP_LOGE(TAG, "Failed to retrieve COAP context.");
        return 1;
    }

    // set up address
    ESP_LOGI(TAG, "Setting COAP server address.");
    coap_address_init(address);
    (*address).addr.sin.sin_family = AF_INET;
    (*address).addr.sin.sin_port = htons(PORT);
    (*address).addr.sin.sin_addr.s_addr = inet_addr(ADDRESS);

    // set up session
    ESP_LOGI(TAG, "Retrieving COAP session.");
    session = coap_new_client_session(context, NULL, address, COAP_PROTO_UDP);
    if(!session) {
        ESP_LOGE(TAG, "Failed to retrieve COAP session.");
        return 1;
    }

    ESP_LOGI(TAG, "Initalized COAP Connection and Session.");
    return 0; // return 0 indicating success
}

static void coap_cleanup() {
    ESP_LOGI(TAG, "Cleaning up COAP session and context.");
    coap_session_release(session);
    coap_free_context(context);
}

static int send_message_reading(char* data, size_t length) {
    coap_pdu_t* msg = coap_new_pdu(COAP_MESSAGE_NON, COAP_REQUEST_PUT, session);
    if (!msg) {
        ESP_LOGE(TAG, "Failed to create message PDU for COAP session.");
        return 1;
    }

    coap_add_data(msg, length, data);
    coap_add_option(msg, COAP_OPTION_URI_PATH, 7, "reading");

    ESP_LOGI(TAG, "Sending message to reading.");
    if (coap_send(session, msg) == COAP_INVALID_TID) {
        ESP_LOGE(TAG, "Failed to send message to reading.");
        return 1;
    }
    return 0;
}

static void recieve_message() {
    coap_io_process(context, COAP_IO_NO_WAIT);
}

static void set_receive_func(void (*function)(char*, size_t)) {
    receiveFunction = function;
}

static void coap_response_handler(coap_session_t *sess, const coap_pdu_t *sent, const coap_pdu_t *recv, const coap_mid_t mid) {
    size_t length;
    unsigned char* data;

    ESP_LOGI(TAG, "Recieved COAP message.");
    if (coap_get_data(recv, &length, &data)) {
        ESP_LOGI(TAG, "Recieved COAP message data.");
        (*receiveFunction)(data, length);
    } else {
        ESP_LOGE(TAG, "Recieved COAP message but failed to retrieve data.");
    }
}