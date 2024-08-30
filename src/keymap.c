#include "keymap.h"
#include "tusb.h"

#include "squirrel.h"
#include "squirrel_keymap.h"
#include "squirrel_quantum.h"

void make_keys(void) {
  make_workman_layer(0);
  make_fn1_layer(1);
  make_fn2_layer(2);
};

void make_workman_layer(uint8_t layer) {
  // Row 1
  layers[layer].keys[key_index_from_xy(0, 0, 15)] = keyboard(HID_KEY_ESCAPE);
  layers[layer].keys[key_index_from_xy(1, 0, 15)] = keyboard(HID_KEY_1);
  layers[layer].keys[key_index_from_xy(2, 0, 15)] = keyboard(HID_KEY_2);
  layers[layer].keys[key_index_from_xy(3, 0, 15)] = keyboard(HID_KEY_3);
  layers[layer].keys[key_index_from_xy(4, 0, 15)] = keyboard(HID_KEY_4);
  layers[layer].keys[key_index_from_xy(5, 0, 15)] = keyboard(HID_KEY_5);
  layers[layer].keys[key_index_from_xy(6, 0, 15)] = keyboard(HID_KEY_6);
  layers[layer].keys[key_index_from_xy(7, 0, 15)] = keyboard(HID_KEY_7);
  layers[layer].keys[key_index_from_xy(8, 0, 15)] = keyboard(HID_KEY_8);
  layers[layer].keys[key_index_from_xy(9, 0, 15)] = keyboard(HID_KEY_9);
  layers[layer].keys[key_index_from_xy(10, 0, 15)] = keyboard(HID_KEY_0);
  layers[layer].keys[key_index_from_xy(11, 0, 15)] = keyboard(HID_KEY_MINUS);
  layers[layer].keys[key_index_from_xy(12, 0, 15)] = keyboard(HID_KEY_EQUAL);
  layers[layer].keys[key_index_from_xy(14, 0, 15)] =
      keyboard(HID_KEY_CAPS_LOCK);

  // Row 2
  layers[layer].keys[0] = keyboard(HID_KEY_TAB);
  layers[layer].keys[1] = keyboard(HID_KEY_Q);
  layers[layer].keys[2] = keyboard(HID_KEY_D);
  layers[layer].keys[3] = keyboard(HID_KEY_R);
  layers[layer].keys[4] = keyboard(HID_KEY_W);
  layers[layer].keys[5] = keyboard(HID_KEY_B);
  layers[layer].keys[6] = keyboard(HID_KEY_J);
  layers[layer].keys[7] = keyboard(HID_KEY_F);
  layers[layer].keys[8] = keyboard(HID_KEY_U);
  layers[layer].keys[9] = keyboard(HID_KEY_P);
  key_add_keycode(keys[1][10], layer, HID_KEY_SEMICOLON);
  key_add_keycode(keys[1][11], layer, HID_KEY_BRACKET_LEFT);
  key_add_keycode(keys[1][12], layer, HID_KEY_BRACKET_RIGHT);
  key_add_keycode(keys[1][13], layer, HID_KEY_BACKSLASH);
  key_add_keycode(keys[1][14], layer, HID_KEY_GRAVE);

  // Row 3
  layers[layer].keys[0] = keyboard(HID_KEY_BACKSPACE);
  layers[layer].keys[1] = keyboard(HID_KEY_A);
  layers[layer].keys[2] = keyboard(HID_KEY_S);
  layers[layer].keys[3] = keyboard(HID_KEY_H);
  layers[layer].keys[4] = keyboard(HID_KEY_T);
  layers[layer].keys[5] = keyboard(HID_KEY_G);
  layers[layer].keys[6] = keyboard(HID_KEY_Y);
  layers[layer].keys[7] = keyboard(HID_KEY_N);
  layers[layer].keys[8] = keyboard(HID_KEY_E);
  layers[layer].keys[9] = keyboard(HID_KEY_O);
  key_add_keycode(keys[2][10], layer, HID_KEY_I);
  key_add_keycode(keys[2][11], layer, HID_KEY_APOSTROPHE);
  key_add_keycode(keys[2][12], layer, HID_KEY_ENTER);
  key_add_keycode(keys[2][13], layer, HID_KEY_DELETE);
  key_add_keycode(keys[2][14], layer, HID_KEY_PRINT_SCREEN);

  // Row 4
  layers[layer].keys[0] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTSHIFT);
  layers[layer].keys[1] = keyboard(HID_KEY_Z);
  layers[layer].keys[2] = keyboard(HID_KEY_X);
  layers[layer].keys[3] = keyboard(HID_KEY_M);
  layers[layer].keys[4] = keyboard(HID_KEY_C);
  layers[layer].keys[5] = keyboard(HID_KEY_V);
  layers[layer].keys[6] = keyboard(HID_KEY_K);
  layers[layer].keys[7] = keyboard(HID_KEY_L);
  layers[layer].keys[8] = keyboard(HID_KEY_COMMA);
  layers[layer].keys[9] = keyboard(HID_KEY_PERIOD);
  key_add_keycode(keys[3][10], layer, HID_KEY_SLASH);
  key_add_mod(keys[3][11], layer, KEYBOARD_MODIFIER_RIGHTALT);
  key_add_keycode(keys[3][12], layer, HID_KEY_PAGE_UP);
  key_add_keycode(keys[3][13], layer, HID_KEY_ARROW_UP);
  key_add_keycode(keys[3][14], layer, HID_KEY_PAGE_DOWN);

  // Row 5
  key_add_mod(keys[4][0], layer, KEYBOARD_MODIFIER_LEFTCTRL);
  key_add_keycode(keys[4][1], layer, HID_KEY_GUI_LEFT);
  key_add_mod(keys[4][2], layer, KEYBOARD_MODIFIER_LEFTALT);
  key_add_momentary(keys[4][3], layer, LAYER_FN1);  // FN_1
  key_add_momentary(keys[4][4], layer, LAYER_FN2);  // FN_2
  key_add_default(keys[4][5], layer, LAYER_QWERTY); // QWERTY
  key_add_keycode(keys[4][6], layer, HID_KEY_SPACE);
  key_add_media(keys[4][7], layer, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
  key_add_media(keys[4][8], layer, HID_USAGE_CONSUMER_PLAY_PAUSE);
  key_add_media(keys[4][9], layer, HID_USAGE_CONSUMER_VOLUME_INCREMENT);
  key_add_keycode(keys[4][10], layer, HID_KEY_HOME);
  key_add_keycode(keys[4][11], layer, HID_KEY_END);
  key_add_keycode(keys[4][12], layer, HID_KEY_ARROW_LEFT);
  key_add_keycode(keys[4][13], layer, HID_KEY_ARROW_DOWN);
  key_add_keycode(keys[4][14], layer, HID_KEY_ARROW_RIGHT);
}

