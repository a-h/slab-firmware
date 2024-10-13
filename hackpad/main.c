#include "bsp/board.h"
#include "hardware/flash.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "keymap.h"
#include "pico/flash.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico_pca9555.h"
#include "ssd1306.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "ws2812.pio.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "squirrel.h"
#include "squirrel_consumer.h"
#include "squirrel_init.h"
#include "squirrel_key.h"
#include "squirrel_keyboard.h"
#include "squirrel_quantum.h"

#include "tinyusb_squirrel.h"

#include "slab.h"

// I2C mutex
mutex_t i2c_mutex;

// last_interaction is the time in ms of the last interaction with the keyboard.
uint64_t last_interaction = 0;

// idle_timeout is the amount of time in ms before the user is considered AFK.
// Set to UINT64_MAX to (effictivly) disable timout. (585 million years).
uint64_t idle_timeout = 3000;

// RGB LEDs
#define NUM_PIXELS 29 // 20 keys + 9 leds on the side.
#define WS2812_GPIO 26

// leds stores the R, G, and B values as uint8s for each pixel.
uint8_t leds[NUM_PIXELS * 3] = {0};

// put_pixel sends a single set of RGB values to the WS2812 LED strip.
static inline void put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// urgb_u32 is a helper function to convert 3 RGB values to a single uint32_t.
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

uint8_t temp_leds[NUM_PIXELS * 3] = {0};
uint8_t led_mode = 0;
uint16_t led_data_index = 0;

#define FLASH_TARGET_OFFSET (256 * 1024)
const uint8_t *flash_target = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

// flash_led_state saves the current LED state to flash so it can be read on
// boot.
void flash_led_state(void *var) {
  flash_range_erase(FLASH_TARGET_OFFSET,
                    512); // Erase 512 bytes of flash memory starting at 0.
  flash_range_program(
      FLASH_TARGET_OFFSET, leds,
      NUM_PIXELS * 3); // Write the current LED state to flash. - 270 bytes
  // 242 bytes left
}

void load_led_state(void *var) { memcpy(leds, flash_target, NUM_PIXELS * 3); }

// GET_REPORT callback (host requests data from device).
// Currently unused.
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  return 0;
}
// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  // report_id was the first byte of the buffer, report_id was removed from the
  // buffer before this function was called.

  if (bufsize == 1) {
    if (report_id == 0b00000000) { // Update LED strip with data
      memcpy(leds, temp_leds, NUM_PIXELS * 3);
      led_data_index = 0;
      return;
    }
    if (report_id == 0b00000001) { // Save the current LED state to flash
      flash_safe_execute(&flash_led_state, NULL, 5);
      return;
    }
    if (report_id == 0b00000010) { // Save the current mode state to flash
      return;
    }
    if (report_id == 0b00000011) { // Update the current mode state
      led_mode = buffer[0];
      return;
    }
  }
  if (bufsize == 60) {
    // Run length decode.
    for (uint8_t i = 0; i < 60; i += 4) {
      uint8_t length = buffer[i];
      if (length == 0) {
        break;
      }
      uint8_t r = buffer[i + 1];
      uint8_t g = buffer[i + 2];
      uint8_t b = buffer[i + 3];
      for (uint8_t j = 0; j < length; j++) {
        temp_leds[led_data_index] = r;
        temp_leds[led_data_index + 1] = g;
        temp_leds[led_data_index + 2] = b;
        led_data_index += 3;
      }
    }
  }

  // Recived data from the host.
  if (report_type == HID_REPORT_TYPE_OUTPUT) {
    // Keyboard type report
    if (report_id == REPORT_ID_KEYBOARD) {
      // Set keyboard LED e.g Capslock, Numlock etc...
      // bufsize should be (at least) 1
      if (bufsize < 1) {
        return;
      }

      uint8_t const kbd_leds = buffer[0];
      if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
        /*put_pixel(urgb_u32(255, 0, 0));*/
      } else if (kbd_leds & KEYBOARD_LED_NUMLOCK) {
        /*put_pixel(urgb_u32(0, 255, 0));*/
      } else if (kbd_leds & KEYBOARD_LED_SCROLLLOCK) {
        /*put_pixel(urgb_u32(0, 0, 255));*/
      } else {
        /*put_pixel(0);*/
      }
    }
  }
}

