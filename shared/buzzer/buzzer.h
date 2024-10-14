#pragma once

#include "hardware/pio.h"

void buzzer_init(int gpio, PIO pio);
void buzzer_set_frequency(int frequency);