void make_fn1_layer(uint8_t layer) {
  for (uint8_t y = 0; y < 5; y++) {
    for (uint8_t x = 0; x < 15; x++) {
      // Fill the array with pass-through keys.
      key_add_pass_through(keys[y][x], layer);
    }
  }

  // Row 1
  layers[layer].keys[1] = keyboard(HID_KEY_F1);
  layers[layer].keys[2] = keyboard(HID_KEY_F2);
  layers[layer].keys[3] = keyboard(HID_KEY_F3);
  layers[layer].keys[4] = keyboard(HID_KEY_F4);
  layers[layer].keys[5] = keyboard(HID_KEY_F5);
  layers[layer].keys[6] = keyboard(HID_KEY_F6);
  layers[layer].keys[7] = keyboard(HID_KEY_F7);
  layers[layer].keys[8] = keyboard(HID_KEY_F8);
  layers[layer].keys[9] = keyboard(HID_KEY_F9);
  layers[layer].keys[10] = keyboard(HID_KEY_F10);
  key_add_keycode(keys[0][11], layer, HID_KEY_F11);
  key_add_keycode(keys[0][12], layer, HID_KEY_F12);

  key_add_media(keys[1][1], layer, HID_USAGE_CONSUMER_SLEEP);
  key_add_media(keys[1][2], layer, HID_USAGE_CONSUMER_POWER);
  key_add_media(keys[1][3], layer, HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT);
  key_add_media(keys[1][4], layer, HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT);
  key_add_media(keys[1][5], layer, HID_USAGE_CONSUMER_AL_LOCAL_BROWSER);
  key_add_media(keys[1][6], layer, HID_USAGE_CONSUMER_AL_CALCULATOR);
  key_add_media(keys[1][7], layer, HID_USAGE_CONSUMER_AL_EMAIL_READER);
  key_add_media(keys[1][8], layer, HID_USAGE_CONSUMER_BASS_DECREMENT);
  key_add_media(keys[1][9], layer, HID_USAGE_CONSUMER_BASS_INCREMENT);
  key_add_media(keys[1][10], layer, HID_USAGE_CONSUMER_TREBLE_DECREMENT);
  key_add_media(keys[1][11], layer, HID_USAGE_CONSUMER_TREBLE_INCREMENT);
  key_add_media(keys[1][12], layer, HID_USAGE_CONSUMER_BASS_BOOST);
  key_add_media(keys[1][13], layer, HID_USAGE_CONSUMER_AC_BACK);
  key_add_media(keys[1][14], layer, HID_USAGE_CONSUMER_AC_FORWARD);

  // Row 4
  // TODO: Mouse keys

  // Row 5
  key_add_media(keys[4][7], layer, HID_USAGE_CONSUMER_SCAN_PREVIOUS);
  key_add_media(keys[4][8], layer, HID_USAGE_CONSUMER_MUTE);
  key_add_media(keys[4][9], layer, HID_USAGE_CONSUMER_SCAN_NEXT);
  // TODO: Mouse keys
}

