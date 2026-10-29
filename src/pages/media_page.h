#pragma once
#include "../hal/hid.h"
#include "../ui/draw.h"
#include "../ui/page.h"

// Turn right: next track. Turn left: previous track. Press: play/pause.
class MediaPage : public Page {
public:
    const char *title() const override { return "Media"; }
    uint16_t accent() const override { return Draw::rgb(255, 140, 40); }  // orange

    void onRotate(int steps) override {
        // One track per click, but ignore a fast spin so you don't skip 10 songs.
        uint32_t now = millis();
        if (now - lastSkip_ < SKIP_COOLDOWN_MS) return;
        lastSkip_ = now;
        Hid::media(steps > 0 ? CONSUMER_CONTROL_SCAN_NEXT : CONSUMER_CONTROL_SCAN_PREVIOUS);
        flash(steps > 0 ? NEXT : PREV);
    }

    void onPress() override {
        Hid::media(CONSUMER_CONTROL_PLAY_PAUSE);
        flash(PLAY);
    }

    void draw(Arduino_GFX &g, int cx, int cy, uint32_t now) override {
        float f = flashLevel(now);
        const int spacing = 100, size = 56;
        for (int i = 0; i < 3; i++) {
            int x = cx + (i - 1) * spacing;
            bool hot = f > 0 && flashId() == i;
            if (hot) g.fillCircle(x, cy, 44, Draw::mix(BLACK, accent(), f * 0.5f));
            uint16_t c = hot ? Draw::mix(Draw::WHITE_, accent(), f) : Draw::WHITE_;
            if (i == PREV) Draw::prevTrack(g, x, cy, size, c);
            if (i == PLAY) Draw::playPause(g, x, cy, size, c);
            if (i == NEXT) Draw::nextTrack(g, x, cy, size, c);
        }
        Draw::text(g, "press to play / pause", cx, cy + 85, Draw::FONT_SMALL, Draw::GREY);
    }

private:
    enum { PREV, PLAY, NEXT };
    static constexpr uint32_t SKIP_COOLDOWN_MS = 300;
    uint32_t lastSkip_ = 0;
};
