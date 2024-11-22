#include <hardware/i2c.h>
#include <pico/mutex.h>
#include <stdint.h>

i2c_inst_t *slider_inst;
uint8_t slider_address;

uint16_t slider_value = 0;

void slider_init(i2c_inst_t *i2c, uint8_t address) {
  slider_inst = i2c;
  slider_address = address;
}

// ADS1110A0IDBVR (tokmas)
uint8_t i2c_configuration_buffer[1] = {
    // function of bits, in left-to-right order:
    // 1. I want to read a sample
    // 2. X
    // 3. X
    // 4. Please sample only when I ask
    // 5/6. two bit pair - represents data rate + resolution:
    // 5/6. 00 is 12-bit, 01 is 14-bit, 10 is 15-bit, 11 is 16-bit.
    // 7/8. two bit pair - gain setting:
    // 7/8. higher number is more gain
    0b10011100,
};
uint8_t i2c_recv_buffer[2];

void slider_task(mutex_t *i2c_mutex) {
  mutex_enter_blocking(i2c_mutex);
  i2c_write_blocking(slider_inst, slider_address, i2c_configuration_buffer, 1,
                     false);
  i2c_read_blocking(slider_inst, slider_address, i2c_recv_buffer, 2, false);
  mutex_exit(i2c_mutex);
  slider_value = (i2c_recv_buffer[0] << 8) | i2c_recv_buffer[1];
}
