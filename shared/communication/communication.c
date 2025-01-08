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

int alive_count = 0;
int last_com = -1;

int accumulation_index = 0;
uint8_t accumulation_buffer[11];

int sync_index = 0;
uint8_t sync_buffer[11];

bool accumulation_done = false;
bool should_send_accumulation = false;
bool should_send_sync_complete = false;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
    // Recive data from the master
  case I2C_SLAVE_RECEIVE:
    // Each 'packet' has at least 1 header byte that tells us the COM_TYPE

    // If we don't have a com_type, use this byte to determine it.
    if (last_com == -1) {
      // Read the header byte
      last_com = i2c_read_byte_raw(i2c);

      // If the header byte is ALIVE, we don't need to do anything, so increment
      // the counter and pretend nothing happened
      if (last_com == COM_TYPE_ALIVE) {
        alive_count++;
        last_com = -1;
      }

      // If the header byte is ACCUMULATION_PACKET, we need to reset the index
      // to prepare for reading the packet.
      if (last_com == COM_TYPE_ACCUMULATION_PACKET) {
        accumulation_index = 0;
      }

      // If the header byte is WANT_SYNCHRONIZATION_PACKET, we need to write the
      // packet into the buffer for the next 11 bytes we are about to send, and
      // reset the index.
      if (last_com == COM_TYPE_WANT_SYNCHRONIZATION_PACKET) {
        sync_index = 0;
        get_packet(&sync_buffer);
      }

      // If the header byte is SYNCHRONIZATION_COMPLETE, we need to forward it.
      if (last_com == COM_TYPE_SYNCHRONIZATION_COMPLETE && !leftmost) {
        should_send_sync_complete = true;
      }

      // Nothing left to do with this byte.
      return;
    }

    // We have a com_type, so we can read the rest of the packet based on it.
    switch (last_com) {
    case COM_TYPE_ACCUMULATION_PACKET:
      accumulation_buffer[accumulation_index] = i2c_read_byte_raw(i2c);
      accumulation_index++;
      break;
    }
    break;
  // Send data to the master
  case I2C_SLAVE_REQUEST:
    switch (last_com) {
    case COM_TYPE_WANT_ACCUMULATION_STATUS:
      i2c_write_byte_raw(i2c, accumulation_done || leftmost
                                  ? COM_TYPE_DONE_ACCUMULATING
                                  : COM_TYPE_NOT_DONE_ACCUMULATING);
      last_com = -1;
      break;
    case COM_TYPE_WANT_SYNCHRONIZATION_PACKET:
      i2c_write_byte_raw(i2c, sync_buffer[sync_index]);
      sync_index++;
      break;
    }
    break;
  case I2C_SLAVE_FINISH: // master STOP / RESTART
    switch (last_com) {
    case COM_TYPE_ACCUMULATION_PACKET:
      last_com = -1;
      process_packet(&accumulation_buffer);
      if (!leftmost) {
        should_send_accumulation = true;
      }
      break;
    case COM_TYPE_WANT_SYNCHRONIZATION_PACKET:
      if (sync_index == 11) {
        last_com = -1;
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

void send_accumulation_and_wait() {
  // Prepare the send_buffer with the packet data.
  uint8_t send_buffer[12];
  uint8_t packet_buffer[11];
  memset(send_buffer, 0, 12);
  get_packet(&packet_buffer);
  memcpy(send_buffer + 1, packet_buffer, 11);
  send_buffer[0] = COM_TYPE_ACCUMULATION_PACKET;

  // Write the packet to the left device.
  i2c_write_blocking(master_i2c_inst, their_address, send_buffer, 12, false);

  // Constantly read bytes from the left device until we get a
  // DONE_ACCUMULATING
  uint8_t accu_buffer[1] = {COM_TYPE_NOT_DONE_ACCUMULATING};
  uint8_t send_buffer_want[1] = {COM_TYPE_WANT_ACCUMULATION_STATUS};
  while (!accumulation_done) {
    i2c_write_blocking(master_i2c_inst, their_address, send_buffer_want, 1,
                       false);
    i2c_read_blocking(master_i2c_inst, their_address, accu_buffer, 1, false);
    if (accu_buffer[0] == COM_TYPE_DONE_ACCUMULATING) {
      accumulation_done = true;
    }
  }
  uint8_t send_buffer_want_sync[1] = {COM_TYPE_WANT_SYNCHRONIZATION_PACKET};
  i2c_write_blocking(master_i2c_inst, their_address, send_buffer_want_sync, 1,
                     false);
  uint8_t sync_buffer[11];
  i2c_read_blocking(master_i2c_inst, their_address, sync_buffer, 11, false);
  process_packet(&sync_buffer);
  accumulation_done = false;
}

void communication_task(bool usb_present, bool should_screensaver,
                        uint32_t millis) {
  left_or_right(millis);

  if (rightmost && leftmost) {
    return;
  }

  if (should_send_accumulation) {
    should_send_accumulation = false;
    send_accumulation_and_wait();
  }

  if (should_send_sync_complete) {
    should_send_sync_complete = false;
    uint8_t sync_complete_buffer[1] = {COM_TYPE_SYNCHRONIZATION_COMPLETE};
    i2c_write_blocking(master_i2c_inst, their_address, sync_complete_buffer, 1,
                       false);
  }

  // If we are the rightmost device, start the transmission chain.
  if (rightmost) {
    send_accumulation_and_wait();
    uint8_t sync_complete_buffer[1] = {COM_TYPE_SYNCHRONIZATION_COMPLETE};
    i2c_write_blocking(master_i2c_inst, their_address, sync_complete_buffer, 1,
                       false);
  }
}
