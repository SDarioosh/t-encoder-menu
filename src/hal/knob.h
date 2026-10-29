#pragma once
#include <Arduino.h>

// Rotary encoder + push button.
namespace Knob {

enum class Press { None, Short, Long };

void begin();
int takeSteps();     // detents since last call: + clockwise, - counter-clockwise
Press pollButton();  // call every loop

}  // namespace Knob
