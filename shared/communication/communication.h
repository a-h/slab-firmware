#pragma once

#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>

enum com_type {
  // ALIVE packet
  COM_TYPE_ALIVE,
  // Communication chain packets
  COM_TYPE_ACCUMULATION_PACKET,
  COM_TYPE_WANT_ACCUMULATION_STATUS,
  COM_TYPE_DONE_ACCUMULATING,
  COM_TYPE_NOT_DONE_ACCUMULATING,
};

// master_i2c_inst is the i2c_inst_t that the master talks on
// slave_i2c_inst is the i2c_inst_t that the slave listens on
void communication_init(i2c_inst_t *master_i2c_inst,
                        i2c_inst_t *slave_i2c_inst);

void communication_task(bool usb_present, bool should_screensaver,
                        uint32_t millis);
