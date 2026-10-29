#include "display.h"
#include "../config.h"

namespace Display {

static Arduino_DataBus *sBus;
static Arduino_GFX *sPanel;
static Arduino_Canvas *sCanvas;
static bool sNewer;
static int16_t sLevel = -1;

void begin(bool newerPanel) {
    sNewer = newerPanel;
    pinMode(PIN_SCREEN_EN, OUTPUT);
    digitalWrite(PIN_SCREEN_EN, HIGH);

    sBus = new Arduino_ESP32QSPI(PIN_SCREEN_CS, PIN_SCREEN_SCLK, PIN_SCREEN_D0, PIN_SCREEN_D1,
                                 PIN_SCREEN_D2, PIN_SCREEN_D3);
    if (newerPanel)
        sPanel = new Arduino_CO5300(sBus, PIN_SCREEN_RST, 0, false, SCREEN_W, SCREEN_H, 0, 0, 0, 0);
    else
        sPanel = new Arduino_SH8601(sBus, PIN_SCREEN_RST, 0, false, SCREEN_W, SCREEN_H);

    // Framebuffer is ~300 KB, so it lands in PSRAM.
    sCanvas = new Arduino_Canvas(SCREEN_W, SCREEN_H, sPanel);
    sCanvas->begin(40000000);
    sCanvas->setRotation(SCREEN_ROTATION);
    sCanvas->fillScreen(BLACK);
    sCanvas->flush();
    setBrightness(BRIGHTNESS_ON);
}

Arduino_GFX &canvas() { return *sCanvas; }

void flush() { sCanvas->flush(); }

void setBrightness(uint8_t level) {
    if (level == sLevel) return;
    sLevel = level;
    if (sNewer)
        static_cast<Arduino_CO5300 *>(sPanel)->setBrightness(level);
    else
        sPanel->Display_Brightness(level);
}

}  // namespace Display
