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

i2c_inst_t *slave_i2c_inst;
uint8_t our_address = 0b00010111;

i2c_inst_t *master_i2c_inst;
uint8_t their_address = 0b00010111;

bool hasUSB = false;

int i2c_sent_index = -1;
uint8_t packet_sent_buffer[9];
int i2c_recv_index = -1;
uint8_t packet_recv_buffer[9];

int last_com = -1;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master sent a byte
    if (last_com == -1) {
      last_com = i2c_read_byte_raw(i2c);
      break;
    }
    switch (last_com) {
    case COM_TYPE_WANT_PACKET:
      get_packet(&packet_sent_buffer);
      break;
    case COM_TYPE_PACKET:
      packet_recv_buffer[++i2c_sent_index] = i2c_read_byte_raw(i2c);
      break;
    default:
      break;
    }
    break;
  case I2C_SLAVE_REQUEST: // master waiting for a byte
    if (last_com == COM_TYPE_WANT_PACKET) {
      i2c_sent_index++;
      i2c_write_byte_raw(i2c, packet_sent_buffer[i2c_sent_index]);
    }
    break;
  case I2C_SLAVE_FINISH: // master STOP / RESTART
    if (i2c_recv_index >= 0) {
      process_packet(&packet_recv_buffer);
      i2c_recv_index = -1;
      last_com = -1;
    }
    if (i2c_sent_index >= 0) {
      i2c_sent_index = -1;
      last_com = -1;
    }
    break;
  default:
    break;
  }
}

void communication_init(i2c_inst_t *master_i2c, i2c_inst_t *slave_i2c) {
  slave_i2c_inst = slave_i2c;
  master_i2c_inst = master_i2c;

  i2c_slave_init(slave_i2c_inst, our_address, &i2c_slave_handler);
};

static uint8_t com_want_packet[1] = {COM_TYPE_WANT_PACKET};
static uint8_t com_packet[1] = {COM_TYPE_PACKET};

void communication_task(mutex_t *i2c_mutex, bool usb_present) {
  hasUSB = usb_present;

  uint8_t squirrel_data[9];

  // perform a dummy read - check if we have a slave attached
  uint8_t rx_data;
  mutex_enter_blocking(i2c_mutex);
  if (i2c_read_blocking(master_i2c_inst, their_address, &rx_data, 1, false) <
      0) {
    mutex_exit(i2c_mutex);
    gpio_put(17, 1);
    return; // If the read fails, we don't have a slave, so return.
  }
  mutex_exit(i2c_mutex);

  if (hasUSB) {
    // If we have a USB connection, request a packet
    mutex_enter_blocking(i2c_mutex);
    i2c_write_blocking(master_i2c_inst, their_address, com_want_packet, 1,
                       false);
    i2c_read_blocking(master_i2c_inst, their_address, squirrel_data, 9, false);
    mutex_exit(i2c_mutex);
    process_packet(&squirrel_data);
    return;
  }

  // If we don't have a USB connection, send a packet
  get_packet(&squirrel_data);
  mutex_enter_blocking(i2c_mutex);
  i2c_write_blocking(master_i2c_inst, their_address, com_packet, 1, false);
  i2c_write_blocking(master_i2c_inst, their_address, squirrel_data, 9, false);
  mutex_exit(i2c_mutex);
}
