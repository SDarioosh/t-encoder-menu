#include "host.h"

namespace Host {

static constexpr uint32_t TIMEOUT_MS = 3500;  // helper sends at least once a second

static char sLine[64];
static size_t sLen = 0;
static uint32_t sLastHeard = 0;
static bool sEverHeard = false;
static bool sWasConnected = false;
static int sVolume = 0;
static bool sMuted = false;

static bool parse(const char *line) {
    int vol, mute;
    if (sscanf(line, "vol %d mute %d", &vol, &mute) != 2) return false;
    bool changed = vol != sVolume || (mute != 0) != sMuted;
    sVolume = constrain(vol, 0, 100);
    sMuted = mute != 0;
    sLastHeard = millis();
    sEverHeard = true;
    return changed;
}

bool connected() { return sEverHeard && millis() - sLastHeard < TIMEOUT_MS; }
int volume() { return sVolume; }
bool muted() { return sMuted; }

bool poll() {
    bool changed = false;
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (sLen) {
                sLine[sLen] = 0;
                changed |= parse(sLine);
                sLen = 0;
            }
        } else if (sLen < sizeof(sLine) - 1) {
            sLine[sLen++] = c;
        }
    }
    bool now = connected();
    if (now != sWasConnected) {
        sWasConnected = now;
        changed = true;
    }
    return changed;
}

}  // namespace Host
