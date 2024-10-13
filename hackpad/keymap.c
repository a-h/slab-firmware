#include "keymap.h"
#include "tusb.h"
#include <stdlib.h>

#include "squirrel.h"
#include "squirrel_keymap.h"
#include "squirrel_quantum.h"

void make_keys(void) {
  layers[0].keys[0] = keyboard(HID_KEY_A);
  layers[0].keys[1] = keyboard(HID_KEY_B);
};
