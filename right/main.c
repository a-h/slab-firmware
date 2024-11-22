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
#include <pico_pca9555.h>
#include <ssd1306.h>
#include <tusb.h>

// shared
#include <buzzer.h>
#include <communication.h>
#include <display.h>
#include <rgbleds.h>
#include <tinyusb_squirrel.h>
#include <tusb_config.h>
#include <usb_descriptors.h>

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
#define GPIO_EXPANDER1_INT -1
#define GPIO_EXPANDER2_INT 3
// Rotary encoder pins
#define GPIO_ROTARY_A 26
#define GPIO_ROTARY_B 27
// WS2812 LED strip data input
#define GPIO_WS2812 4

// I2C addresses for peripherals.
const uint8_t I2C1_OLED = 0x3C;
// See https://www.nxp.com/docs/en/data-sheet/PCA9555.pdf for more details.
const uint8_t I2C1_EXPANDER1 = 0b0100000;
const uint8_t I2C1_EXPANDER2 = 0b0100001;
// ADC for reading slider values.
const uint8_t I2C1_ADC = 0b1001000;

// I2C mutex - prevents multiple cores from accessing the I2C bus at the same
// time.
mutex_t i2c1_mutex;

// last_interaction is the time in milliseconds of the last interaction with the
// keyboard.
uint64_t last_interaction = 0;

// idle_timeout is the amount of time in milliseconds before the user is
// considered AFK. Set to UINT64_MAX to disable (585 million years).
uint64_t idle_timeout = 15000;

#define NUM_PIXELS 31 // 30 keys + 1 LED under the encoder.
// leds stores the R, G, and B values for each LED in the LED strip.
uint8_t leds[NUM_PIXELS * 3] = {0};

// interaction is called when any interaction with the keyboard is detected.
void interaction(void) {
  last_interaction = board_millis();
  if (tud_suspended()) {
    tud_remote_wakeup(); // Wake up the host if it is suspended.
  }
}

uint32_t lookup_expanders[30] = {
    /*-------1-------|-------0-------| expander */
    /*---1---|---0---|---1---|---0---| output set */
    /*76543210765432107654321076543210 GPIO pin */
    0b00000000000000000000000000100000, // 1
    0b00000000000000000000000001000000, // 2
    0b00000000000000000000000010000000, // 3
    0b00000000000000000000000100000000, // 4
    0b00000000000000000000001000000000, // 5
    0b00000000000000000000010000000000, // 6
    0b00000000000000000000000000000100, // 7
    0b00000000000000000000000000001000, // 8
    0b00000000000000000000000000010000, // 9
    0b00000000000000000000100000000000, // 10
    0b00000000000000000001000000000000, // 11
    0b00000000000000000010000000000000, // 12
    0b00000000100000000000000000000000, // 13
    0b00000000000000000000000000000010, // 14
    0b00000000000000000000000000000001, // 15
    0b00000000000000001000000000000000, // 16
    0b00000000000000000100000000000000, // 17
    0b00000010000000000000000000000000, // 18
    0b00000000010000000000000000000000, // 19
    0b00000000001000000000000000000000, // 20
    0b00000000000100000000000000000000, // 21
    0b00010000000000000000000000000000, // 22
    0b00001000000000000000000000000000, // 23
    0b00000100000000000000000000000000, // 24
    0b00000000000010000000000000000000, // 25
    0b00000000000001000000000000000000, // 26
    0b00000000000000100000000000000000, // 27
    0b10000000000000000000000000000000, // 28
    0b01000000000000000000000000000000, // 29
    0b00100000000000000000000000000000, // 30
};

// check_keys reads the values from all the expanders and updates the key
// states.
void check_keys(void) {
  // inputs1 and inputs2 store the values read from the two expanders.
  static uint16_t inputs1;
  static uint16_t inputs2;

  if (mutex_try_enter(&i2c1_mutex, NULL)) {
    inputs1 = pca9555_read_input(&i2c1_inst, I2C1_EXPANDER1);
    inputs2 = pca9555_read_input(&i2c1_inst, I2C1_EXPANDER2);
    mutex_exit(&i2c1_mutex);

    uint32_t inputs_combined = (inputs2 << 16) | inputs1;

    for (int i = 0; i < 30; i++) {
      if (inputs_combined & lookup_expanders[i]) {
        interaction();
      }
      check_key(i, inputs_combined & lookup_expanders[i]);
    }
  }
}

void i2c_devices_init(void) {
  // Initialize the I2C bus.
  i2c_init(&i2c1_inst, 400000); // 400kHz
  /*i2c_init(&i2c0_inst, 400000); // 400kHz*/

  // Configure the I2C pins.
  gpio_set_function(GPIO_I2C1_SDA, GPIO_FUNC_I2C);
  gpio_set_function(GPIO_I2C1_SCL, GPIO_FUNC_I2C);
  /*gpio_set_function(GPIO_I2C0_SDA, GPIO_FUNC_I2C);*/
  /*gpio_set_function(GPIO_I2C0_SCL, GPIO_FUNC_I2C);*/

  // Initialize the I2C mutex.
  mutex_init(&i2c1_mutex);

  // We don't need to lock the I2C mutex here because this function is run
  // before the multicore loop.

  // Configure the expanders with all pins as inputs.
  pca9555_configure(&i2c1_inst, I2C1_EXPANDER1, 0xFFFF);
  pca9555_configure(&i2c1_inst, I2C1_EXPANDER2, 0xFFFF);
  pca9555_input_inversion(&i2c1_inst, I2C1_EXPANDER1, 0xFFFF);
  pca9555_input_inversion(&i2c1_inst, I2C1_EXPANDER2, 0xFFFF);

  // Initialize the OLED display.
  display_init(&i2c1_inst, ROT_180, 0x3C);

  // Initialize communication with other slab devices.
  communication_init(&i2c1_inst, &i2c0_inst);
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
    communication_task(&i2c1_mutex,
                       true); // Send messages to other slab devices.
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

  squirrel_init();
  make_keys(); // Generate the defualt keymap.

  rgbleds_init(GPIO_WS2812, pio0);
  buzzer_init(GPIO_BUZZER);
  buzzer_play(0);
  stdio_uart_init_full(uart0, 115200, GPIO_UART_TX, GPIO_UART_RX);
  i2c_devices_init();

  gpio_init(17);
  gpio_set_dir(17, GPIO_OUT);

  multicore_launch_core1(core1_main);
  core0_main();
}
