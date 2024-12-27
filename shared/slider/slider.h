#pragma once
#include <hardware/i2c.h>

extern uint16_t slider_value;

void slider_init(i2c_inst_t *i2c, uint8_t address);
void slider_task();
