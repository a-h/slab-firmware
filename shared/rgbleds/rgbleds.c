#include "bsp/board.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"
#include <math.h>

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

uint32_t hsv2rgb(float H, float S, float V) {
  float r, g, b;

  float h = H / 360;
  float s = S / 100;
  float v = V / 100;

  int i = floor(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (i % 6) {
  case 0:
    r = v, g = t, b = p;
    break;
  case 1:
    r = q, g = v, b = p;
    break;
  case 2:
    r = p, g = v, b = t;
    break;
  case 3:
    r = p, g = q, b = v;
    break;
  case 4:
    r = t, g = p, b = v;
    break;
  case 5:
    r = v, g = p, b = q;
    break;
  }

  return urgb_u32(r * 255, g * 255, b * 255);
}

const uint32_t rgbleds_interval_ms = 5;
void rgbleds_update(uint8_t *leds, uint pixel_count) {
  static uint32_t next_render_ms = 0; // Time of next report
  if (board_millis() - next_render_ms < rgbleds_interval_ms) {
    return;
  };
  next_render_ms += rgbleds_interval_ms;

  // Update the LED strip with the new data.
  for (int i = 0; i < pixel_count; i++) {
    put_pixel(hsv2rgb((i * 7 + board_millis() / 8) % 360, 100, 100));
  }
}

void rgbleds_init(uint gpio) {
  bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
      &ws2812_program, &pio, &sm, &offset, gpio, 1, true);
  hard_assert(success);
  ws2812_program_init(pio, sm, offset, gpio, 800000, false);
}
