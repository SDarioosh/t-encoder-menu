#pragma once
// Pins and tunables for the LilyGO T-Encoder Pro.

// ---- Pins (from LilyGO's pin_config.h) ----
#define PIN_SCREEN_EN   3
#define PIN_SCREEN_CS   10
#define PIN_SCREEN_SCLK 12
#define PIN_SCREEN_D0   11
#define PIN_SCREEN_D1   13
#define PIN_SCREEN_D2   7
#define PIN_SCREEN_D3   14
#define PIN_SCREEN_RST  4

#define PIN_I2C_SDA     5
#define PIN_I2C_SCL     6
#define PIN_TOUCH_INT   9
#define PIN_TOUCH_RST   8

#define PIN_KNOB_A      1
#define PIN_KNOB_B      2
#define PIN_KNOB_KEY    0   // knob push button (also the BOOT button)

#define SCREEN_W 390
#define SCREEN_H 390

// ---- Behaviour ----
#define SCREEN_ROTATION     2      // 0 = LilyGO default, 2 = upside down (180°)
#define KNOB_REVERSE        false  // flip if turning clockwise lowers the volume
#define TOUCH_FLIP_X        false  // flip if swipes go the wrong way
#define SWIPE_MIN_PX        60     // horizontal travel needed to count as a swipe
#define LONG_PRESS_MS       600

#define BRIGHTNESS_ON       200
#define BRIGHTNESS_DIM      30
#define IDLE_DIM_MS         30000   // dim after 30 s without input
#define IDLE_OFF_MS         300000  // screen off after 5 min (protects the AMOLED)
