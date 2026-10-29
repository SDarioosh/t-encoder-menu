# T-Encoder Pro media knob

Firmware that turns a LilyGO T-Encoder Pro into a USB media controller. It shows up in
Windows as a keyboard with media keys, so no PC software is needed.

| Menu   | Turn                    | Press       |
|--------|-------------------------|-------------|
| Volume | volume up / down        | mute        |
| Media  | next / previous track   | play/pause  |

Swipe left/right on the screen to switch menus.

## Build and flash

```
pio run -t upload
```

Upload restarts the board into its bootloader automatically. If that ever fails: unplug the board, hold the knob down (it doubles as the BOOT button), plug it back in, then let go.

## Showing the real volume

Windows doesn't send its volume level back over USB, so a small helper does it:
`pc/knob-sync.ps1` reads the Windows volume and sends it to the knob over serial.
With the helper running, the volume wheel shows the real Windows volume
(0% and 100% at the top) and the current percentage.

```
powershell -ExecutionPolicy Bypass -File pc\install.ps1             # start at login
powershell -ExecutionPolicy Bypass -File pc\install.ps1 -Uninstall  # remove
powershell -ExecutionPolicy Bypass -File pc\knob-sync.ps1 -Test     # check it can see the knob
```

Without the helper, the knob counts clicks from 50%. That count lines up with Windows once you turn to 0 or 100.

Pages can read what the PC sends through `src/hal/host.h`. To send more (e.g. the current song), add a field to the helper's line and parse it in `host.cpp`.

## Adding a menu

1. Copy `src/pages/media_page.h` to a new file, such as `src/pages/scroll_page.h`.
2. Change `title()` and `accent()`, and fill in `onRotate`, `onPress`, `onLongPress` or `onTap`.
   - Media keys: `Hid::media(CONSUMER_CONTROL_...)` (see `USBHIDConsumerControl.h` for the full list)
   - Keyboard shortcuts: `Hid::keys({KEY_LEFT_CTRL, KEY_TAB})`
3. Draw the page in `draw()` using the helpers in `src/ui/draw.h`.
4. Add the new page to the list in `src/pages/pages.cpp`.

The page dots, swipe animation and screen dimming update on their own.

## Layout

- `src/config.h`: pins, knob/swipe direction flips, and dimming times
- `src/hal/`: hardware (knob, touch, display, USB HID)
- `src/ui/`: page base class, page manager, drawing helpers
- `src/pages/`: the menus themselves

## Third-party code

- `lib/Arduino_GFX`: Arduino_GFX 1.5.0 (BSD), the copy shipped with LilyGO's T-Encoder-Pro repo
- `lib/SensorLib`: SensorLib 0.1.4 (MIT), by Lewis He
- `include/FreeSans*.h`: fonts from Adafruit-GFX-Library (BSD)
