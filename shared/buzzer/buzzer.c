#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

unsigned short slice_num = 0;

void buzzer_init(int gpio) {
  gpio_set_function(gpio, GPIO_FUNC_PWM);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 100.f);
  slice_num = pwm_gpio_to_slice_num(gpio);
  pwm_init(slice_num, &config, true);
}

static inline void pwm_calcDivTop(pwm_config *c, int frequency, int sysClock) {
  uint count = sysClock * 16 / frequency;
  uint div = count / 60000; // to be lower than 65535*15/16 (rounding error)
  if (div < 16)
    div = 16;
  c->div = div;
  c->top = count / div;
}

void buzzer_play(unsigned short frequency) {
  pwm_config cfg = pwm_get_default_config();
  if (frequency == 0)
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
  else {
    pwm_calcDivTop(&cfg, frequency, 125000000);
    pwm_init(slice_num, &cfg, true);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, cfg.top / 2);
  }
}
