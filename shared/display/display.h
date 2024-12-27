#pragma once

#include "ssd1306.h"

extern ssd1306_t display;

void display_init(i2c_inst_t *i2c_inst, ssd1306_rotation_t rotation,
                  uint8_t addr);

void display_render(bool screensaver, uint64_t millis);
void display_draw();
