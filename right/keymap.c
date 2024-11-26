#include "keymap.h"
#include "tusb.h"
#include <stdlib.h>

#include <squirrel.h>
#include <squirrel_keymap.h>
#include <squirrel_quantum.h>

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
  layers[0].keys[21] = keyboard(HID_KEY_PERIOD);
  layers[0].keys[22] = keyboard(HID_KEY_SLASH);
  layers[0].keys[23] = keyboard_modifier(KEYBOARD_MODIFIER_RIGHTSHIFT);
  // ----
  layers[0].keys[24] = keyboard(HID_KEY_SPACE);
  layers[0].keys[25] = layer_momentary(1);
  layers[0].keys[26] = layer_momentary(2);
  layers[0].keys[27] = keyboard_modifier(KEYBOARD_MODIFIER_RIGHTALT);
  layers[0].keys[28] = keyboard(HID_KEY_GUI_RIGHT);
  layers[0].keys[29] = keyboard(HID_KEY_POWER);

  layers[0].active = true;

  // ----
  layers[1].keys[0] = keyboard(HID_KEY_F6);
  layers[1].keys[1] = keyboard(HID_KEY_F7);
  layers[1].keys[2] = keyboard(HID_KEY_F8);
  layers[1].keys[3] = keyboard(HID_KEY_F9);
  layers[1].keys[4] = keyboard(HID_KEY_F10);
  // ----
  layers[1].keys[7] = keyboard(HID_KEY_HOME);
  layers[1].keys[8] = keyboard(HID_KEY_END);
  layers[1].keys[9] = keyboard(HID_KEY_F11);
  layers[1].keys[10] = keyboard(HID_KEY_F12);
  // ----
  layers[1].keys[13] = keyboard(HID_KEY_MINUS);
  layers[1].keys[14] = keyboard(HID_KEY_EQUAL);
  layers[1].keys[15] = keyboard(HID_KEY_PAGE_UP);
  layers[1].keys[16] = keyboard(HID_KEY_PAGE_DOWN);
  // ----
  layers[1].keys[19] = keyboard(HID_KEY_DELETE);
  layers[1].keys[20] = keyboard(HID_KEY_PRINT_SCREEN);

  layers[1].active = false;
};
