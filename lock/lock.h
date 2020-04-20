#include <Arduino.h>
#include <homekit/homekit.h>
#include <homekit/types.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>

typedef enum
{
    lock_state_unsecured = 0,
    lock_state_secured = 1,
    lock_state_jammed = 2,
    lock_state_unknown = 3,
} lock_state_t;

void lock_lock();
void lock_timeout();
void lock_unlock();
void lock_setter(lock_state_t required_state);
void lock_target_state_setter(homekit_value_t value);
void lock_control_point(homekit_value_t value);
void lock_identify(homekit_value_t value);
