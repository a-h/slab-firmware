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
#include "pico_pca9555.h"
#include "ssd1306.h"
#include "tusb.h"

// shared
#include "buzzer.h"
#include "communication.h"
#include "display.h"
#include "rgbleds.h"
#include "slab.h"
#include "tinyusb_squirrel.h"
#include "tusb_config.h"
#include "usb_descriptors.h"

// on I2C0, we are a slave
#define GPIO_I2C0_SDA 28
#define GPIO_I2C0_SCL 29
// on I2C1, we are a master
#define GPIO_I2C1_SDA 6
#define GPIO_I2C1_SCL 7
// UART for debug ports
#define GPIO_UART_TX 0
#define GPIO_UART_RX 1
// Buzzer for annoying sounds - what else?
#define GPIO_BUZZER 2
// Expander interrupt pins
#define GPIO_EXPANDER1_INT 26
#define GPIO_EXPANDER2_INT 3
// WS2812 LED strip data input
#define GPIO_WS2812 4

// I2C addresses for peripherals.
const uint8_t I2C1_OLED = 0x3C;
// See https://www.nxp.com/docs/en/data-sheet/PCA9555.pdf for more details.
const uint8_t I2C1_EXPANDER1 = 0b0100000;
const uint8_t I2C1_EXPANDER2 = 0b0100001;

// I2C mutex - prevents multiple cores from accessing the I2C bus at the same
// time.
mutex_t i2c1_mutex;

// last_interaction is the time in milliseconds of the last interaction with the
// keyboard.
uint64_t last_interaction = 0;

// idle_timeout is the amount of time in milliseconds before the user is
// considered AFK. Set to UINT64_MAX to disable (585 million years).
uint64_t idle_timeout = 3000;

#define NUM_PIXELS 29 // 20 keys + 9 leds on the side.
// leds stores the R, G, and B values for each LED in the LED strip.
uint8_t leds[NUM_PIXELS * 3] = {0};

// interaction is called when any interaction with the keyboard is detected.
void interaction(void) {
  last_interaction = board_millis();
  if (tud_suspended()) {
    tud_remote_wakeup(); // Wake up the host if it is suspended.
  }
}

// lookup_expander1 is a lookup table for the pin values of the first expander
// for each key.
uint16_t lookup_expander1[12] = {
    /*---1---|---0---|*/
    /*7654321076543210*/
    0b0000000000100000, // key 1 (index 0) - pin IO0_5
    0b0000000000010000, // key 2 (index 1) - pin IO0_4
    0b0000000100000000, // key 3 (index 2) - pin IO1_0
    0b0000001000000000, // key 4 (index 3) - pin IO1_1
    0b0000000010000000, // key 5 (index 4) - pin IO0_7
    0b0000000001000000, // key 6 (index 5) - pin IO0_6
    0b0000010000000000, // key 7 (index 6) - pin IO1_2
    0b0000100000000000, // key 8 (index 7) - pin IO1_3
    0b0000000000001000, // key 9 (index 8) - pin IO0_3
    0b0000000000000100, // key 10 (index 9) - pin IO0_2
    0b0010000000000000, // key 11 (index 10) - pin IO1_4
    0b0001000000000000, // key 12 (index 11) - pin IO1_5
};

// lookup_expander2 is a lookup table for the pin values of the second expander
// for each key.
uint16_t lookup_expander2[8] = {
    /*---1---|---0---|*/
    /*7654321076543210*/
    0b0000000010000000, // key 13 (index 12) - pin IO0_7
    0b0000000001000000, // key 14 (index 13) - pin IO0_6
    0b0000000100000000, // key 15 (index 14) - pin IO1_0
    0b0000001000000000, // key 16 (index 15) - pin IO1_1
    0b0000000000100000, // key 17 (index 16) - pin IO0_5
    0b0000000000010000, // key 18 (index 17) - pin IO0_4
    0b0000100000000000, // key 19 (index 18) - pin IO1_3
    0b0000010000000000, // key 20 (index 19) - pin IO1_2
};

// check_keys reads the values from all the expanders and updates the key
// states.
void check_keys(void) {
  static uint16_t inputs1;
  static uint16_t inputs2;
  if (mutex_try_enter(&i2c1_mutex, NULL)) {
    pca9555_read_input(&i2c1_inst, I2C1_EXPANDER1, &inputs1);
    pca9555_read_input(&i2c1_inst, I2C1_EXPANDER2, &inputs2);
    mutex_exit(&i2c1_mutex);
  };
  for (int i = 0; i < 12; i++) {
    check_key(i, inputs1 & lookup_expander1[i]);
  }
  for (int i = 0; i < 8; i++) {
    check_key(i + 12, inputs2 & lookup_expander2[i]);
  }
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

  // Initialize the I2C mutex.
  mutex_init(&i2c1_mutex);

  // We don't need to lock the I2C mutex here because this function is run
  // before the multicore loop.

  // Configure the expanders with all pins as inputs.
  pca9555_configure(&i2c1_inst, I2C1_EXPANDER1, 0xFFFF);
  pca9555_configure(&i2c1_inst, I2C1_EXPANDER2, 0xFFFF);

  // Initialize the OLED display.
  display_init(&i2c1_inst, ROT_180, 0x3C);

  // Initialize communication with other slab devices.
  communication_init(&i2c1_inst, &i2c0_inst, 0x17);
}

// Core 1 deals with the LED strip and OLED display.
void core1_main(void) {
  flash_safe_execute_core_init(); // Declare we won't use flash on core 1.
  while (true) {
    rgbleds_update(leds, NUM_PIXELS); // Update the LED strip.
    display_render(board_millis() - last_interaction > idle_timeout,
                   board_millis()); // Write the display buffer.
    display_draw(&i2c1_mutex); // Sends the display buffer to the OLED. This
                               // will hang until the I2C bus is available -
                               // usually fast enough.
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

  rgbleds_init(GPIO_WS2812, pio0);
  buzzer_init(GPIO_BUZZER, pio1);
  i2c_devices_init();

  multicore_launch_core1(core1_main);
  core0_main();
}
