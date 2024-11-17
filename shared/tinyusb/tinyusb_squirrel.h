#pragma once

#include "bsp/board.h"

#include "tusb.h"
#include "usb_descriptors.h"

#include "squirrel_consumer.h"
#include "squirrel_keyboard.h"

// send_hid_kbd_codes sends a HID report with the given keycodes to the host.
void send_hid_kbd_codes(uint8_t keycode_assembly[6], uint8_t modifiers);

void send_hid_no_keycodes(uint8_t modifiers);

// Every 10ms, we will send 1 HID report (per device) to the host.
// First, the keyboard. Subsequent reports will be sent in the
// tud_hid_report_complete_cb callback.
void hid_task(void);

// tud_hid_report_complete_cb is invoked when a report is sent to the host.
// report[0] is the report ID of the report just sent.
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len);
