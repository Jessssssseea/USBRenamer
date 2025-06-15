# USB Renamer

![Windows](https://img.shields.io/badge/Platform-Windows-blue)
![C++](https://img.shields.io/badge/Language-C%2B%2B-red)

Automatically renames USB drives with the volume label "CPH" to "XPH" upon insertion. Operates as a background service with no visible interface.

## Features
- Real-time USB device monitoring
- Automatic volume label modification
- Silent background operation
- System tray compatible (no UI)

## Requirements
- Windows 7 or newer
- C++17 compatible compiler (Visual Studio 2019+ recommended)
- Administrator privileges (for volume label modification)

## Build Instructions
1. **Visual Studio (Recommended):**
   - Open `USB_Renamer.sln`
   - Build Solution (Ctrl+Shift+B)
   - Output: `Release/USB_Renamer.exe`

2. **Command Line (MSVC):**
cl.exe /nologo /EHsc /std:c++17 /DUNICODE main.cpp /link user32.lib advapi32.lib

## Installation & Usage
1. Build the executable
2. Run with elevated privileges:
```cmd
USB_Renamer.exe
```
3. The program will:
   - Register as a background service
   - Monitor USB connections indefinitely
   - Automatically rename qualifying drives

## Behavior
1. When any USB storage device is inserted:
   - System detects new drive letters
   - Service checks volume label
   - If label is "CPH":
     - Immediately changes label to "XPH"
     - Leaves other drives unchanged

## Technical Notes
   - Uses `WM_DEVICECHANGE` Windows messages
   - Implements device interface notifications via `RegisterDeviceNotification()`
   - Requires admin rights for `SetVolumeLabelA()` API call
   - Hidden window message pump architecture

## Limitations
   - Windows OS only
   - Does not modify drive contents
   - Requires reboot persistence implementation
   - Logging currently disabled (uncomment `OutputDebugStringA` for debugging)

## License
MIT License - Free for personal and commercial use

---

### Key Components Explained:
1. **Background Service Architecture**  
   Creates hidden window to process device events without user interaction

2. **Device Detection Logic**  
   Uses `WM_DEVICECHANGE` with `DBT_DEVICEARRIVAL` to trigger drive checks

3. **Drive Processing**  
   `RenameDriveIfCPH()` function:
   - Checks volume label via `GetVolumeInformationA()`
   - Modifies label using `SetVolumeLabelA()` if "CPH" detected

4. **Security Context**  
   Requires administrator privileges to modify drive labels

> **Note**: For production use, consider adding:
> - System tray icon for control
> - Installation as Windows Service
> - Logging to Event Viewer
> - Configuration file support
