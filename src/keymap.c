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
  // row 1
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
  layers[layer].keys[10] = keyboard(HID_KEY_0);
  layers[layer].keys[11] = keyboard(HID_KEY_MINUS);
  layers[layer].keys[12] = keyboard(HID_KEY_EQUAL);
  layers[layer].keys[13] = nop();
  layers[layer].keys[14] = keyboard(HID_KEY_CAPS_LOCK);
  // row 2
  layers[layer].keys[15] = keyboard(HID_KEY_TAB);
  layers[layer].keys[16] = keyboard(HID_KEY_Q);
  layers[layer].keys[17] = keyboard(HID_KEY_D);
  layers[layer].keys[18] = keyboard(HID_KEY_R);
  layers[layer].keys[19] = keyboard(HID_KEY_W);
  layers[layer].keys[20] = keyboard(HID_KEY_B);
  layers[layer].keys[21] = keyboard(HID_KEY_J);
  layers[layer].keys[22] = keyboard(HID_KEY_F);
  layers[layer].keys[23] = keyboard(HID_KEY_U);
  layers[layer].keys[24] = keyboard(HID_KEY_P);
  layers[layer].keys[25] = keyboard(HID_KEY_SEMICOLON);
  layers[layer].keys[26] = keyboard(HID_KEY_BRACKET_LEFT);
  layers[layer].keys[27] = keyboard(HID_KEY_BRACKET_RIGHT);
  layers[layer].keys[28] = keyboard(HID_KEY_BACKSLASH);
  layers[layer].keys[29] = keyboard(HID_KEY_GRAVE);
  // row 3
  layers[layer].keys[30] = keyboard(HID_KEY_BACKSPACE);
  layers[layer].keys[31] = keyboard(HID_KEY_A);
  layers[layer].keys[32] = keyboard(HID_KEY_S);
  layers[layer].keys[33] = keyboard(HID_KEY_H);
  layers[layer].keys[34] = keyboard(HID_KEY_T);
  layers[layer].keys[35] = keyboard(HID_KEY_G);
  layers[layer].keys[36] = keyboard(HID_KEY_Y);
  layers[layer].keys[37] = keyboard(HID_KEY_N);
  layers[layer].keys[38] = keyboard(HID_KEY_E);
  layers[layer].keys[39] = keyboard(HID_KEY_O);
  layers[layer].keys[40] = keyboard(HID_KEY_I);
  layers[layer].keys[41] = keyboard(HID_KEY_APOSTROPHE);
  layers[layer].keys[42] = keyboard(HID_KEY_ENTER);
  layers[layer].keys[43] = keyboard(HID_KEY_DELETE);
  layers[layer].keys[44] = keyboard(HID_KEY_PRINT_SCREEN);
  // row 4
  layers[layer].keys[45] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTSHIFT);
  layers[layer].keys[46] = keyboard(HID_KEY_Z);
  layers[layer].keys[47] = keyboard(HID_KEY_X);
  layers[layer].keys[48] = keyboard(HID_KEY_M);
  layers[layer].keys[49] = keyboard(HID_KEY_C);
  layers[layer].keys[50] = keyboard(HID_KEY_V);
  layers[layer].keys[51] = keyboard(HID_KEY_K);
  layers[layer].keys[52] = keyboard(HID_KEY_L);
  layers[layer].keys[53] = keyboard(HID_KEY_COMMA);
  layers[layer].keys[54] = keyboard(HID_KEY_PERIOD);
  layers[layer].keys[55] = keyboard(HID_KEY_SLASH);
  layers[layer].keys[56] = keyboard_modifier(KEYBOARD_MODIFIER_RIGHTALT);
  layers[layer].keys[57] = keyboard(HID_KEY_PAGE_UP);
  layers[layer].keys[58] = keyboard(HID_KEY_ARROW_UP);
  layers[layer].keys[59] = keyboard(HID_KEY_PAGE_DOWN);
  // row 5
  layers[layer].keys[60] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTCTRL);
  layers[layer].keys[61] = keyboard(HID_KEY_GUI_LEFT);
  layers[layer].keys[62] = keyboard_modifier(KEYBOARD_MODIFIER_LEFTALT);
  layers[layer].keys[63] = layer_momentary(1);
  layers[layer].keys[64] = layer_momentary(2);
  layers[layer].keys[65] = nop();
  layers[layer].keys[66] = keyboard(HID_KEY_SPACE);
  layers[layer].keys[67] = consumer(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
  layers[layer].keys[68] = consumer(HID_USAGE_CONSUMER_PLAY_PAUSE);
  layers[layer].keys[69] = consumer(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
  layers[layer].keys[70] = keyboard(HID_KEY_HOME);
  layers[layer].keys[71] = keyboard(HID_KEY_END);
  layers[layer].keys[72] = keyboard(HID_KEY_ARROW_LEFT);
  layers[layer].keys[73] = keyboard(HID_KEY_ARROW_DOWN);
  layers[layer].keys[74] = keyboard(HID_KEY_ARROW_RIGHT);
}

// TODO: Mouse keys
void make_fn1_layer(uint8_t layer) {
  // row 1
  layers[layer].keys[0] = passthrough();
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
  layers[layer].keys[11] = keyboard(HID_KEY_F11);
  layers[layer].keys[12] = keyboard(HID_KEY_F12);
  layers[layer].keys[13] = passthrough();
  layers[layer].keys[14] = passthrough();
  // row 2
  layers[layer].keys[15] = passthrough();
  layers[layer].keys[16] = passthrough();
  layers[layer].keys[17] = passthrough();
  layers[layer].keys[18] = passthrough();
  layers[layer].keys[19] = passthrough();
  layers[layer].keys[20] = passthrough();
  layers[layer].keys[21] = passthrough();
  layers[layer].keys[22] = passthrough();
  layers[layer].keys[23] = passthrough();
  layers[layer].keys[24] = passthrough();
  layers[layer].keys[25] = passthrough();
  layers[layer].keys[26] = passthrough();
  layers[layer].keys[27] = passthrough();
  layers[layer].keys[28] = passthrough();
  layers[layer].keys[29] = passthrough();
  // row 3
  layers[layer].keys[30] = passthrough();
  layers[layer].keys[31] = passthrough();
  layers[layer].keys[32] = passthrough();
  layers[layer].keys[33] = passthrough();
  layers[layer].keys[34] = passthrough();
  layers[layer].keys[35] = passthrough();
  layers[layer].keys[36] = passthrough();
  layers[layer].keys[37] = passthrough();
  layers[layer].keys[38] = passthrough();
  layers[layer].keys[39] = passthrough();
  layers[layer].keys[40] = passthrough();
  layers[layer].keys[41] = passthrough();
  layers[layer].keys[42] = passthrough();
  layers[layer].keys[43] = passthrough();
  layers[layer].keys[44] = passthrough();
  layers[layer].keys[45] = passthrough();
  // row 4
  layers[layer].keys[46] = passthrough();
  layers[layer].keys[47] = passthrough();
  layers[layer].keys[48] = passthrough();
  layers[layer].keys[49] = passthrough();
  layers[layer].keys[50] = passthrough();
  layers[layer].keys[51] = passthrough();
  layers[layer].keys[52] = passthrough();
  layers[layer].keys[53] = passthrough();
  layers[layer].keys[54] = passthrough();
  layers[layer].keys[55] = passthrough();
  layers[layer].keys[56] = passthrough();
  layers[layer].keys[57] = passthrough();
  layers[layer].keys[58] = passthrough();
  layers[layer].keys[59] = passthrough();
  layers[layer].keys[60] = passthrough();
  // row 5
  layers[layer].keys[61] = passthrough();
  layers[layer].keys[62] = passthrough();
  layers[layer].keys[63] = passthrough();
  layers[layer].keys[64] = passthrough();
  layers[layer].keys[65] = passthrough();
  layers[layer].keys[66] = passthrough();
  layers[layer].keys[67] = consumer(HID_USAGE_CONSUMER_SCAN_PREVIOUS);
  layers[layer].keys[68] = consumer(HID_USAGE_CONSUMER_MUTE);
  layers[layer].keys[69] = consumer(HID_USAGE_CONSUMER_SCAN_NEXT);
  layers[layer].keys[70] = passthrough();
  layers[layer].keys[71] = passthrough();
  layers[layer].keys[72] = passthrough();
  layers[layer].keys[73] = passthrough();
  layers[layer].keys[74] = passthrough();
}

void make_fn2_layer(uint8_t layer) {}

void make_qwerty_layer(uint8_t layer) {}
