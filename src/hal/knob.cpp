#include "knob.h"
#include "../config.h"

namespace Knob {

// Quadrature decoding. State = (A<<1)|B. Clockwise runs 00 -> 10 -> 11 -> 01 -> 00.
// This knob has a detent at both 00 and 11, so two quarter-steps = one click.
static const int8_t kQuarter[16] = {0, -1, +1, 0, +1, 0, 0, -1, -1, 0, 0, +1, 0, +1, -1, 0};

static volatile uint8_t sState = 0;
static volatile int8_t sQuarter = 0;
static volatile int sSteps = 0;

static void IRAM_ATTR onEdge() {
    uint8_t s = (digitalRead(PIN_KNOB_A) << 1) | digitalRead(PIN_KNOB_B);
    int8_t q = sQuarter + kQuarter[(sState << 2) | s];
    sState = s;
    if (s == 0b00 || s == 0b11) {  // resting on a detent
        if (q >= 2) sSteps = sSteps + 1;
        else if (q <= -2) sSteps = sSteps - 1;
        q = 0;  // also discards contact bounce that went nowhere
    }
    sQuarter = q;
}

void begin() {
    pinMode(PIN_KNOB_A, INPUT_PULLUP);
    pinMode(PIN_KNOB_B, INPUT_PULLUP);
    pinMode(PIN_KNOB_KEY, INPUT_PULLUP);
    sState = (digitalRead(PIN_KNOB_A) << 1) | digitalRead(PIN_KNOB_B);
    attachInterrupt(PIN_KNOB_A, onEdge, CHANGE);
    attachInterrupt(PIN_KNOB_B, onEdge, CHANGE);
}

int takeSteps() {
    noInterrupts();
    int s = sSteps;
    sSteps = 0;
    interrupts();
    return KNOB_REVERSE ? -s : s;
}

Press pollButton() {
    static bool down = false, longFired = false;
    static uint32_t downAt = 0, lastChange = 0;
    uint32_t now = millis();
    bool pressed = digitalRead(PIN_KNOB_KEY) == LOW;

    if (pressed != down && now - lastChange > 25) {  // debounce
        lastChange = now;
        down = pressed;
        if (down) {
            downAt = now;
            longFired = false;
        } else if (!longFired) {
            return Press::Short;
        }
    }
    if (down && !longFired && now - downAt >= LONG_PRESS_MS) {
        longFired = true;
        return Press::Long;
    }
    return Press::None;
}

}  // namespace Knob
