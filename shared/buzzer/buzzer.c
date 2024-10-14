#include "hardware/pio.h"
#include "pwm.pio.h"

PIO buzzer_pio;
uint buzzer_sm;

void buzzer_init(int gpio, PIO pio) {
  uint offset = pio_add_program(pio, &pwm_program);
  buzzer_sm = pio_claim_unused_sm(pio, true);
  pwm_program_init(pio, buzzer_sm, offset, gpio);
}

void buzzer_set_frequency(int frequency) {
  pio_sm_set_enabled(buzzer_pio, buzzer_sm, false);
  pio_sm_put_blocking(buzzer_pio, buzzer_sm, frequency);
  pio_sm_exec(buzzer_pio, buzzer_sm, pio_encode_pull(false, false));
  pio_sm_exec(buzzer_pio, buzzer_sm, pio_encode_out(pio_isr, 32));
  pio_sm_set_enabled(buzzer_pio, buzzer_sm, true);
}
