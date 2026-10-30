# Installs the knob volume helper so it starts hidden when you log in.
#   install:    powershell -ExecutionPolicy Bypass -File pc\install.ps1
#   uninstall:  powershell -ExecutionPolicy Bypass -File pc\install.ps1 -Uninstall
param([switch]$Uninstall)

$dir = Join-Path $env:LOCALAPPDATA 'TEncoderKnob'
$script = Join-Path $dir 'knob-sync.ps1'
$shortcut = Join-Path ([Environment]::GetFolderPath('Startup')) 'T-Encoder Knob Sync.lnk'

# Stop any copy that's already running.
Get-CimInstance Win32_Process -Filter "Name = 'powershell.exe'" |
    Where-Object { $_.CommandLine -like '*knob-sync.ps1*' } |
    ForEach-Object { Stop-Process -Id $_.ProcessId -Force }

if ($Uninstall) {
    Remove-Item $shortcut -ErrorAction SilentlyContinue
    Remove-Item $dir -Recurse -ErrorAction SilentlyContinue
    Write-Output 'Knob helper removed.'
    exit
}

New-Item -ItemType Directory -Force $dir | Out-Null
Copy-Item (Join-Path $PSScriptRoot 'knob-sync.ps1') $script -Force

$launch = "-NoProfile -MTA -WindowStyle Hidden -ExecutionPolicy Bypass -File `"$script`""
$lnk = (New-Object -ComObject WScript.Shell).CreateShortcut($shortcut)
$lnk.TargetPath = 'powershell.exe'
$lnk.Arguments = $launch
$lnk.WindowStyle = 7  # minimized
$lnk.Description = 'Sends Windows volume to the T-Encoder Pro knob'
$lnk.Save()

Start-Process powershell.exe -ArgumentList $launch -WindowStyle Hidden
Write-Output "Knob helper installed to $dir and set to start at login. It's running now."
