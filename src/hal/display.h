#pragma once
#include <Arduino_GFX_Library.h>

// AMOLED with an off-screen framebuffer: draw into canvas(), then flush().
namespace Display {

void begin(bool newerPanel);  // true = CO5300, false = SH8601
Arduino_GFX &canvas();
void flush();
void setBrightness(uint8_t level);

}  // namespace Display
