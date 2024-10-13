#include "tusb.h"
#include "usb_descriptors.h"

// GET_REPORT callback (host requests data from device).
// Currently unused.
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  // report_id was the first byte of the buffer, report_id was removed from the
  // buffer before this function was called.

  // TODO: reimplement OpenRGB support

  // Recived data from the host.
  if (report_type == HID_REPORT_TYPE_OUTPUT) {
    // Keyboard type report
    if (report_id == REPORT_ID_KEYBOARD) {
      // Set keyboard LED e.g Capslock, Numlock etc...
      // bufsize should be (at least) 1
      if (bufsize < 1) {
        return;
      }

      uint8_t const kbd_leds = buffer[0];
      if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
      } else if (kbd_leds & KEYBOARD_LED_NUMLOCK) {
      } else if (kbd_leds & KEYBOARD_LED_SCROLLLOCK) {
      } else {
      }
    }
  }
}
