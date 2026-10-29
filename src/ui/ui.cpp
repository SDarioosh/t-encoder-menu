#include "ui.h"
#include "../config.h"
#include "../hal/display.h"
#include "../hal/host.h"
#include "draw.h"

namespace Ui {

static Page *const *sPages;
static size_t sCount;
static size_t sCurrent = 0;

static bool sDirty = true;
static uint32_t sLastInput = 0;
static uint32_t sLastFrame = 0;

// Slide animation between pages.
static constexpr uint32_t SLIDE_MS = 220;
static int sSlideFrom = -1;  // page index sliding out, -1 = none
static int sSlideDir = 0;
static uint32_t sSlideStart = 0;

static constexpr int CX = SCREEN_W / 2;
static constexpr int CY = SCREEN_H / 2;

static Page &current() { return *sPages[sCurrent]; }

static void wake() {
    sLastInput = millis();
    Display::setBrightness(BRIGHTNESS_ON);
    sDirty = true;
}

void begin(Page *const *pages, size_t count) {
    sPages = pages;
    sCount = count;
    sCurrent = 0;
    current().onEnter();
    wake();
}

void rotate(int steps) { wake(); current().onRotate(steps); }
void press() { wake(); current().onPress(); }
void longPress() { wake(); current().onLongPress(); }
void tap() { wake(); current().onTap(); }

void swipe(int dir) {
    wake();
    if (sCount < 2) return;
    sSlideFrom = sCurrent;
    sSlideDir = dir;
    sSlideStart = millis();
    sCurrent = (sCurrent + sCount + dir) % sCount;
    current().onEnter();
}

static void drawPage(Arduino_GFX &g, Page &p, int offsetX, uint32_t now) {
    Draw::text(g, p.title(), CX + offsetX, 92, Draw::FONT_MED, p.accent());
    p.draw(g, CX + offsetX, CY + 5, now);
}

static void drawDots(Arduino_GFX &g) {
    const int gap = 18, y = SCREEN_H - 40;
    int x0 = CX - int(sCount - 1) * gap / 2;
    for (size_t i = 0; i < sCount; i++) {
        bool on = i == sCurrent;
        g.fillCircle(x0 + i * gap, y, on ? 5 : 3, on ? current().accent() : Draw::GREY);
    }
}

static void render(uint32_t now) {
    Arduino_GFX &g = Display::canvas();
    g.fillScreen(BLACK);

    if (sSlideFrom >= 0) {
        float t = float(now - sSlideStart) / SLIDE_MS;
        if (t >= 1) {
            sSlideFrom = -1;
        } else {
            float e = 1 - (1 - t) * (1 - t);  // ease-out
            int shift = int(e * SCREEN_W) * sSlideDir;
            drawPage(g, *sPages[sSlideFrom], -shift, now);
            drawPage(g, current(), sSlideDir * SCREEN_W - shift, now);
        }
    }
    if (sSlideFrom < 0) drawPage(g, current(), 0, now);

    drawDots(g);
    Display::flush();
}

void update() {
    if (Host::poll()) sDirty = true;
    uint32_t now = millis();
    for (size_t i = 0; i < sCount; i++) sPages[i]->tick(now);

    // Idle dimming.
    uint32_t idle = now - sLastInput;
    if (idle > IDLE_OFF_MS) Display::setBrightness(0);
    else if (idle > IDLE_DIM_MS) Display::setBrightness(BRIGHTNESS_DIM);

    bool moving = sSlideFrom >= 0 || current().animating(now);
    if ((sDirty || moving) && now - sLastFrame >= 16) {
        sDirty = false;
        sLastFrame = now;
        render(now);
        // One last frame after an animation ends so it settles cleanly.
        if (moving) sDirty = true;
    }
}

}  // namespace Ui
