# Auto-Mute Audio on Windows

This project is a lightweight Windows program that automatically mutes audio sessions for specific devices (e.g., microphones, headsets) to prevent echo or unwanted sound during screen sharing or other scenarios. It runs silently in the background and requires minimal configuration.

## Features

- Automatically mutes sessions for selected audio devices
- Works for dynamically created audio sessions (e.g., when starting a screen share or launching an app)
- Runs in the background with minimal CPU and memory usage
- Configurable via config.json
- Tested on Windows 10/11
- Lightweight and suitable for autostart

## Motivation

During meetings, screen sharing, or audio recording, certain devices (e.g., microphones or headsets) can introduce echo or feedback. Manually muting audio sessions for each device and application is tedious. This program automates the process:

1. Mutes audio output for selected devices for a specified application  
2. Ensures that new audio sessions are automatically muted

The program periodically checks all audio sessions and enforces mute on matching devices for the configured target application.

## Installation

1. Download this repository as zip and unpack.

2. Optional: Compile the program (gcc - gnu compiler collection necessary or use the pre-compiled AutoMuteApp version in this repository if your do not want to compile and just start with it):
   `gcc src/*.c -Iinclude -lole32 -luuid -lpropsys -lpsapi -mwindows -o AutoMuteApp.exe`

3. Place config.json in the same folder as the executable.
4. Edit config.json and place your preferences in the required fields (the program is case sensitive and only needs as many words as necessary to distinguish it from other devices, so not the whole name is required).
5. Optional: Add the program shortcut to Autostart to run at login. For this press **Win + R** and then type `shell:startup` and put the shortcut in there.

## Configuration

Edit config.json to adjust:

- Target application: the executable for which audio sessions should be muted (e.g., Discord.exe, Zoom.exe, or any other Windows application)
- Devices: audio devices to mute (partial names are supported)
- Polling interval: how frequently the program checks audio sessions (in milliseconds)

Example config.json:
```json
{
  "poll_interval_ms": 2000,
  "target_application": "Discord.exe",
  "devices": [
    { "name_contains": "Aux" },
    { "name_contains": "Headset" }
  ]
}
```

You can replace "Discord.exe" with any other application name, for example "Zoom.exe" or "Teams.exe". The program will mute audio sessions for the specified devices only for that application.

## How to fix/reverse if problems occured
1. Make sure the **AutoMuteApp.exe** is **not** running anymore in the background, using the task manager to eventually close the background task.
2. Press **Win + R** and search for `sndvol`.
3. Look for your devices and unmute the corresponding tasks.

## How it Works

1. On start, the program reads config.json.
2. Runs a background thread that periodically enumerates all active audio endpoints.
3. For each endpoint, checks for devices that match the configured names.
4. For matching devices, enumerates all audio sessions.
5. If the session belongs to the target application, it enforces mute.

This loop ensures that even sessions created dynamically (e.g., launching a new call or screen share) are immediately muted.

## Known Limitations

- Tested on Windows 10/11 only
- Can be scheduled only using Task Scheduler
- Audio muting is one-way; the program does not restore audio
- Behavior may vary with future Windows or application updates
- Requires the target application to be running
