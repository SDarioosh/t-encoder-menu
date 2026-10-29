# Before uploading, restart the knob into its bootloader.
# The running firmware's serial port (TinyUSB) isn't the bootloader's port.
# Opening it at 1200 baud makes the board reboot into the bootloader, which
# then shows up as a new COM port. Windows can take several seconds to
# show the new port, so this waits for it.
Import("env")

import time
import serial
from serial.tools import list_ports

ESPRESSIF_VID = 0x303A


def esp_ports():
    return {p.device for p in list_ports.comports() if p.vid == ESPRESSIF_VID}


def enter_bootloader(source, target, env):
    before = esp_ports()
    if not before:
        print("upload_reset: no T-Encoder found, is it plugged in?")
        return

    for dev in before:
        # The PC volume helper opens the port for a few ms at a time, so retry if it's busy.
        for _ in range(10):
            try:
                s = serial.Serial(dev, 1200)
            except serial.SerialException:
                time.sleep(0.1)
                continue
            try:
                s.dtr = False
                s.close()
            except Exception:
                pass  # the board may vanish mid-close as it reboots
            break

    deadline = time.time() + 20
    while time.time() < deadline:
        time.sleep(0.5)
        new = esp_ports() - before
        if new:
            time.sleep(1)  # let the port settle
            port = sorted(new)[0]
            print("upload_reset: bootloader on " + port)
            env.Replace(UPLOAD_PORT=port)
            return

    # No new port: the board was probably already in the bootloader.
    port = sorted(esp_ports() or before)[0]
    print("upload_reset: using " + port)
    env.Replace(UPLOAD_PORT=port)


env.AddPreAction("upload", enter_bootloader)
