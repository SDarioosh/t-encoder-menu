// LilyGO T-Encoder Pro as a USB media knob.
// Swipe the screen to change menus; each menu decides what the knob does.
#include <Arduino.h>
#include "hal/display.h"
#include "hal/hid.h"
#include "hal/knob.h"
#include "hal/touch.h"
#include "pages/pages.h"
#include "ui/ui.h"

void setup() {
    Hid::begin();
    Serial.begin(115200);

    Touch::Chip chip = Touch::begin();
    // CST816 touch ships with the newer CO5300 panel; default to it if no touch chip answers.
    bool newerPanel = chip != Touch::Chip::CHSC5816;
    Display::begin(newerPanel);
    Knob::begin();
    Ui::begin(PAGES, PAGE_COUNT);

    Serial.printf("Knob ready. Touch: %s, panel: %s\n",
                  chip == Touch::Chip::CST816 ? "CST816" : chip == Touch::Chip::CHSC5816 ? "CHSC5816" : "none",
                  newerPanel ? "CO5300" : "SH8601");
}

void loop() {
    if (int steps = Knob::takeSteps()) Ui::rotate(steps);

    switch (Knob::pollButton()) {
        case Knob::Press::Short: Ui::press(); break;
        case Knob::Press::Long: Ui::longPress(); break;
        default: break;
    }

    switch (Touch::pollGesture()) {
        case Touch::Gesture::SwipeLeft: Ui::swipe(+1); break;
        case Touch::Gesture::SwipeRight: Ui::swipe(-1); break;
        case Touch::Gesture::Tap: Ui::tap(); break;
        default: break;
    }

    Ui::update();
    delay(2);
}
