#pragma once

#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>

enum com_type {
  COM_TYPE_COMMIT = 0,
  COM_TYPE_KEYBOARD_KEYCODE,
  COM_TYPE_KEYBOARD_MODIFIER,
  COM_TYPE_CONSUMER,
  COM_TYPE_RGB,
};

void communication_init(i2c_inst_t *master_i2c_inst, i2c_inst_t *slave_i2c_inst,
                        uint8_t addr);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

void communication_task(mutex_t *i2c_mutex);
