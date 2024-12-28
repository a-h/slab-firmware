#pragma once

#include <stdbool.h>
#include "ssd1306.h"

extern ssd1306_t display;

void display_init(i2c_inst_t *i2c_inst, ssd1306_rotation_t rotation,
                  uint8_t addr);

// Referenced by communcation.c
extern bool screensaver;
extern bool leftmost;
extern bool rightmost;
extern bool central;

void display_render(uint32_t millis);
void display_draw();
