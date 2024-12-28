// pico SDK
#include <hardware/flash.h>
#include <hardware/pio.h>
#include <pico/flash.h>
#include <pico/multicore.h>

// squirrel
#include <squirrel_init.h>
#include <squirrel_quantum.h>

// keymap
#include "keymap.h"

// libraries
#include <tusb.h>

// shared
#include <communication.h>
#include <tinyusb_squirrel.h>
#include <tusb_config.h>
#include <usb_descriptors.h>

// on I2C0, we are a slave
#define GPIO_I2C0_SDA 28
#define GPIO_I2C0_SCL 29
// on I2C1, we are a master
#define GPIO_I2C1_SDA 6
#define GPIO_I2C1_SCL 7

// last_interaction is the time in milliseconds of the last interaction with the
// keyboard.
uint32_t last_interaction = 0;

// idle_timeout is the amount of time in milliseconds before the user is
// considered AFK. Set to UINT64_MAX to disable (585 million years).
uint32_t idle_timeout = 3000; // 3 seconds

// interaction is called when any interaction with the keyboard is detected.
void interaction(void) {
  last_interaction = board_millis();
  if (tud_suspended()) {
    tud_remote_wakeup(); // Wake up the host if it is suspended.
  }
}

#define GPIO_KEY0 0

// check_keys reads the values from all the expanders and updates the key
// states.
void check_keys(void) { check_key(0, gpio_get(GPIO_KEY0)); }

void i2c_devices_init(void) {
  // Initialize the I2C bus.
  i2c_init(&i2c1_inst, 100000); // 400kHz

  // Configure the I2C pins.
  gpio_set_function(GPIO_I2C1_SDA, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C1_SCL, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C0_SDA, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C0_SCL, GPIO_FUNC_I2C);

  gpio_pull_up(GPIO_I2C1_SDA);
  gpio_pull_up(GPIO_I2C1_SCL);

  // Initialize communication with other slab devices.
  communication_init(&i2c1_inst, &i2c0_inst);
}

// Core 1 deals with the LED strip and OLED display.
void core1_main(void) {
  flash_safe_execute_core_init(); // Declare we won't use flash on core 1.
  while (true) {
    tud_task(); // TinyUSB task.
    hid_task(); // Send HID reports to the host.
  }
}

// Core 0 deals with keyboard and USB HID.
void core0_main(void) {
  while (true) {
    check_keys(); // Check the keys on the keyboard for their states.
    communication_task(tud_ready(),
                       board_millis() - last_interaction > idle_timeout,
                       board_millis()); // Send messages to other slab devices.
  }
}

// The main function, runs initialization.
int main(void) {
  // TinyUSB initialization
  board_init();
  tud_init(BOARD_TUD_RHPORT);
  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  squirrel_init(1);
  make_keys(); // Generate the defualt keymap.

  i2c_devices_init();

  gpio_init(17);
  gpio_set_dir(17, GPIO_OUT);
  gpio_init(16);
  gpio_set_dir(16, GPIO_OUT);
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);

  multicore_launch_core1(core1_main);
  core0_main();
}
