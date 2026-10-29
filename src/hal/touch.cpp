#include "touch.h"
#include <Wire.h>
#include <TouchDrvCHSC5816.hpp>
#include "../config.h"

namespace Touch {

static const uint8_t CST816_ADDR = 0x15;
static Chip sChip = Chip::None;
static TouchDrvCHSC5816 sChsc;

static bool probe(uint8_t addr) {
    Wire.beginTransmission(addr);
    return Wire.endTransmission() == 0;
}

Chip begin() {
    pinMode(PIN_TOUCH_RST, OUTPUT);
    digitalWrite(PIN_TOUCH_RST, LOW);
    delay(10);
    digitalWrite(PIN_TOUCH_RST, HIGH);
    delay(60);

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    if (probe(CST816_ADDR)) {
        sChip = Chip::CST816;
        Wire.beginTransmission(CST816_ADDR);  // stop it auto-sleeping
        Wire.write(0xFE);
        Wire.write(0x01);
        Wire.endTransmission();
    } else if (probe(CHSC5816_SLAVE_ADDRESS)) {
        sChsc.setPins(PIN_TOUCH_RST, PIN_TOUCH_INT);
        if (sChsc.begin(Wire, CHSC5816_SLAVE_ADDRESS, PIN_I2C_SDA, PIN_I2C_SCL)) sChip = Chip::CHSC5816;
    }
    return sChip;
}

bool read(int16_t &x, int16_t &y) {
    if (sChip == Chip::CST816) {
        Wire.beginTransmission(CST816_ADDR);
        Wire.write(0x02);  // finger count, then XH XL YH YL
        if (Wire.endTransmission(false) != 0) return false;
        if (Wire.requestFrom(CST816_ADDR, (uint8_t)5) != 5) return false;
        uint8_t b[5];
        for (auto &v : b) v = Wire.read();
        if (b[0] == 0) return false;
        x = ((b[1] & 0x0F) << 8) | b[2];
        y = ((b[3] & 0x0F) << 8) | b[4];
        return true;
    }
    if (sChip == Chip::CHSC5816) {
        return sChsc.getPoint(&x, &y) > 0;
    }
    return false;
}

Gesture pollGesture() {
    static bool down = false;
    static int16_t startX, startY, lastX, lastY;
    static uint32_t lastPoll = 0;

    uint32_t now = millis();
    if (now - lastPoll < 15) return Gesture::None;
    lastPoll = now;

    int16_t x, y;
    if (read(x, y)) {
        if (!down) {
            down = true;
            startX = x;
            startY = y;
        }
        lastX = x;
        lastY = y;
        return Gesture::None;
    }
    if (!down) return Gesture::None;

    // Finger lifted: classify the movement.
    down = false;
    int dx = lastX - startX, dy = lastY - startY;
    if (SCREEN_ROTATION == 2) {  // touch panel doesn't rotate with the picture
        dx = -dx;
        dy = -dy;
    }
    if (TOUCH_FLIP_X) dx = -dx;
    if (abs(dx) >= SWIPE_MIN_PX && abs(dx) > abs(dy)) {
        return dx < 0 ? Gesture::SwipeLeft : Gesture::SwipeRight;
    }
    if (abs(dx) < 20 && abs(dy) < 20) return Gesture::Tap;
    return Gesture::None;
}

}  // namespace Touch
