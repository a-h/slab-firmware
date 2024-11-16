#pragma once

#include "hardware/pio.h"

void buzzer_init(int gpio);
void buzzer_play(unsigned short frequency);