void make_fn2_layer(uint8_t layer) {
  for (uint8_t y = 0; y < 5; y++) {
    for (uint8_t x = 0; x < 15; x++) {
      // Fill the array with pass-through keys.
      key_add_pass_through(keys[y][x], layer);
    }
  }
  // Row 1
  key_add_media(keys[0][1], layer, HID_USAGE_CONSUMER_SLEEP);
  key_add_media(keys[0][2], layer, HID_USAGE_CONSUMER_POWER);
  key_add_media(keys[0][3], layer, HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT);
  key_add_media(keys[0][4], layer, HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT);
  key_add_media(keys[0][5], layer, HID_USAGE_CONSUMER_AL_LOCAL_BROWSER);
  key_add_media(keys[0][6], layer, HID_USAGE_CONSUMER_AL_CALCULATOR);
  key_add_media(keys[0][7], layer, HID_USAGE_CONSUMER_AL_EMAIL_READER);
  key_add_media(keys[0][8], layer, HID_USAGE_CONSUMER_BASS_DECREMENT);
  key_add_media(keys[0][9], layer, HID_USAGE_CONSUMER_BASS_INCREMENT);
  key_add_media(keys[0][10], layer, HID_USAGE_CONSUMER_TREBLE_DECREMENT);
  key_add_media(keys[0][11], layer, HID_USAGE_CONSUMER_TREBLE_INCREMENT);
  key_add_media(keys[0][12], layer, HID_USAGE_CONSUMER_BASS_BOOST);
  key_add_media(keys[0][13], layer, HID_USAGE_CONSUMER_AC_BACK);
  key_add_media(keys[0][14], layer, HID_USAGE_CONSUMER_AC_FORWARD);
}

