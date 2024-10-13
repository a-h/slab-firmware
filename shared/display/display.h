#pragma once

#include "pico/mutex.h"
#include "ssd1306.h"

extern ssd1306_t display;

void display_init(i2c_inst_t *i2c_inst, rotation_t rotation, uint8_t addr);

void draw_screensaver(int frame);
void draw_homescreen(int frame);

void display_render(bool screensaver, uint64_t millis);
void display_draw(mutex_t *i2c_mutex);