// 3 address pins are grounded.
// See https://www.nxp.com/docs/en/data-sheet/PCA9555.pdf for more details.
const uint8_t PCA9555_ADDR = 0b0100000;

// outputs_lookup is a lookup table that provides the correct pin outputs from
// the PCA9555 chip for each column of the keyboard.
const uint16_t outputs_lookup[16] = {
    0b0000000010000000, // physical column 1
    0b0000000001000000, // 2
    0b0000000000100000, // 3
    0b0000000000010000, // 4
    0b0000000000000001, // 5
    0b0000000000000010, // 6
    0b0000000000000100, // 7
    0b0000000000001000, // 8
    0b0100000000000000, // 9
    0b0010000000000000, // 10
    0b0001000000000000, // 11
    0b0000100000000000, // 12
    0b0000010000000000, // 13
    0b0000001000000000, // 14
    0b0000000100000000, // 15
    0b1000000000000000, // unused - not connected
};

// interaction is called when any interaction with the keyboard is detected.
void interaction(void) {
  last_interaction = board_millis();
  if (tud_suspended()) {
    tud_remote_wakeup();
  }
}

// debounce checks keys twice over 200us and if the key is still in the same
// position, it counts is as confirmed and runs check_key. This prevents
// chatter. (false key activations)
void debounce(uint8_t column) {
  // Get the state of all keys in the column
  bool r1 = gpio_get(1);
  bool r2 = gpio_get(0);
  bool r3 = gpio_get(29);
  bool r4 = gpio_get(28);
  bool r5 = gpio_get(27);

  bool r1_prev = r1;
  bool r2_prev = r2;
  bool r3_prev = r3;
  bool r4_prev = r4;
  bool r5_prev = r5;

  // Wait for 200us
  sleep_us(200);

  r1 = gpio_get(1);
  r2 = gpio_get(0);
  r3 = gpio_get(29);
  r4 = gpio_get(28);
  r5 = gpio_get(27);

  // If the key is still in the same state after 20ms, run check_key.
  // Also, if any key is pressed, update the last_interaction time.
  if (r1 == r1_prev) {
    check_key(column, r1);
    if (r1) {
      interaction();
    }
  }
  if (r2 == r2_prev) {
    check_key(column + 15, r2);
    if (r2) {
      interaction();
    }
  }
  if (r3 == r3_prev) {
    check_key(column + 30, r3);
    if (r3) {
      interaction();
    }
  }
  if (r4 == r4_prev) {
    check_key(column + 45, r4);
    if (r4) {
      interaction();
    }
  }
  if (r5 == r5_prev) {
    check_key(column + 60, r5);
    if (r5) {
      interaction();
    }
  }
}

// check_keys loops through all columns and runs a check on each key.
void check_keys() {
  // PCA9555 uses two sets of 8-bit outputs
  // Loop through all columns
  if (mutex_try_enter(&i2c_mutex, NULL)) {
    for (uint8_t x = 0; x < 15; x++) {
      uint16_t column_outputs = outputs_lookup[x];
      pca9555_output(&i2c1_inst, PCA9555_ADDR, column_outputs);
      debounce(x);
    }
    mutex_exit(&i2c_mutex);
  };
}

void row_setup(void) {
  // Configure the row pins as inputs with pull-down resistors.
  gpio_init(1);
  gpio_set_dir(1, GPIO_IN);
  gpio_pull_down(1);

  gpio_init(0);
  gpio_set_dir(0, GPIO_IN);
  gpio_pull_down(0);

  gpio_init(29);
  gpio_set_dir(29, GPIO_IN);
  gpio_pull_down(29);

  gpio_init(28);
  gpio_set_dir(28, GPIO_IN);
  gpio_pull_down(28);

  gpio_init(27);
  gpio_set_dir(27, GPIO_IN);
  gpio_pull_down(27);
}

// LED strip

PIO led_pio = pio0;

void led_init(void) {
  uint led_pio_offset = pio_add_program(led_pio, &ws2812_program);
  uint led_sm = pio_claim_unused_sm(led_pio, true);
  ws2812_program_init(led_pio, led_sm, led_pio_offset, WS2812_GPIO, 800000,
                      false);
}

void led_task(void) {
  // Update the LED strip with the new data.
  for (int i = 0; i < NUM_PIXELS; i++) {
    put_pixel(urgb_u32(leds[i * 3], leds[i * 3 + 1], leds[i * 3 + 2]));
  }
  sleep_us(50);
}

