#pragma once
#include <Preferences.h>
#include "../hal/hid.h"
#include "../hal/host.h"
#include "../ui/draw.h"
#include "../ui/page.h"

// Turn: Windows volume up/down. Press: mute toggle.
//
// The wheel shows the real Windows volume when the PC helper (pc/knob-sync.ps1)
// is running: 0% and 100% both sit at the top, and a full lap covers the range.
// Without the helper it falls back to counting clicks from 50%.
class VolumePage : public Page {
public:
    const char *title() const override { return "Volume"; }
    uint16_t accent() const override { return 0x07FF; }  // cyan

    void onRotate(int steps) override {
        // Always send the keys, even at the limit, so Windows reaches the real
        // 0 or 100 when our count is off.
        uint16_t key = steps > 0 ? CONSUMER_CONTROL_VOLUME_INCREMENT : CONSUMER_CONTROL_VOLUME_DECREMENT;
        for (int i = 0; i < abs(steps); i++) Hid::media(key);

        int before = level();
        level_ = constrain(before + steps * STEP, 0, 100);
        lastTurn_ = millis();
        if (level_ == before) flash(steps > 0 ? AT_MAX : AT_MIN);  // at the end: wheel stays put
        else flash(steps > 0 ? UP : DOWN);
    }

    void onPress() override {
        Hid::media(CONSUMER_CONTROL_MUTE);
        flash(MUTE);
    }

    void draw(Arduino_GFX &g, int cx, int cy, uint32_t now) override {
        float f = flashLevel(now);
        int vol = level();
        bool muted = Host::connected() && Host::muted();

        Draw::tickRing(g, cx, cy, TICKS, (vol / STEP) % TICKS, muted ? Draw::GREY : accent(), Draw::DARK);
        bool waves = !muted && !(flashId() == MUTE && f > 0);
        Draw::speaker(g, cx, cy - 10, 90, Draw::mix(muted ? Draw::GREY : Draw::WHITE_, accent(), f), waves);

        char buf[8];
        const char *label;
        const GFXfont *font = Draw::FONT_SMALL;
        uint16_t color = Draw::GREY;
        if (f > 0) {
            switch (flashId()) {
                case UP: label = "+"; font = Draw::FONT_LARGE; break;
                case DOWN: label = "-"; font = Draw::FONT_LARGE; break;
                case AT_MAX: label = "max"; break;
                case AT_MIN: label = "min"; break;
                default: label = "mute"; break;
            }
            color = Draw::mix(Draw::DARK, accent(), f);
        } else if (muted) {
            label = "muted";
        } else if (Host::connected()) {
            snprintf(buf, sizeof(buf), "%d%%", vol);
            label = buf;
            font = Draw::FONT_MED;
            color = Draw::WHITE_;
        } else {
            label = "press to mute";
        }
        Draw::text(g, label, cx, cy + 85, font, color);
    }

private:
    enum { UP, DOWN, MUTE, AT_MAX, AT_MIN };
    static constexpr int STEP = 2;            // Windows moves 2% per volume key
    static constexpr int TICKS = 100 / STEP;  // one tick per click: 0..100% is one lap
    static constexpr uint32_t SETTLE_MS = 400;

    int level_ = 50;
    uint32_t lastTurn_ = 0;
    bool loaded_ = false;
    int saved_ = -1;
    uint32_t lastSave_ = 0;

    int level() {
        // Start from the last volume Windows reported before the knob was
        // restarted, so the wheel is right before the helper reconnects.
        if (!loaded_) {
            loaded_ = true;
            Preferences p;
            p.begin("volume", true);
            saved_ = p.getInt("level", 50);
            p.end();
            level_ = saved_;
        }
        // Follow Windows when the helper is running, except right after a turn
        // so the wheel doesn't jump back while Windows catches up.
        if (Host::connected() && millis() - lastTurn_ > SETTLE_MS) level_ = Host::volume();
        return level_;
    }

public:
    // Remember the Windows volume for next boot (at most every few seconds, to spare the flash).
    void tick(uint32_t now) override {
        if (!Host::connected() || now - lastSave_ < 3000) return;
        int vol = level();
        if (vol == saved_) return;
        Preferences p;
        p.begin("volume", false);
        p.putInt("level", vol);
        p.end();
        saved_ = vol;
        lastSave_ = now;
    }
};
