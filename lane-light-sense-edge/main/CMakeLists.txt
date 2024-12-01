idf_component_register(SRCS  "main.c" "led_helper.c" "esp_01_helper.c" "bt_helper.c"
                    INCLUDE_DIRS "."
                    REQUIRES bt nvs_flash driver)
