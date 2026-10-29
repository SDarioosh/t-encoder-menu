#pragma once
#include <Arduino.h>

// Status sent by the PC helper (pc/knob-sync.ps1) over USB serial.
// Each line looks like: "vol 42 mute 0"
// Without the helper running, connected() is false and pages fall back
// to their own guesses.
namespace Host {

bool poll();       // call every loop; true when something changed and the screen should redraw
bool connected();  // heard from the helper in the last few seconds
int volume();      // Windows master volume, 0..100
bool muted();

}  // namespace Host
