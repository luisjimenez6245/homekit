/*
 * simple_led_accessory.c
 * Define the accessory in pure C language using the Macro in characteristics.h
 *
 *  Created on: 2020-04-20
 *      Author: luisjimenez6245 (Luis Jiménez)
 */

#include <Arduino.h>
#include "lock.h"
#include <homekit/homekit.h>
#include <homekit/types.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>

#define PIN_BUTTON 4 //D4
#define PIN_RELAY 12 //D4
#define PIN_LED 2	 //D4
#define UNLOCK_PERIOD 1 //D4

lock_state_t current_state;
homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "ESP8266_LOCK");
homekit_characteristic_t lock_current_state = HOMEKIT_CHARACTERISTIC_(
    LOCK_CURRENT_STATE,
    lock_state_unknown, );
homekit_characteristic_t lock_target_state = HOMEKIT_CHARACTERISTIC_(
    LOCK_TARGET_STATE,
    lock_state_secured,
    .setter = lock_target_state_setter, );
homekit_characteristic_t lock_control = HOMEKIT_CHARACTERISTIC_(
    LOCK_CONTROL_POINT,
    .setter = lock_control_point, );

void lock_lock()
{
	digitalWrite(PIN_LED, LOW);
	digitalWrite(PIN_RELAY, LOW);
}

void lock_timeout()
{
	if (lock_target_state.value.int_value != lock_state_secured)
	{
		lock_setter(lock_state_secured);
	}
}

void lock_unlock()
{
	digitalWrite(PIN_LED, HIGH);
	digitalWrite(PIN_RELAY, HIGH);
	if (UNLOCK_PERIOD)
	{
		delay(UNLOCK_PERIOD * 1000);
		lock_timeout();
	}
}
void lock_setter(lock_state_t required_state)
{
	if (required_state != lock_target_state.value.int_value)
	{
		lock_current_state.value = HOMEKIT_UINT8(!required_state ? lock_state_secured : lock_state_unsecured);
		if (!required_state)
		{
			lock_unlock();
		}
		else
		{
			lock_lock();
		}
		homekit_characteristic_notify(&lock_current_state, lock_target_state.value);
	}
}

void lock_target_state_setter(homekit_value_t value)
{
	if (value.format != homekit_format_int)
	{
		return;
	}
	lock_setter(value.int_value);
}

void lock_control_point(homekit_value_t value)
{
}

void lock_identify(homekit_value_t value)
{
	printf("accessory identify\n");
	for (int j = 0; j < 3; j++)
	{
		digitalWrite(PIN_LED, HIGH);
		delay(100);
		digitalWrite(PIN_LED, LOW);
		delay(100);
		digitalWrite(PIN_LED, HIGH);
		delay(100);
		digitalWrite(PIN_LED, LOW);
	}
}

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(
      .id=1,
      .category=homekit_accessory_category_door_lock, 
      .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC_(NAME, "ESP8266_LOCK"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Home"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "SN_0123456"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.9"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, lock_identify),
            NULL
        }),
        HOMEKIT_SERVICE(LOCK_MECHANISM, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC_(NAME, "LOCK"),
            &lock_current_state,
            &lock_target_state,
            NULL
        }),
        HOMEKIT_SERVICE(LOCK_MANAGEMENT, .characteristics=(homekit_characteristic_t*[]){
            &lock_control,
            HOMEKIT_CHARACTERISTIC(VERSION, "1"),
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
	.accessories = accessories,
	.password = "111-11-111",
	.setupId = "ABCD"
};

void accessory_init()
{
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_RELAY, OUTPUT);
	pinMode(PIN_BUTTON, INPUT_PULLUP);
	lock_setter(lock_state_secured);
}
