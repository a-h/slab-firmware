#include "bsp/board.h"

#include "tusb.h"
#include "usb_descriptors.h"

#include <squirrel_consumer.h>
#include <squirrel_keyboard.h>

// send_hid_kbd_codes sends a HID report with the given keycodes to the host.
void send_hid_kbd_codes(uint8_t keycode_assembly[6], uint8_t modifiers) {
  if (!tud_hid_ready()) {
    // Don't send if HID is not ready.
    return;
  };
  // Send the currently active keycodes and modifiers to the host.
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycode_assembly);
}

void send_hid_no_keycodes(uint8_t modifiers) {
  if (!tud_hid_ready()) {
    // Don't send if HID is not ready.
    return;
  };
  // Send the currently active keycodes and modifiers to the host.
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, NULL);
}

// Every 10ms, we will send 1 HID report (per device) to the host.
// First, the keyboard. Subsequent reports will be sent in the
// tud_hid_report_complete_cb callback.
const uint32_t interval_ms = 10;
void hid_task(void) {
  static uint32_t next_report_ms = 0; // Time of next report
  if (board_millis() - next_report_ms < interval_ms) {
    return;
  };
  next_report_ms += interval_ms;

  uint8_t modifiers = keyboard_get_modifiers(); // Get the current modifiers.
  // Define an array to store the active keycodes. 6 is the limit for USB HID.
  uint8_t active_keycodes[6] = {0, 0, 0, 0, 0, 0};
  bool any_keycodes = keyboard_get_keycodes(
      &active_keycodes); // Fill the array with the keycodes.
  if (any_keycodes) {
    send_hid_kbd_codes(active_keycodes, modifiers); // Send the HID report.
  } else {
    send_hid_no_keycodes(modifiers); // Send the HID report.
  }
}

// tud_hid_report_complete_cb is invoked when a report is sent to the host.
// report[0] is the report ID of the report just sent.
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  (void)instance;
  (void)report;
  (void)len;
  if (report[0] == REPORT_ID_KEYBOARD) {
    // If the keyboard report was just sent, send the consumer report.
    uint16_t consumer_code = consumer_get_consumer_code();
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_code,
                   2); // Send the report.
    return;
  }
}
