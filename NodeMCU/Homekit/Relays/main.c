#include <stdio.h>
#include <string.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>
#include <toggle.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#include "wifi.h"


#define MAX_SERVICES 20


static void wifi_init() {
    struct sdk_station_config wifi_config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
    };

    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&wifi_config);
    sdk_wifi_station_connect();
}

const int sensor_gpio = 4;
const int led_gpio = 2;

const uint8_t relay_gpios[] = {
  12, 5, 14, 13
};
const size_t relay_count = sizeof(relay_gpios) / sizeof(*relay_gpios);


void relay_write(int relay, bool on) {
    printf("Relay %d %s\n", relay, on ? "ON" : "OFF");
    gpio_write(relay, on ? 0 : 1);
}

homekit_characteristic_t occupancy_detected = HOMEKIT_CHARACTERISTIC_(OCCUPANCY_DETECTED, 0);

void sensor_callback(bool high, void *context) {
    occupancy_detected.value = HOMEKIT_UINT8(high ? 1 : 0);
    homekit_characteristic_notify(&occupancy_detected, occupancy_detected.value);
}

void led_write(bool on) {
    gpio_write(led_gpio, on ? 0 : 1);
}

void gpio_init() {
    gpio_enable(led_gpio, GPIO_OUTPUT);
    led_write(false);

    for (int i=0; i < relay_count; i++) {
        gpio_enable(relay_gpios[i], GPIO_OUTPUT);
        relay_write(relay_gpios[i], true);
    }
    if (toggle_create(sensor_gpio, sensor_callback, NULL)) {
        printf("Failed to initialize sensor\n");
    }
}

void lamp_identify_task(void *_args) {
    led_write(true);

    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            led_write(true);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            led_write(false);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    led_write(false);

    vTaskDelete(NULL);
}

void lamp_identify(homekit_value_t _value) {
    printf("Lamp identify\n");
    xTaskCreate(lamp_identify_task, "Lamp identify", 256, NULL, 2, NULL);
}

void relay_callback(homekit_characteristic_t *ch, homekit_value_t value, void *context) {
    uint8_t *gpio = context;
    relay_write(*gpio, value.bool_value);
}


homekit_accessory_t *accessories[2];

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "563-09-221"
};

void on_wifi_ready() {
    homekit_server_init(&config);
}


void init_accessory() {
    uint8_t macaddr[6];
    sdk_wifi_get_macaddr(STATION_IF, macaddr);

    int name_len = snprintf(NULL, 0, "Relays-%02X%02X%02X",
                            macaddr[3], macaddr[4], macaddr[5]);
    char *name_value = malloc(name_len+1);
    snprintf(name_value, name_len+1, "Relays-%02X%02X%02X",
             macaddr[3], macaddr[4], macaddr[5]);

    homekit_service_t* services[MAX_SERVICES + 1];
    homekit_service_t** s = services;

    *(s++) = NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
        NEW_HOMEKIT_CHARACTERISTIC(NAME, name_value),
        NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, "HaPK"),
        NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0"),
        NEW_HOMEKIT_CHARACTERISTIC(MODEL, "Relays"),
        NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
        NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, lamp_identify),
        NULL
    });

    for (int i=0; i < relay_count; i++) {
        int relay_name_len = snprintf(NULL, 0, "Relay %d", i + 1);
        char *relay_name_value = malloc(relay_name_len+1);
        snprintf(relay_name_value, relay_name_len+1, "Relay %d", i + 1);
        *(s++) = NEW_HOMEKIT_SERVICE(OUTLET, .characteristics=(homekit_characteristic_t*[]) {
            NEW_HOMEKIT_CHARACTERISTIC(NAME, relay_name_value),
            NEW_HOMEKIT_CHARACTERISTIC(
                ON, true,
                .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
                    relay_callback, .context=(void*)&relay_gpios[i]
                ),
            ),
            NULL
        });
    }
 
   *(s++) = NEW_HOMEKIT_SERVICE(OCCUPANCY_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
            NEW_HOMEKIT_CHARACTERISTIC(NAME, "Occupancy Sensor"),
            &occupancy_detected,
            NULL
        });

    *(s++) = NULL;

    accessories[0] = NEW_HOMEKIT_ACCESSORY(.category=homekit_accessory_category_other, .services=services);
    accessories[1] = NULL;
}

void user_init(void) {
    uart_set_baud(0, 115200);

    init_accessory();

    gpio_init();

    wifi_init();
    on_wifi_ready();
}
