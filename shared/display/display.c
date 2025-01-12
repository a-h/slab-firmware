#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <hardware/i2c.h>

#include <squirrel_quantum.h>
#include <squirrel_split.h>
#include <ssd1306.h>

#include "display.h"

ssd1306_t display;

bool was_screensaver = false;

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

char old_layer_number[2] = "0";
char old_position_indicator = 'U';

// If force is true, the display will be redrawn regardless of whether the
// contents have changed
bool draw_homescreen(int frame, bool force) {
  bool display_changed = false;

  // Create layer number display
  char layer_number[2];
  uint8_t current_layer = 0;
  for (current_layer = 16; current_layer > 0; current_layer--) { // 15-0
    if (!layers[current_layer - 1].active &&
        !remote_layers[current_layer - 1]) {
      continue;
    }
    sprintf(layer_number, "%d", current_layer - 1);
    break;
  }

  // Check if layer number has changed
  if (strcmp(old_layer_number, layer_number) != 0) {
    strcpy(old_layer_number, layer_number);
    display_changed = true;
  };

  // Board position indicator
  char position_indicator[1];
  if (leftmost) {
    position_indicator[0] = 'L';
  } else if (rightmost) {
    position_indicator[0] = 'R';
  } else if (central) {
    position_indicator[0] = 'C';
  } else {
    position_indicator[0] = 'U';
  }

  if (old_position_indicator != position_indicator[0]) {
    old_position_indicator = position_indicator[0];
    display_changed = true;
  }

  if (display_changed || force) {
    ssd1306_clear(&display);

    ssd1306_draw_empty_square(&display, 2, 2, 27, 27);
    if (current_layer >= 10) {
      ssd1306_draw_string(&display, 4, 10, 2, layer_number);
    } else {
      ssd1306_draw_string(&display, 8, 6, 3, layer_number);
    };

    if (display.rotation == ROT_0 || display.rotation == ROT_180) {
    } else {
      ssd1306_draw_string(&display, 0, 32, 1, position_indicator);
    }

    return true;
  }
  return false;
}

bool display_render(uint32_t millis) {
  if (screensaver && !was_screensaver) {
    ssd1306_poweroff(&display);
    was_screensaver = screensaver;
    return false;
  }
  if (!screensaver) {
    if (was_screensaver) {
      ssd1306_poweron(&display);
      was_screensaver = screensaver;
    }
    return draw_homescreen(millis / 10, false);
  }
  return false;
}

inline void display_update() { ssd1306_show(&display); }
