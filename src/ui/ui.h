#pragma once
#include "page.h"

// Owns the list of pages, routes input to the current one, and renders.
namespace Ui {

void begin(Page *const *pages, size_t count);

void rotate(int steps);
void press();
void longPress();
void tap();
void swipe(int direction);  // +1 = next page, -1 = previous page

void update();  // call every loop: redraws when needed, handles dimming

}  // namespace Ui
