#include "keymap.h"
#include "tusb.h"
#include <stdlib.h>

#include "squirrel.h"
#include "squirrel_keymap.h"
#include "squirrel_quantum.h"

void make_keys(void) {
  layers[0].keys[0] = keyboard(HID_KEY_6);
  layers[0].keys[1] = keyboard(HID_KEY_7);
  layers[0].keys[2] = keyboard(HID_KEY_8);
  layers[0].keys[3] = keyboard(HID_KEY_9);
  layers[0].keys[4] = keyboard(HID_KEY_0);
  layers[0].keys[5] = keyboard(HID_KEY_GRAVE);
  // ----
  layers[0].keys[6] = keyboard(HID_KEY_J);
  layers[0].keys[7] = keyboard(HID_KEY_F);
  layers[0].keys[8] = keyboard(HID_KEY_U);
  layers[0].keys[9] = keyboard(HID_KEY_P);
  layers[0].keys[10] = keyboard(HID_KEY_SEMICOLON);
  layers[0].keys[11] = keyboard(HID_KEY_BACKSLASH);
  // ----
  layers[0].keys[12] = keyboard(HID_KEY_Y);
  layers[0].keys[13] = keyboard(HID_KEY_N);
  layers[0].keys[14] = keyboard(HID_KEY_E);
  layers[0].keys[15] = keyboard(HID_KEY_O);
  layers[0].keys[16] = keyboard(HID_KEY_I);
  layers[0].keys[17] = keyboard(HID_KEY_APOSTROPHE);
  // ----
  layers[0].keys[18] = keyboard(HID_KEY_K);
  layers[0].keys[19] = keyboard(HID_KEY_L);
  layers[0].keys[20] = keyboard(HID_KEY_COMMA);
  layers[0].keys[21] = keyboard(HID_KEY_DECIMAL_SEPARATOR);
  layers[0].keys[22] = keyboard(HID_KEY_SLASH);
  layers[0].keys[23] = keyboard_modifier(HID_KEY_SHIFT_RIGHT);
  // ----
  layers[0].keys[24] = keyboard(HID_KEY_SPACE);
  layers[0].keys[25] = layer_momentary(1);
  layers[0].keys[26] = layer_momentary(2);
  layers[0].keys[27] = keyboard_modifier(HID_KEY_ALT_RIGHT);
  layers[0].keys[28] = keyboard(HID_KEY_GUI_RIGHT);
  layers[0].keys[29] = keyboard(HID_KEY_POWER);

  layers[0].active = true;
};
