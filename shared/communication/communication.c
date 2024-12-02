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
uint8_t packet_send_buffer[9];
int i2c_recv_index = -1;
uint8_t packet_recv_buffer[9];

int last_com = -1;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master sent a byte
    if (last_com == -1) {
      last_com = i2c_read_byte_raw(i2c);
      if (last_com == COM_TYPE_WANT_PACKET) {
        get_packet(&packet_send_buffer);
      }
      return;
    }
    switch (last_com) {
    case COM_TYPE_PACKET:
      i2c_recv_index++;
      packet_recv_buffer[i2c_recv_index] = i2c_read_byte_raw(i2c);
      break;
    }
    break;
  case I2C_SLAVE_REQUEST: // master waiting for a byte
    if (last_com == COM_TYPE_WANT_PACKET) {
      i2c_sent_index++;
      i2c_write_byte_raw(i2c, packet_send_buffer[i2c_sent_index]);
      if (i2c_sent_index == 8) {
        i2c_sent_index = -1;
        last_com = -1;
      }
    } else {
      i2c_write_byte_raw(i2c, 0);
    }
    break;
  case I2C_SLAVE_FINISH: // master STOP / RESTART
    if (last_com == COM_TYPE_PACKET) {
      process_packet(&packet_recv_buffer);
    }
    if (last_com != COM_TYPE_WANT_PACKET) {
      last_com = -1;
    }
    i2c_recv_index = -1;
    i2c_sent_index = -1;
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

bool lasthadUSB = false;

void communication_task(mutex_t *i2c_mutex, bool usb_present) {
  /*gpio_put(16, 1);*/
  /*gpio_put(17, 1);*/
  gpio_put(25, !usb_present);
  hasUSB = usb_present;

  if (!hasUSB && lasthadUSB) { // USB just disconnected

    uint8_t buffer[10] = {0}; // Nothing pressed, honest! ;)
    buffer[0] = COM_TYPE_PACKET;

    mutex_enter_blocking(i2c_mutex);
    i2c_write_timeout_us(master_i2c_inst, their_address, buffer, 10, false,
                         1000);
    mutex_exit(i2c_mutex);
    return;
  }

  lasthadUSB = hasUSB;

  // If we don't have USB, send data.
  if (!hasUSB) {
    uint8_t squirrel_data[9];
    get_packet(&squirrel_data);
    uint8_t buffer[10];
    buffer[0] = COM_TYPE_PACKET;
    memcpy(buffer + 1, squirrel_data, 9);
    mutex_enter_blocking(i2c_mutex);
    i2c_write_timeout_us(master_i2c_inst, their_address, buffer, 10, false,
                         1000);
    mutex_exit(i2c_mutex);
  };

  // If we have USB, ask for data.
  if (hasUSB) {
    uint8_t buffer[1] = {COM_TYPE_WANT_PACKET};
    uint8_t recv_buffer[9];
    mutex_enter_blocking(i2c_mutex);
    int write = i2c_write_timeout_us(master_i2c_inst, their_address, buffer, 1,
                                     false, 1000);
    if (write != 1) {
      gpio_put(16, 0);
      mutex_exit(i2c_mutex);
      return;
    } else {
      gpio_put(16, 1);
    }
    int read = i2c_read_timeout_us(master_i2c_inst, their_address, recv_buffer,
                                   9, false, 1000);
    mutex_exit(i2c_mutex);
    if (read == 9) {
      gpio_put(17, 1);
      process_packet(&recv_buffer);
    } else {
      gpio_put(17, 0);
    }
  };
}
