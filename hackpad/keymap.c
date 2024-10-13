#include "keymap.h"
#include "tusb.h"
#include <stdlib.h>

#include "squirrel.h"
#include "squirrel_keymap.h"
#include "squirrel_quantum.h"

void make_keys(void) {
  layers[0].keys[0] = keyboard(HID_KEY_H);
  layers[0].keys[1] = keyboard(HID_KEY_A);
  layers[0].keys[2] = keyboard(HID_KEY_C);
  layers[0].keys[3] = keyboard(HID_KEY_K);
  layers[0].keys[4] = keyboard(HID_KEY_C);
  layers[0].keys[5] = keyboard(HID_KEY_L);
  layers[0].keys[6] = keyboard(HID_KEY_U);
  layers[0].keys[7] = keyboard(HID_KEY_B);
  layers[0].keys[8] = keyboard(HID_KEY_2);
  layers[0].keys[9] = keyboard(HID_KEY_0);
  layers[0].keys[10] = keyboard(HID_KEY_2);
  layers[0].keys[11] = keyboard(HID_KEY_4);
  layers[0].keys[12] = consumer(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
  layers[0].keys[13] = consumer(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
  layers[0].keys[14] = consumer(HID_USAGE_CONSUMER_MUTE);
  layers[0].keys[15] = consumer(HID_USAGE_CONSUMER_PLAY_PAUSE);
  layers[0].keys[16] = consumer(HID_USAGE_CONSUMER_SCAN_NEXT);
  layers[0].keys[17] = consumer(HID_USAGE_CONSUMER_SCAN_PREVIOUS);
  layers[0].keys[18] = consumer(HID_USAGE_CONSUMER_STOP);
  layers[0].keys[19] = consumer(HID_USAGE_CONSUMER_SLEEP);
};
