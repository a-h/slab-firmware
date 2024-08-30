#ifndef SLAB_KEYMAP_H
#define SLAB_KEYMAP_H

#include "squirrel.h"

// make_keys runs all of the make_layer functions
void make_keys(void);
void make_layers(uint8_t layer);
void make_workman_layer(uint8_t layer);
void make_fn1_layer(uint8_t layer);
void make_fn2_layer(uint8_t layer);
void make_qwerty_layer(uint8_t layer);
#endif
