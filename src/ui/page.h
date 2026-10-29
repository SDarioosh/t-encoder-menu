#pragma once
#include <Arduino_GFX_Library.h>

// One swipeable screen ("menu") on the knob.
//
// To add a new menu:
//   1. Create a class in src/pages/ that derives from Page.
//   2. Implement title(), draw(), and whichever inputs you need.
//   3. Add an instance of it to the list in src/pages/pages.cpp.
// Swiping, page dots, slide animation and screen dimming are handled for you.
class Page {
public:
    virtual ~Page() = default;

    virtual const char *title() const = 0;
    virtual uint16_t accent() const { return 0x07FF; }  // RGB565 highlight colour

    // Inputs. steps: + clockwise, - counter-clockwise (one per detent).
    virtual void onRotate(int steps) {}
    virtual void onPress() {}
    virtual void onLongPress() {}
    virtual void onTap() {}  // tap on the touchscreen
    virtual void onEnter() {}

    // Called every loop for every page, even ones not on screen. For background work.
    virtual void tick(uint32_t now) {}

    // Draw the page body centred on (cx, cy). The title and page dots are drawn by the UI.
    virtual void draw(Arduino_GFX &g, int cx, int cy, uint32_t now) = 0;

    // Return true while the page has an animation running and needs redrawing.
    virtual bool animating(uint32_t now) const { return flashLevel(now) > 0; }

protected:
    // Small helper for "light something up briefly" feedback.
    // flash(n) marks item n; flashLevel() fades 1 -> 0 over FLASH_MS.
    static constexpr uint32_t FLASH_MS = 350;
    void flash(int id) {
        flashId_ = id;
        flashAt_ = millis();
    }
    int flashId() const { return flashId_; }
    float flashLevel(uint32_t now) const {
        if (flashId_ < 0 || now - flashAt_ >= FLASH_MS) return 0;
        return 1.0f - float(now - flashAt_) / FLASH_MS;
    }

private:
    int flashId_ = -1;
    uint32_t flashAt_ = 0;
};
