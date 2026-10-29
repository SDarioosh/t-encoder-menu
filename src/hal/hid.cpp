#include "hid.h"
#include <USB.h>

namespace Hid {

static USBHIDConsumerControl sConsumer;
static USBHIDKeyboard sKeyboard;

void begin() {
    sConsumer.begin();
    sKeyboard.begin();
    USB.productName("T-Encoder Pro Knob");
    USB.begin();
}

void media(uint16_t usage) {
    sConsumer.press(usage);
    sConsumer.release();
}

void keys(std::initializer_list<uint8_t> combo) {
    for (uint8_t k : combo) sKeyboard.press(k);
    delay(10);
    sKeyboard.releaseAll();
}

}  // namespace Hid
