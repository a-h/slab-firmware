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
#include <tusb.h>

#include "communication.h"

uint8_t address = 0b00010111;
i2c_inst_t *slave_i2c_inst;
i2c_inst_t *master_i2c_inst;

uint8_t squirrel_recv_buf[9];
uint8_t squirrel_recv_buf_index = 0;

uint8_t com_type =
    0; // If 0, we need to read a byte to determine the new type of data.

// true is master gives slave data, false is master requests data from slave.
bool master_tells_slave = true;

uint8_t squirrel_request_buf[9];
uint8_t squirrel_request_buf_index = 0;

bool starter = true;

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
    case COM_TYPE_START:
      printf("S");
      starter = false;
      break;
    default:
      printf("U");
      break;
    }
    break;
  case I2C_SLAVE_REQUEST: // master is requesting data
    if (!master_tells_slave) {
      printf("M");
      i2c_write_byte_raw(i2c, squirrel_request_buf[squirrel_request_buf_index]);
      squirrel_request_buf_index++;
      break;
    }

    // if we have usb, send 1.
    if (tud_connected()) {
      printf("C");
      i2c_write_byte_raw(i2c, 1);
      break;
    }
    // if we don't have usb,
    //     - send a usbsearch to the next board in the chain.
    //     - if we get a 1, send 1.
    //     - if we get a 0, send 0.
    //     - if we timeout, send 0 - we are the last board in the chain.
    uint8_t *has_USB_buf = (uint8_t *)malloc(1);
    has_USB_buf[0] = 8;
    printf("&");
    i2c_read_blocking_until(slave_i2c_inst, address, has_USB_buf, 1, false,
                            500);
    if (has_USB_buf[0] == 1) {
      printf("1");
      i2c_write_byte_raw(i2c, 1);
    } else {
      printf("0");
      i2c_write_byte_raw(i2c, 0);
    }
    free(has_USB_buf);
    has_USB_buf = NULL;
    break;
  case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
    squirrel_recv_buf_index = 0;
    squirrel_request_buf_index = 0;
    com_type = 0;
    if (master_tells_slave) {
      process_packet(&squirrel_recv_buf);
      memset(squirrel_recv_buf, 0, 9);
    } else {
      process_packet(&squirrel_request_buf);
      memset(squirrel_request_buf, 0, 9);
    }
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
  sleep_ms(100); // Allow all boards to start up before sending data.
  uint8_t start_buf[1] = {COM_TYPE_START};
  printf("/");
  i2c_write_blocking_until(slave_i2c_inst, address, start_buf, 1, false, 500);
  if (starter) {
    printf("!");
    uint8_t *has_USB_buf = (uint8_t *)malloc(1);
    has_USB_buf[0] = 8;
    i2c_read_blocking_until(slave_i2c_inst, address, has_USB_buf, 1, false,
                            500);
    if (has_USB_buf[0] == 1) {
      printf("R");
      master_tells_slave = true;
    } else {
      printf("L");
      master_tells_slave = false;
    }
    free(has_USB_buf);
    has_USB_buf = NULL;
  };
};

uint8_t squirrel_send_buf[9] = {0};
uint8_t i2c_send_buf[10] = {COM_TYPE_SQUIRREL};

void communication_task(mutex_t *i2c_mutex) {
  get_packet(&squirrel_send_buf);
  memcpy(i2c_send_buf + 1, squirrel_send_buf, 9);

  /*mutex_enter_blocking(i2c_mutex);*/
  if (master_tells_slave) {
    // If USB is to the right
    i2c_write_blocking(master_i2c_inst, address, i2c_send_buf, 10, false);
  } else {
    // If USB is to the left
    i2c_read_blocking(master_i2c_inst, address, i2c_send_buf, 10, false);
  }
  /*mutex_exit(i2c_mutex);*/
}
