#pragma once

#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>

enum com_type {
  COM_TYPE_SQUIRREL = 1,
};

void communication_init(i2c_inst_t *master_i2c_inst, i2c_inst_t *slave_i2c_inst,
                        uint8_t addr);

void communication_task(mutex_t *i2c_mutex);
