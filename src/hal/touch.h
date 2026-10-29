#pragma once
#include <Arduino.h>

// Touch panel. The T-Encoder Pro shipped with two screen versions:
//   older: SH8601 display + CHSC5816 touch (I2C 0x2E)
//   newer: CO5300 display + CST816 touch  (I2C 0x15)
// begin() probes I2C to find out which one is fitted.
namespace Touch {

enum class Chip { None, CST816, CHSC5816 };
enum class Gesture { None, SwipeLeft, SwipeRight, Tap };

Chip begin();
bool read(int16_t &x, int16_t &y);  // true while a finger is down
Gesture pollGesture();              // call every loop

}  // namespace Touch
