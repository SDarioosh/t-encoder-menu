# Sends the Windows master volume to the T-Encoder Pro knob, so its screen
# shows the real volume. Runs in the background; safe to leave running.
#
# Protocol: one line per update, "vol <0-100> mute <0|1>\n", sent on every
# change and once a second as a heartbeat.
#
# The serial port is opened only for each write, so firmware uploads can
# still grab it.

param([switch]$Test)  # -Test: send once, print what happened, exit

$ErrorActionPreference = 'Stop'

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

[Guid("5CDF2C82-841E-4546-9722-0CF74078229A"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
interface IAudioEndpointVolume {
    int RegisterControlChangeNotify(IntPtr n);
    int UnregisterControlChangeNotify(IntPtr n);
    int GetChannelCount(out uint c);
    int SetMasterVolumeLevel(float l, ref Guid g);
    int SetMasterVolumeLevelScalar(float l, ref Guid g);
    int GetMasterVolumeLevel(out float l);
    int GetMasterVolumeLevelScalar(out float l);
    int SetChannelVolumeLevel(uint c, float l, ref Guid g);
    int SetChannelVolumeLevelScalar(uint c, float l, ref Guid g);
    int GetChannelVolumeLevel(uint c, out float l);
    int GetChannelVolumeLevelScalar(uint c, out float l);
    int SetMute([MarshalAs(UnmanagedType.Bool)] bool m, ref Guid g);
    int GetMute([MarshalAs(UnmanagedType.Bool)] out bool m);
}

[Guid("D666063F-1587-4E43-81F1-B948E807363F"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
interface IMMDevice {
    int Activate(ref Guid id, int clsCtx, IntPtr activationParams, [MarshalAs(UnmanagedType.IUnknown)] out object iface);
}

[Guid("A95664D2-9614-4F35-A746-DE8DB63617E6"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
interface IMMDeviceEnumerator {
    int EnumAudioEndpoints(int dataFlow, int stateMask, out IntPtr devices);
    int GetDefaultAudioEndpoint(int dataFlow, int role, out IMMDevice endpoint);
}

[ComImport, Guid("BCDE0395-E52F-467C-8E3D-C4579291692E")]
class MMDeviceEnumerator { }

public static class KnobAudio {
    static IAudioEndpointVolume vol;

    // Re-fetch the default output device (call periodically so switching
    // speakers/headphones is picked up).
    public static void Refresh() {
        var e = (IMMDeviceEnumerator)new MMDeviceEnumerator();
        IMMDevice dev;
        Marshal.ThrowExceptionForHR(e.GetDefaultAudioEndpoint(0 /*render*/, 1 /*multimedia*/, out dev));
        Guid iid = typeof(IAudioEndpointVolume).GUID;
        object o;
        Marshal.ThrowExceptionForHR(dev.Activate(ref iid, 23 /*CLSCTX_ALL*/, IntPtr.Zero, out o));
        vol = (IAudioEndpointVolume)o;
    }

    public static int Percent() {
        float v;
        Marshal.ThrowExceptionForHR(vol.GetMasterVolumeLevelScalar(out v));
        return (int)Math.Round(v * 100);
    }

    public static bool Muted() {
        bool m;
        Marshal.ThrowExceptionForHR(vol.GetMute(out m));
        return m;
    }
}
'@

# The knob's firmware serial port: Espressif VID, interface 1.
# (The bootloader uses interface 0, so we never touch it mid-upload.)
function Find-KnobPort {
    $dev = Get-CimInstance Win32_PnPEntity -Filter "PNPDeviceID LIKE 'USB\\VID_303A&PID_1001&MI_01%'" |
        Where-Object { $_.Name -match '\((COM\d+)\)' } | Select-Object -First 1
    if ($dev -and $dev.Name -match '\((COM\d+)\)') { return $Matches[1] }
    return $null
}

function Send-Line([string]$port, [string]$line) {
    $sp = New-Object System.IO.Ports.SerialPort $port, 115200
    $sp.WriteTimeout = 300
    try {
        $sp.Open()
        $sp.Write($line)
        return $true
    } catch {
        return $false
    } finally {
        $sp.Dispose()
    }
}

if ($Test) {
    [KnobAudio]::Refresh()
    $mute = if ([KnobAudio]::Muted()) { 1 } else { 0 }
    $line = "vol $([KnobAudio]::Percent()) mute $mute"
    $port = Find-KnobPort
    Write-Output "Windows: $line"
    Write-Output "Knob port: $port"
    if ($port) { Write-Output "Sent: $(Send-Line $port "$line`n")" }
    exit
}

$port = $null
$lastSent = ''
$lastSendTime = [DateTime]::MinValue
$lastRefresh = [DateTime]::MinValue
$lastPortSearch = [DateTime]::MinValue

while ($true) {
    $now = Get-Date
    try {
        if (($now - $lastRefresh).TotalSeconds -ge 2) {
            [KnobAudio]::Refresh()
            $lastRefresh = $now
        }
        $mute = if ([KnobAudio]::Muted()) { 1 } else { 0 }
        $line = "vol $([KnobAudio]::Percent()) mute $mute`n"
    } catch {
        # No audio device right now; try again shortly.
        $lastRefresh = [DateTime]::MinValue
        Start-Sleep -Milliseconds 500
        continue
    }

    if (-not $port -and ($now - $lastPortSearch).TotalMilliseconds -ge 500) {
        $port = Find-KnobPort
        $lastPortSearch = $now
        $lastSent = ''  # resend right away to a freshly found knob
    }

    if ($port -and ($line -ne $lastSent -or ($now - $lastSendTime).TotalMilliseconds -ge 1000)) {
        if (Send-Line $port $line) {
            $lastSent = $line
            $lastSendTime = $now
        } else {
            $port = $null  # unplugged, rebooting, or being flashed
        }
    }

    Start-Sleep -Milliseconds 50
}
