#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>
#include <squirrel_consumer.h>
#include <squirrel_keyboard.h>
#include <string.h>

#include "communication.h"

uint8_t address = 0b00010111;
i2c_inst_t *slave_i2c_inst;
i2c_inst_t *master_i2c_inst;

void communication_init(i2c_inst_t *master_i2c, i2c_inst_t *slave_i2c,
                        uint8_t addr) {
  address = addr;
  slave_i2c_inst = slave_i2c;
  master_i2c_inst = master_i2c;
  i2c_slave_init(slave_i2c_inst, address, &i2c_slave_handler);
};

uint8_t keyboard_recv_buf[32];
uint8_t keyboard_modifier_buf[1];
uint8_t consumer_buf[2];
uint8_t rgb_buf[3];

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master has written some data
    switch (i2c_read_byte_raw(slave_i2c_inst)) {
    case COM_TYPE_COMMIT:
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 8; j++) {
          keyboard_keycodes[(i * 32) + j] |= (keyboard_recv_buf[i] >> j) & 1;
        }
      }
      keyboard_modifiers = keyboard_modifier_buf[0];
      consumer_code = consumer_buf[0] | (consumer_buf[1] << 8);
      break;
    case COM_TYPE_KEYBOARD_KEYCODE:
      i2c_read_raw_blocking(slave_i2c_inst, keyboard_recv_buf, 32);
      break;
    case COM_TYPE_KEYBOARD_MODIFIER:
      i2c_read_raw_blocking(slave_i2c_inst, keyboard_modifier_buf, 1);
      break;
    case COM_TYPE_CONSUMER:
      i2c_read_raw_blocking(slave_i2c_inst, consumer_buf, 2);
      break;
    default:
      break;
    }
    break;
  case I2C_SLAVE_REQUEST: // master is requesting data
    break;
  case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
    break;
  default: // other/unknown event
    break;
  }
}

void communication_task(mutex_t *i2c_mutex) {
  uint8_t kb_send_buf[256];
  kb_send_buf[0] = COM_TYPE_KEYBOARD_KEYCODE;
  memcpy(&kb_send_buf[1], keyboard_keycodes, 255);

  uint8_t keyboard_mod_buf[2] = {COM_TYPE_KEYBOARD_MODIFIER,
                                 keyboard_modifiers};

  uint8_t consumer_buf[3] = {COM_TYPE_CONSUMER, consumer_code & 0xFF,
                             consumer_code >> 8};

  uint8_t commit_buf[1] = {COM_TYPE_COMMIT};
  mutex_enter_blocking(i2c_mutex);
  i2c_write_blocking(master_i2c_inst, address, kb_send_buf, 256, true);
  i2c_write_blocking(master_i2c_inst, address, keyboard_mod_buf, 2, true);
  i2c_write_blocking(master_i2c_inst, address, consumer_buf, 3, true);
  i2c_write_blocking(master_i2c_inst, address, commit_buf, 1, false);
  mutex_exit(i2c_mutex);
}