// I2C Display
ssd1306_t display;

#define SCREENSAVER_LENGTH 14 // Length of the screensaver text.
char screensaver_text[SCREENSAVER_LENGTH] = {'S', 'L', 'A', 'B', ' ', 'K', 'E',
                                             'Y', 'B', 'O', 'A', 'R', 'D', ' '};

void draw_screensaver(int frame) {
  // write the screensaver text waving across the screen
  for (uint8_t x = 0; x < SCREENSAVER_LENGTH; x++) {
    int y = 2 * sin(-frame / 10.0 + x * 2) + 8;
    if (y < 0) {
      y = 0;
    }
    if (y > 32) {
      y = 32;
    }
    int letter_x = x * 22 + (-frame % (27 * SCREENSAVER_LENGTH)) + (22 * 3);
    if (letter_x < 0) {
      letter_x += (27 * SCREENSAVER_LENGTH);
    }
    if (letter_x > (27 * SCREENSAVER_LENGTH)) {
      letter_x -= (27 * SCREENSAVER_LENGTH);
    }
    ssd1306_draw_char(&display, letter_x, y, 3, screensaver_text[x]);
    ssd1306_draw_char(&display, letter_x - (27 * SCREENSAVER_LENGTH), y, 3,
                      screensaver_text[x]);
  }
}

void draw_homescreen(int frame) {
  // Layer number display
  ssd1306_draw_empty_square(&display, 2, 2, 27, 28);
  char layer_number[2];
  uint8_t current_layer = 0;
  for (current_layer = 16; current_layer > 0; current_layer--) { // 15-0
    if (!layers[current_layer - 1].active) {
      continue;
    }
    sprintf(layer_number, "%d", current_layer - 1);
    break;
  }
  if (current_layer >= 10) {
    ssd1306_draw_string(&display, 4, 10, 2, layer_number);
  } else {
    ssd1306_draw_string(&display, 8, 6, 3, layer_number);
  };

  char ms[10];
  sprintf(ms, "%d", last_interaction);
  ssd1306_draw_string(&display, 30, 2, 1, ms);

  char mod[10];
  sprintf(mod, "%d", keyboard_get_modifiers());
  ssd1306_draw_string(&display, 30, 12, 1, mod);
}

void display_task(void) {
  ssd1306_clear(&display);

  if (board_millis() - last_interaction > idle_timeout) {
    draw_screensaver(board_millis() / 10);
  } else {
    draw_homescreen(board_millis() / 10);
  }

  // Update the display.
  mutex_enter_blocking(&i2c_mutex);
  ssd1306_show(&display);
  mutex_exit(&i2c_mutex);
}

void i2c_devices_init(void) {
  // Initialize the I2C bus.
  i2c_init(&i2c1_inst, 400000); // 400kHz

  // Configure the I2C pins.
  gpio_set_function(6, GPIO_FUNC_I2C);
  gpio_set_function(7, GPIO_FUNC_I2C);

  // Initialize the I2C mutex.
  mutex_init(&i2c_mutex);

  // We don't need to lock the I2C mutex here because this function is run
  // before the multicore loop.
  pca9555_configure(&i2c1_inst, PCA9555_ADDR, 0x0000);
  ssd1306_init(&display, 128, 32, 0x3C, i2c1);
  ssd1306_set_rotation(&display, ROT_180);
}

// Core 1 deals with the LED strip and OLED display.
void core1_main() {
  flash_safe_execute_core_init();
  while (true) {
    led_task();
    display_task();
  }
}

// Core 0 deals with keyboard and USB HID.
void core0_main() {
  while (true) {
    check_keys(); // Check the keys on the keyboard for their states.
    tud_task();   // tinyusb device task.
    hid_task();   // Send HID reports to the host.
  }
}

// The main function, runs initialization.
int main(void) {
  board_init();               // Initialize the pico board
  tud_init(BOARD_TUD_RHPORT); // Initialize the tinyusb device stack
  tusb_init();                // Initialize tinyusb
  squirrel_init();            // Initialize the squirrel keyboard with 75 keys.

  make_keys();        // Initialize the keys on the keyboard
  led_init();         // Initialize the WS2812 LED strip
  i2c_devices_init(); // Initialize the I2C devices

  // Load the defualt LED state from flash.
  load_led_state(NULL);

  // Core 1 loop
  multicore_launch_core1(core1_main);
  // Core 0 loop
  core0_main();
}
