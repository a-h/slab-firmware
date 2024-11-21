#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>
#include <squirrel_consumer.h>
#include <squirrel_keyboard.h>
#include <squirrel_split.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "communication.h"

uint8_t address = 0b00010111;
i2c_inst_t *slave_i2c_inst;
i2c_inst_t *master_i2c_inst;
bool hasUSB = false;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master has written some data
    break;
  case I2C_SLAVE_REQUEST: // master is requesting data
    break;
  case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
    break;
  default: // other/unknown event
    break;
  }
}

void communication_init(i2c_inst_t *master_i2c, i2c_inst_t *slave_i2c,
                        uint8_t addr) {
  address = addr;
  slave_i2c_inst = slave_i2c;
  master_i2c_inst = master_i2c;
  i2c_slave_init(slave_i2c_inst, address, &i2c_slave_handler);
};

void communication_task(mutex_t *i2c_mutex, bool usb_present) {
  hasUSB = usb_present;
  mutex_enter_blocking(i2c_mutex);
  mutex_exit(i2c_mutex);
}
