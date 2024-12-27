#include <math.h>
#include <stdio.h>

#include <hardware/i2c.h>

#include <squirrel_quantum.h>
#include <ssd1306.h>

#include "display.h"

ssd1306_t display;

void display_init(i2c_inst_t *i2c_inst, ssd1306_rotation_t rotation,
                  uint8_t addr) {
  ssd1306_init(&display, 128, 32, addr, i2c_inst);
  ssd1306_set_rotation(&display, rotation);
}

#define SCREENSAVER_LENGTH 14 // Length of the screensaver text.
char screensaver_text[SCREENSAVER_LENGTH] = {'S', 'L', 'A', 'B', ' ', 'K', 'E',
                                             'Y', 'B', 'O', 'A', 'R', 'D', ' '};

void draw_screensaver(int frame) {
  // write the screensaver text waving across the screen
  ssd1306_clear(&display);
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
  ssd1306_clear(&display);
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
}

void display_render(bool screensaver, uint64_t millis) {
  if (screensaver) {
    draw_screensaver(millis / 10);
  } else {
    draw_homescreen(millis / 10);
  }
}

inline void display_draw() { ssd1306_show(&display); }
