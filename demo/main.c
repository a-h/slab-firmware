// pico SDK
#include "hardware/flash.h"
#include "hardware/pio.h"
#include "pico/flash.h"
#include "pico/multicore.h"

// squirrel
#include "squirrel_init.h"
#include "squirrel_quantum.h"

// keymap
#include "keymap.h"

// libraries
#include "tusb.h"

// shared
#include "communication.h"
#include "slab.h"
#include "tinyusb_squirrel.h"
#include "tusb_config.h"
#include "usb_descriptors.h"

#include "pico/stdio_uart.h"

// UART for debug ports
#define GPIO_UART_TX 12
#define GPIO_UART_RX 13

// on I2C0, we are a master
#define GPIO_I2C0_SDA 0
#define GPIO_I2C0_SCL 1
// on I2C1, we are a slave
#define GPIO_I2C1_SDA 2
#define GPIO_I2C1_SCL 3

// I2C mutex - prevents multiple cores from accessing the I2C bus at the same
// time.
mutex_t i2c1_mutex;

// last_interaction is the time in milliseconds of the last interaction with the
// keyboard.
uint64_t last_interaction = 0;

// idle_timeout is the amount of time in milliseconds before the user is
// considered AFK. Set to UINT64_MAX to disable (585 million years).
uint64_t idle_timeout = 3000;

// interaction is called when any interaction with the keyboard is detected.
void interaction(void) {
  last_interaction = board_millis();
  if (tud_suspended()) {
    tud_remote_wakeup(); // Wake up the host if it is suspended.
  }
}

// check_keys reads the values from all the expanders and updates the key
// states.
void check_keys(void) {
  if (keyboard_keycodes[HID_KEY_A]) {
    gpio_put(17, 1);
  } else {
    gpio_put(17, 0);
  }
  check_key(0, gpio_get(16) == 0);
}

void i2c_devices_init(void) {
  // Initialize the I2C bus.
  i2c_init(&i2c1_inst, 400000); // 400kHz
  i2c_init(&i2c0_inst, 400000); // 400kHz

  // Configure the I2C pins.
  gpio_set_function(GPIO_I2C1_SDA, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C1_SCL, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C0_SDA, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C0_SCL, GPIO_FUNC_I2C);

  gpio_pull_up(GPIO_I2C1_SDA);
  gpio_pull_up(GPIO_I2C1_SCL);
  gpio_pull_up(GPIO_I2C0_SDA);
  gpio_pull_up(GPIO_I2C0_SCL);

  // Initialize the I2C mutex.
  mutex_init(&i2c1_mutex);

  // We don't need to lock the I2C mutex here because this function is run
  // before the multicore loop.

  // Initialize communication with other slab devices.
  communication_init(&i2c0_inst, &i2c1_inst, 0x17);
}

// Core 1 deals with the LED strip and OLED display.
void core1_main(void) {
  flash_safe_execute_core_init(); // Declare we won't use flash on core 1.
  while (true) {
  }
}

// Core 0 deals with keyboard and USB HID.
void core0_main(void) {
  while (true) {
    check_keys(); // Check the keys on the keyboard for their states.
    tud_task();   // TinyUSB task.
    hid_task();   // Send HID reports to the host.
    communication_task(&i2c1_mutex); // Send messages to other slab devices.
  }
}

// The main function, runs initialization.
int main(void) {
  // TinyUSB initialization
  board_init();
  tud_init(BOARD_TUD_RHPORT);
  tusb_init();

  // SQUIRREL initialization
  squirrel_init();

  make_keys(); // Generate the defualt keymap.

  gpio_init(16);
  gpio_set_dir(16, GPIO_IN);
  gpio_pull_up(16);

  gpio_init(17);
  gpio_set_dir(17, GPIO_OUT);

  gpio_init(GPIO_UART_TX);
  gpio_set_function(GPIO_UART_TX, GPIO_FUNC_UART);

  gpio_init(GPIO_UART_RX);
  gpio_set_function(GPIO_UART_RX, GPIO_FUNC_UART);

  uart_init(uart0, 115200);

  i2c_devices_init();

  multicore_launch_core1(core1_main);
  core0_main();
}
