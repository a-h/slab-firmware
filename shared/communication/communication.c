#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <squirrel_consumer.h>
#include <squirrel_keyboard.h>
#include <squirrel_split.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "communication.h"
#include <display.h>

i2c_inst_t *slave_i2c_inst;
uint8_t our_address = 0b00010111;

i2c_inst_t *master_i2c_inst;
uint8_t their_address = 0b00010111;

int i2c_sent_index = -1;
uint8_t packet_send_buffer[9];
int i2c_recv_index = -1;
uint8_t packet_recv_buffer[9];

int last_com = -1;

uint8_t extra_data = 0b00000000;

int alive_count = 0;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  case I2C_SLAVE_RECEIVE: // master sent a byte
    if (last_com == -1) {
      last_com = i2c_read_byte_raw(i2c);
      if (last_com == COM_TYPE_WANT_PACKET) {
        get_packet(&packet_send_buffer);
        i2c_sent_index = -2;
      }
      if (last_com == COM_TYPE_ALIVE) {
        alive_count++;
        last_com = -1;
      }
      return;
    }
    switch (last_com) {
    case COM_TYPE_PACKET:
      i2c_recv_index++;
      packet_recv_buffer[i2c_recv_index] = i2c_read_byte_raw(i2c);
      break;
    case COM_TYPE_EXTRA:
      extra_data = i2c_read_byte_raw(i2c);
      screensaver = extra_data & 0b00000001;
      break;
    }
    break;
  case I2C_SLAVE_REQUEST: // master waiting for a byte
    if (i2c_sent_index == -1) {
      i2c_write_byte_raw(i2c, 0);
      return;
    }
    switch (last_com) {
    case COM_TYPE_WANT_PACKET:
      if (i2c_sent_index == -2) {
        i2c_sent_index = 0;
      }
      i2c_write_byte_raw(i2c, packet_send_buffer[i2c_sent_index]);
      if (i2c_sent_index == 8) {
        last_com = -1;
        i2c_sent_index = -1;
      }
      i2c_sent_index++;
      break;
    case COM_TYPE_WANT_EXTRA:
      i2c_write_byte_raw(i2c, extra_data);
      last_com = -1;
      break;
    }
    break;
  case I2C_SLAVE_FINISH: // master STOP / RESTART
    if (last_com == COM_TYPE_PACKET || last_com == COM_TYPE_EXTRA) {
      last_com = -1;
      i2c_recv_index = -1;
      process_packet(&packet_recv_buffer);
    }
    if ((last_com == COM_TYPE_WANT_PACKET || last_com == COM_TYPE_WANT_EXTRA) &&
        i2c_sent_index != -2) {
      i2c_sent_index = -1;
      last_com = -1;
    };
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

bool last_usb_present = false;

uint32_t last_last_millis = 0;
uint32_t last_millis = 0;
int last_alive_count = 1;

void communication_task(bool usb_present, bool should_screensaver,
                        uint32_t millis) {
  if (millis > last_millis + 500) {
    last_millis = millis;
    uint8_t buffer[1] = {COM_TYPE_ALIVE};
    i2c_write_blocking(master_i2c_inst, their_address, buffer, 1, false);
  }
  if (millis > last_last_millis + 1000) {
    last_last_millis = millis;
    if (last_alive_count >= alive_count) {
      rightmost = true;
      last_alive_count = alive_count;
    } else {
      rightmost = false;
    }
  }

  uint8_t throwaway[1];
  int read_success =
      i2c_read_blocking(master_i2c_inst, their_address, throwaway, 1, false);
  if (read_success != 1) {
    leftmost = true;
  }

  if (!usb_present) {
    // send a SQUIRERL packet
    uint8_t squirrel_data[9];
    get_packet(&squirrel_data);
    uint8_t buffer[10];
    buffer[0] = COM_TYPE_PACKET;
    memcpy(buffer + 1, squirrel_data, 9);
    i2c_write_blocking(master_i2c_inst, their_address, buffer, 10, false);
  };

  if (usb_present) {
    // For unknown reasons, the first I2C read we perform returns garbage data -
    // so we read once and discard the data
    if (!last_usb_present) {
      uint8_t recv_buffer[9];
      int read = i2c_read_blocking(master_i2c_inst, their_address, recv_buffer,
                                   9, false);
    }
    last_usb_present = usb_present;

    // ask for a SQUIRREL packet
    uint8_t buffer[1] = {COM_TYPE_WANT_PACKET};
    int write =
        i2c_write_blocking(master_i2c_inst, their_address, buffer, 1, false);
    if (write != 1) {
      return;
    }
    uint8_t recv_buffer[9] = {0};
    int read = i2c_read_blocking(master_i2c_inst, their_address, recv_buffer, 9,
                                 false);
    if (read != 9) {
      return;
    }
    process_packet(&recv_buffer);
  };
}
