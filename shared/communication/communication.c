#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>
#include <squirrel_consumer.h>
#include <squirrel_keyboard.h>
#include <squirrel_split.h>
#include <stdio.h>
#include <string.h>

#include "communication.h"

uint8_t address = 0b00010111;
i2c_inst_t *slave_i2c_inst;
i2c_inst_t *master_i2c_inst;

uint8_t squirrel_recv_buf[9];
uint8_t squirrel_recv_buf_index = 0;

uint8_t com_type =
    0; // If 0, we need to read a byte to determine the new type of data.

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master has written some data
    if (com_type == 0) {
      com_type = i2c_read_byte_raw(i2c);
      break;
    };
    switch (com_type) {
    case COM_TYPE_SQUIRREL:
      squirrel_recv_buf[squirrel_recv_buf_index] = i2c_read_byte_raw(i2c);
      squirrel_recv_buf_index++;
      break;
    default:
      printf("U");
      break;
    }
    break;
  case I2C_SLAVE_REQUEST: // master is requesting data
    break;
  case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
    squirrel_recv_buf_index = 0;
    com_type = 0;
    process_packet(&squirrel_recv_buf);
    memset(squirrel_recv_buf, 0, 9);
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

uint8_t squirrel_send_buf[9] = {0};
uint8_t i2c_send_buf[10] = {COM_TYPE_SQUIRREL};

void communication_task(mutex_t *i2c_mutex) {
  get_packet(&squirrel_send_buf);
  memcpy(i2c_send_buf + 1, squirrel_send_buf, 9);

  /*mutex_enter_blocking(i2c_mutex);*/
  i2c_write_blocking(master_i2c_inst, address, i2c_send_buf, 10, false);
  /*mutex_exit(i2c_mutex);*/
}
