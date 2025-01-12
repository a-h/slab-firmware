#pragma once

#include "hardware/pio.h"
#include "ws2812.pio.h"

void rgbleds_update(uint8_t leds[], uint pixel_count);
void rgbleds_init(uint gpio);
