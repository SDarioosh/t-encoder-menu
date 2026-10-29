#include "draw.h"
#include "FreeSans9pt7b.h"
#include "FreeSansBold12pt7b.h"
#include "FreeSansBold18pt7b.h"

namespace Draw {

const GFXfont *const FONT_LARGE = &FreeSansBold18pt7b;
const GFXfont *const FONT_MED = &FreeSansBold12pt7b;
const GFXfont *const FONT_SMALL = &FreeSans9pt7b;

uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }

uint16_t mix(uint16_t a, uint16_t b, float t) {
    if (t <= 0) return a;
    if (t >= 1) return b;
    auto ch = [t](int x, int y) { return int(x + (y - x) * t); };
    int r = ch(a >> 11, b >> 11), gg = ch((a >> 5) & 0x3F, (b >> 5) & 0x3F), bb = ch(a & 0x1F, b & 0x1F);
    return (r << 11) | (gg << 5) | bb;
}

void text(Arduino_GFX &g, const char *s, int cx, int baselineY, const GFXfont *font, uint16_t color) {
    g.setFont(font);
    g.setTextSize(1);
    int16_t x1, y1;
    uint16_t w, h;
    g.getTextBounds(s, 0, baselineY, &x1, &y1, &w, &h);
    g.setTextColor(color);
    g.setCursor(cx - int(w) / 2 - x1, baselineY);
    g.print(s);
}

void speaker(Arduino_GFX &g, int cx, int cy, int s, uint16_t c, bool waves) {
    int left = cx - s / 2;
    int bodyW = s / 5, bodyH = s * 2 / 5;
    g.fillRect(left, cy - bodyH / 2, bodyW, bodyH, c);
    int coneX = left + bodyW, coneW = s / 4, coneH = s * 4 / 5;
    // Cone: trapezoid from the body out to a taller edge.
    g.fillTriangle(coneX, cy - bodyH / 2, coneX + coneW, cy - coneH / 2, coneX + coneW, cy + coneH / 2, c);
    g.fillTriangle(coneX, cy - bodyH / 2, coneX, cy + bodyH / 2, coneX + coneW, cy + coneH / 2, c);
    if (waves) {
        int ox = coneX + coneW - s / 10;
        g.fillArc(ox, cy, s * 3 / 10, s * 3 / 10 - 6, -45, 45, c);
        g.fillArc(ox, cy, s / 2, s / 2 - 6, -45, 45, c);
    }
}

static void tri(Arduino_GFX &g, int x, int cy, int w, int h, bool right, uint16_t c) {
    if (right) g.fillTriangle(x, cy - h / 2, x, cy + h / 2, x + w, cy, c);
    else g.fillTriangle(x + w, cy - h / 2, x + w, cy + h / 2, x, cy, c);
}

void prevTrack(Arduino_GFX &g, int cx, int cy, int s, uint16_t c) {
    int bar = s / 8, triW = (s - bar) / 2;
    int x = cx - s / 2;
    g.fillRect(x, cy - s / 3, bar, s * 2 / 3, c);
    tri(g, x + bar, cy, triW, s * 2 / 3, false, c);
    tri(g, x + bar + triW, cy, triW, s * 2 / 3, false, c);
}

void nextTrack(Arduino_GFX &g, int cx, int cy, int s, uint16_t c) {
    int bar = s / 8, triW = (s - bar) / 2;
    int x = cx - s / 2;
    tri(g, x, cy, triW, s * 2 / 3, true, c);
    tri(g, x + triW, cy, triW, s * 2 / 3, true, c);
    g.fillRect(x + 2 * triW, cy - s / 3, bar, s * 2 / 3, c);
}

void playPause(Arduino_GFX &g, int cx, int cy, int s, uint16_t c) {
    int x = cx - s / 2;
    tri(g, x, cy, s / 2, s * 2 / 3, true, c);
    int bar = s / 7;
    g.fillRect(x + s / 2 + bar, cy - s / 3, bar, s * 2 / 3, c);
    g.fillRect(x + s / 2 + 3 * bar, cy - s / 3, bar, s * 2 / 3, c);
}

void tickRing(Arduino_GFX &g, int cx, int cy, int ticks, int active, uint16_t on, uint16_t off) {
    float step = 360.0f / ticks;
    for (int i = 0; i < ticks; i++) {
        float a = i * step - 90;
        bool hot = i == active;
        g.fillArc(cx, cy, 186, hot ? 160 : 172, a - 1.5f, a + 1.5f, hot ? on : off);
    }
}

}  // namespace Draw
