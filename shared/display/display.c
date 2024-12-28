#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <hardware/i2c.h>

#include <squirrel_quantum.h>
#include <ssd1306.h>

#include "display.h"

ssd1306_t display;

bool screensaver;
bool central;
bool leftmost;
bool rightmost;

void display_init(i2c_inst_t *i2c_inst, ssd1306_rotation_t rotation,
                  uint8_t addr) {
  ssd1306_init(&display, 128, 32, addr, i2c_inst);
  ssd1306_set_rotation(&display, rotation);
  leftmost = false;
  rightmost = false;
  screensaver = false;
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
  // Board position indicator
  if (leftmost) {
    ssd1306_draw_string(&display, 50, 20, 1, "L");
  } else if (rightmost) {
    ssd1306_draw_string(&display, 50, 20, 1, "R");
  } else if (central) {
    ssd1306_draw_string(&display, 50, 20, 1, "C");
  } else {
    ssd1306_draw_string(&display, 50, 20, 1, "U");
  }
}

bool was_screensaver = false;
bool screensaver = false;

void display_render(uint32_t millis) {
  if (screensaver && !was_screensaver) {
    ssd1306_clear(&display);
    ssd1306_poweroff(&display);
    was_screensaver = screensaver;
  }
  if (!screensaver && was_screensaver) {
    ssd1306_poweron(&display);
    was_screensaver = screensaver;
  }
  if (!screensaver) {
    draw_homescreen(millis / 10);
  }
}

inline void display_draw() { ssd1306_show(&display); }
