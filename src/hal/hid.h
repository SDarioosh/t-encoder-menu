#pragma once
#include <Arduino.h>
#include <USBHIDConsumerControl.h>
#include <USBHIDKeyboard.h>

// The board appears to Windows as a USB keyboard with media keys.
// No driver or PC-side app is needed.
namespace Hid {

void begin();

// Media / consumer keys, e.g. CONSUMER_CONTROL_VOLUME_INCREMENT,
// CONSUMER_CONTROL_SCAN_NEXT, CONSUMER_CONTROL_PLAY_PAUSE (see USBHIDConsumerControl.h).
void media(uint16_t usage);

// Regular keyboard shortcuts, e.g. Hid::keys({KEY_LEFT_CTRL, 'z'}).
void keys(std::initializer_list<uint8_t> combo);

}  // namespace Hid
