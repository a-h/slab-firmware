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

bool should_send_accumulation_packet = false;
bool slave_done_accumulating = false;

uint8_t accumulation_buffer[11] = {0};
int accumulation_buffer_index = 0;

uint8_t accumulation_status_buffer[12] = {0};
int accumulation_status_buffer_index = 0;

int alive_count = 0;
int last_com = -1;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
  // Recive data from the master
  case I2C_SLAVE_RECEIVE:
    // Each 'packet' has at least 1 header byte that tells us the COM_TYPE
    // If we don't have a com_type, use this byte to determine it.
    if (last_com == -1) {
      // Read the header byte
      last_com = i2c_read_byte_raw(i2c);
      // COM_TYPE_ALIVE is a special case, there is no data to read, so we just
      // increment the alive_count and pretend like nothing happened.
      switch (last_com) {
      case COM_TYPE_ALIVE: {
        alive_count++;
        last_com = -1;
        break;
      }
      case COM_TYPE_ACCUMULATION_PACKET: {
        accumulation_buffer_index = 0;
        break;
      }
      case COM_TYPE_WANT_ACCUMULATION_STATUS: {
        accumulation_status_buffer_index = 0;
        uint8_t packet_buffer[11];
        get_packet(&packet_buffer);
        memcpy(accumulation_status_buffer + 1, packet_buffer, 11);
        if (slave_done_accumulating || leftmost) {
          accumulation_status_buffer[0] = COM_TYPE_DONE_ACCUMULATING;
        } else {
          accumulation_status_buffer[0] = COM_TYPE_NOT_DONE_ACCUMULATING;
        }
        break;
      }
      }

      return;
    }

    // We have a com_type, so we can read the rest of the packet based on it.
    switch (last_com) {
    case COM_TYPE_ACCUMULATION_PACKET: {
      accumulation_buffer[accumulation_buffer_index] = i2c_read_byte_raw(i2c);
      accumulation_buffer_index++;
      break;
    }
    default: {
      i2c_read_byte_raw(i2c);
    }
    }
    break;
  // Send data to the master
  case I2C_SLAVE_REQUEST:
    switch (last_com) {
    case COM_TYPE_WANT_ACCUMULATION_STATUS: {
      i2c_write_byte_raw(
          i2c, accumulation_status_buffer[accumulation_status_buffer_index]);
      accumulation_status_buffer_index++;
      if (accumulation_status_buffer_index == 12) {
        last_com = -1;
      }
      break;
    }
    default: {
      i2c_write_byte_raw(i2c, 0);
    }
    }
    break;
  case I2C_SLAVE_FINISH: // master STOP / RESTART
    switch (last_com) {
    case COM_TYPE_ACCUMULATION_PACKET: {
      if (accumulation_buffer_index == 11) {
        process_packet(&accumulation_buffer);
        slave_done_accumulating = false;
        should_send_accumulation_packet = !leftmost;
      }
      last_com = -1;
      break;
    }
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

bool last_usb_present = false;

uint32_t alive_ticker_millis = 0;
uint32_t alive_checker_millis = 0;
int last_alive_count = 1;

// Determine if we are the rightmost or leftmost device
void left_or_right(uint32_t millis) {
  uint8_t throwaway;
  int read_success =
      i2c_read_blocking(master_i2c_inst, their_address, &throwaway, 1, false);
  if (read_success != 1) {
    leftmost = true;
  }
  if (millis > alive_ticker_millis + 25) {
    alive_ticker_millis = millis;
    uint8_t buffer[1] = {COM_TYPE_ALIVE};
    i2c_write_blocking(master_i2c_inst, their_address, buffer, 1, false);
  }
  if (millis > alive_checker_millis + 100) {
    alive_checker_millis = millis;
    if (last_alive_count >= alive_count) {
      rightmost = true;
      last_alive_count = alive_count;
    } else {
      rightmost = false;
    }
    if (!rightmost && !leftmost) {
      central = true;
    }
  }
}

void send_accumulation_packet(void) {
  uint8_t buffer[12];
  buffer[0] = COM_TYPE_ACCUMULATION_PACKET;
  uint8_t squirrel_buffer[11];
  get_packet(&squirrel_buffer);
  memcpy(buffer + 1, squirrel_buffer, 11);
  i2c_write_blocking(master_i2c_inst, their_address, buffer, 12, false);
}

void communication_task(bool usb_present, bool should_screensaver,
                        uint32_t millis) {
  left_or_right(millis);

  if (rightmost && leftmost) {
    screensaver = should_screensaver;
    return;
  }

  screensaver = false;

  if (rightmost || should_send_accumulation_packet) {
    send_accumulation_packet();
    uint8_t accumulation_buffer[11] = {0};
    slave_done_accumulating = false;
    uint8_t read_buffer[12];
    while (!slave_done_accumulating) {
      sleep_ms(5);

      uint8_t buffer[1] = {COM_TYPE_WANT_ACCUMULATION_STATUS};
      i2c_write_blocking(master_i2c_inst, their_address, buffer, 1, false);
      i2c_read_blocking(master_i2c_inst, their_address, read_buffer, 12, false);
      slave_done_accumulating = read_buffer[0] == COM_TYPE_DONE_ACCUMULATING;
    }
    should_send_accumulation_packet = false;
    memcpy(accumulation_buffer, read_buffer + 1, 11);
    process_packet(&accumulation_buffer);
  }
}
