#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>
#include <squirrel_consumer.h>
#include <squirrel_keyboard.h>
#include <stdio.h>
#include <string.h>

#include "communication.h"

uint8_t address = 0b00010111;
i2c_inst_t *slave_i2c_inst;
i2c_inst_t *master_i2c_inst;

uint8_t keyboard_recv_buf[32];
uint8_t keyboard_recv_buf_index = 0;
uint8_t keyboard_modifier_buf[1];
uint8_t consumer_buf[2];
uint8_t rgb_buf[3];

uint8_t com_type = 0;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master has written some data
    if (com_type == 0) {
      com_type = i2c_read_byte_raw(i2c);
    };
    switch (com_type) {
    case COM_TYPE_COMMIT:
      printf("C");
      com_type = 0;
      break;
    case COM_TYPE_KEYBOARD_KEYCODE:
      keyboard_recv_buf[keyboard_recv_buf_index] = i2c_read_byte_raw(i2c);
      keyboard_recv_buf_index++;
      if (keyboard_recv_buf_index == 32) {
        keyboard_recv_buf_index = 0;
        com_type = 0;
      }
      break;
    case COM_TYPE_KEYBOARD_MODIFIER:
      break;
    case COM_TYPE_CONSUMER:
      break;
    default:
      printf("U");
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

void communication_init(i2c_inst_t *master_i2c, i2c_inst_t *slave_i2c,
                        uint8_t addr) {
  address = addr;
  slave_i2c_inst = slave_i2c;
  master_i2c_inst = master_i2c;
  i2c_slave_init(slave_i2c_inst, address, &i2c_slave_handler);
};

uint8_t commit_buf[1] = {COM_TYPE_COMMIT};
uint8_t send_kb_kc_buf[7] = {COM_TYPE_KEYBOARD_KEYCODE};
uint8_t send_kb_mod_buf[2] = {COM_TYPE_KEYBOARD_MODIFIER};
uint8_t send_consumer_buf[3] = {COM_TYPE_CONSUMER};

void communication_task(mutex_t *i2c_mutex) {
  memset(send_kb_kc_buf + 1, 0, 6);
  uint8_t active_keycodes[6];
  keyboard_get_keycodes(&active_keycodes);
  memcpy(send_kb_kc_buf + 1, active_keycodes, 6);
  send_kb_mod_buf[1] = keyboard_get_modifiers();
  send_consumer_buf[1] = consumer_code & 0xFF;
  send_consumer_buf[2] = consumer_code >> 8;

  /*mutex_enter_blocking(i2c_mutex);*/
  i2c_write_blocking(master_i2c_inst, address, send_kb_kc_buf, 7, false);
  sleep_ms(5);
  /*i2c_write_blocking(master_i2c_inst, address, send_kb_mod_buf, 2, false);*/
  /*sleep_ms(5);*/
  /*i2c_write_blocking(master_i2c_inst, address, send_consumer_buf, 3, false);*/
  /*sleep_ms(5);*/
  i2c_write_blocking(master_i2c_inst, address, commit_buf, 1, false);
  sleep_ms(5);
  /*mutex_exit(i2c_mutex);*/
}
