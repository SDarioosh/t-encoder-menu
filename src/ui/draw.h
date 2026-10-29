#pragma once
#include <Arduino_GFX_Library.h>

// Shared drawing helpers so pages look consistent.
namespace Draw {

extern const GFXfont *const FONT_LARGE;  // FreeSansBold 18pt
extern const GFXfont *const FONT_MED;    // FreeSansBold 12pt
extern const GFXfont *const FONT_SMALL;  // FreeSans 9pt

constexpr uint16_t WHITE_ = 0xFFFF;
constexpr uint16_t GREY = 0x7BEF;
constexpr uint16_t DARK = 0x2104;

uint16_t rgb(uint8_t r, uint8_t g, uint8_t b);
uint16_t mix(uint16_t from, uint16_t to, float t);  // t = 0..1

void text(Arduino_GFX &g, const char *s, int cx, int baselineY, const GFXfont *font, uint16_t color);

// Icons, centred on (cx, cy), roughly `size` px wide.
void speaker(Arduino_GFX &g, int cx, int cy, int size, uint16_t color, bool waves);
void prevTrack(Arduino_GFX &g, int cx, int cy, int size, uint16_t color);
void nextTrack(Arduino_GFX &g, int cx, int cy, int size, uint16_t color);
void playPause(Arduino_GFX &g, int cx, int cy, int size, uint16_t color);

// Ring of tick marks around the screen edge; tick `active` is highlighted.
void tickRing(Arduino_GFX &g, int cx, int cy, int ticks, int active, uint16_t on, uint16_t off);

}  // namespace Draw
