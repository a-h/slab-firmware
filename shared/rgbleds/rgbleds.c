#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"

PIO pio;
uint sm;
uint offset;

// put_pixel sends a single set of RGB values to the WS2812 LED strip.
static inline void put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

// urgb_u32 is a helper function to convert 3 RGB values to a single uint32_t.
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void rgbleds_update(uint8_t leds[], uint pixel_count) {
  // Update the LED strip with the new data.
  for (int i = 0; i < pixel_count; i++) {
    put_pixel(urgb_u32(leds[i * 3], leds[i * 3 + 1], leds[i * 3 + 2]));
  }
  sleep_us(50);
}

void rgbleds_init(uint gpio) {
  bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
      &ws2812_program, &pio, &sm, &offset, gpio, 1, true);
  hard_assert(success);
  ws2812_program_init(pio, sm, offset, gpio, 800000, false);
}
