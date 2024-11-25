#include "keymap.h"
#include "tusb.h"
#include <stdlib.h>

#include <squirrel.h>
#include <squirrel_keymap.h>
#include <squirrel_quantum.h>

void make_keys(void) {
  layers[0].keys[0] = keyboard(HID_KEY_ESCAPE);
  layers[0].keys[1] = keyboard(HID_KEY_1);
  layers[0].keys[2] = keyboard(HID_KEY_2);
  layers[0].keys[3] = keyboard(HID_KEY_3);
  layers[0].keys[4] = keyboard(HID_KEY_4);
  layers[0].keys[5] = keyboard(HID_KEY_5);
  // ----
  layers[0].keys[6] = keyboard(HID_KEY_TAB);
  layers[0].keys[7] = keyboard(HID_KEY_Q);
  layers[0].keys[8] = keyboard(HID_KEY_D);
  layers[0].keys[9] = keyboard(HID_KEY_R);
  layers[0].keys[10] = keyboard(HID_KEY_W);
  layers[0].keys[11] = keyboard(HID_KEY_B);
  // ----
  layers[0].keys[12] = keyboard(HID_KEY_BACKSPACE);
  layers[0].keys[13] = keyboard(HID_KEY_A);
  layers[0].keys[14] = keyboard(HID_KEY_S);
  layers[0].keys[15] = keyboard(HID_KEY_H);
  layers[0].keys[16] = keyboard(HID_KEY_T);
  layers[0].keys[17] = keyboard(HID_KEY_G);
  // ----
  layers[0].keys[18] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTSHIFT);
  layers[0].keys[19] = keyboard(HID_KEY_Z);
  layers[0].keys[20] = keyboard(HID_KEY_X);
  layers[0].keys[21] = keyboard(HID_KEY_M);
  layers[0].keys[22] = keyboard(HID_KEY_C);
  layers[0].keys[23] = keyboard(HID_KEY_V);
  // ----
  layers[0].keys[24] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTCTRL);
  layers[0].keys[25] = keyboard(HID_KEY_GUI_LEFT);
  layers[0].keys[26] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTALT);
  layers[0].keys[27] = layer_momentary(2);
  layers[0].keys[28] = layer_momentary(1);
  layers[0].keys[29] = keyboard(HID_KEY_RETURN);

  layers[0].active = true;
};