void make_qwerty_layer(uint8_t layer) {
  for (uint8_t y = 0; y < 5; y++) {
    for (uint8_t x = 0; x < 15; x++) {
      // Fill the array with pass-through keys.
      key_add_pass_through(keys[y][x], layer);
    }
  }

  // Row 1
  layers[layer].keys[0] = keyboard(HID_KEY_ESCAPE);
  layers[layer].keys[1] = keyboard(HID_KEY_1);
  layers[layer].keys[2] = keyboard(HID_KEY_2);
  layers[layer].keys[3] = keyboard(HID_KEY_3);
  layers[layer].keys[4] = keyboard(HID_KEY_4);
  layers[layer].keys[5] = keyboard(HID_KEY_5);
  layers[layer].keys[6] = keyboard(HID_KEY_6);
  layers[layer].keys[7] = keyboard(HID_KEY_7);
  layers[layer].keys[8] = keyboard(HID_KEY_8);
  layers[layer].keys[9] = keyboard(HID_KEY_9);
  key_add_keycode(keys[0][10], layer, HID_KEY_0);
  key_add_keycode(keys[0][11], layer, HID_KEY_MINUS);
  key_add_keycode(keys[0][12], layer, HID_KEY_EQUAL);
  key_add_custom_code(
      keys[0][13],
      layer); // when held, enter a code in binary then press enter.
  key_add_keycode(keys[0][14], layer, HID_KEY_CAPS_LOCK);

  // Row 2
  layers[layer].keys[0] = keyboard(HID_KEY_TAB);
  layers[layer].keys[1] = keyboard(HID_KEY_Q);
  layers[layer].keys[2] = keyboard(HID_KEY_W);
  layers[layer].keys[3] = keyboard(HID_KEY_E);
  layers[layer].keys[4] = keyboard(HID_KEY_R);
  layers[layer].keys[5] = keyboard(HID_KEY_T);
  layers[layer].keys[6] = keyboard(HID_KEY_Y);
  layers[layer].keys[7] = keyboard(HID_KEY_U);
  layers[layer].keys[8] = keyboard(HID_KEY_I);
  layers[layer].keys[9] = keyboard(HID_KEY_O);
  key_add_keycode(keys[1][10], layer, HID_KEY_P);
  key_add_keycode(keys[1][11], layer, HID_KEY_BRACKET_LEFT);
  key_add_keycode(keys[1][12], layer, HID_KEY_BRACKET_RIGHT);
  key_add_keycode(keys[1][13], layer, HID_KEY_BACKSLASH);
  key_add_keycode(keys[1][14], layer, HID_KEY_GRAVE);

  // Row 3
  layers[layer].keys[0] = keyboard(HID_KEY_BACKSPACE);
  layers[layer].keys[1] = keyboard(HID_KEY_A);
  layers[layer].keys[2] = keyboard(HID_KEY_S);
  layers[layer].keys[3] = keyboard(HID_KEY_D);
  layers[layer].keys[4] = keyboard(HID_KEY_F);
  layers[layer].keys[5] = keyboard(HID_KEY_G);
  layers[layer].keys[6] = keyboard(HID_KEY_H);
  layers[layer].keys[7] = keyboard(HID_KEY_J);
  layers[layer].keys[8] = keyboard(HID_KEY_K);
  layers[layer].keys[9] = keyboard(HID_KEY_L);
  key_add_keycode(keys[2][10], layer, HID_KEY_SEMICOLON);
  key_add_keycode(keys[2][11], layer, HID_KEY_APOSTROPHE);
  key_add_keycode(keys[2][12], layer, HID_KEY_ENTER);
  key_add_keycode(keys[2][13], layer, HID_KEY_DELETE);
  key_add_keycode(keys[2][14], layer, HID_KEY_PRINT_SCREEN);

  // Row 4
  layers[layer].keys[0] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTSHIFT);
  layers[layer].keys[1] = keyboard(HID_KEY_Z);
  layers[layer].keys[2] = keyboard(HID_KEY_X);
  layers[layer].keys[3] = keyboard(HID_KEY_C);
  layers[layer].keys[4] = keyboard(HID_KEY_V);
  layers[layer].keys[5] = keyboard(HID_KEY_B);
  layers[layer].keys[6] = keyboard(HID_KEY_N);
  layers[layer].keys[7] = keyboard(HID_KEY_M);
  layers[layer].keys[8] = keyboard(HID_KEY_COMMA);
  layers[layer].keys[9] = keyboard(HID_KEY_PERIOD);
  key_add_keycode(keys[3][10], layer, HID_KEY_SLASH);
  key_add_mod(keys[3][11], layer, KEYBOARD_MODIFIER_RIGHTALT);
  key_add_keycode(keys[3][12], layer, HID_KEY_PAGE_UP);
  key_add_keycode(keys[3][13], layer, HID_KEY_ARROW_UP);
  key_add_keycode(keys[3][14], layer, HID_KEY_PAGE_DOWN);

  // Row 5
  key_add_mod(keys[4][0], layer, KEYBOARD_MODIFIER_LEFTCTRL);
  key_add_keycode(keys[4][1], layer, HID_KEY_GUI_LEFT);
  key_add_mod(keys[4][2], layer, KEYBOARD_MODIFIER_LEFTALT);
  key_add_momentary(keys[4][3], layer, LAYER_FN1);   // FN_1
  key_add_momentary(keys[4][4], layer, LAYER_FN2);   // FN_2
  key_add_default(keys[4][5], layer, LAYER_WORKMAN); // WORKMAN
  key_add_keycode(keys[4][6], layer, HID_KEY_SPACE);
  key_add_media(keys[4][7], layer, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
  key_add_media(keys[4][8], layer, HID_USAGE_CONSUMER_PLAY_PAUSE);
  key_add_media(keys[4][9], layer, HID_USAGE_CONSUMER_VOLUME_INCREMENT);
  key_add_keycode(keys[4][10], layer, HID_KEY_HOME);
  key_add_keycode(keys[4][11], layer, HID_KEY_END);
  key_add_keycode(keys[4][12], layer, HID_KEY_ARROW_LEFT);
  key_add_keycode(keys[4][13], layer, HID_KEY_ARROW_DOWN);
  key_add_keycode(keys[4][14], layer, HID_KEY_ARROW_RIGHT);
}
