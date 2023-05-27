README
===========================
APS(Airplay Server) is a complete implementation of Airplay server including screen mirroring and video streaming. And this project is written for cross-platform.

| Platform | Build Status |
| --- |  :---: |
| **Windows** | [![Build on Windows](https://github.com/air-display/apsdk/actions/workflows/build-windows.yml/badge.svg)](https://github.com/air-display/apsdk/actions/workflows/build-windows.yml) |
| **Android** | [![Build on Android](https://github.com/air-display/apsdk/actions/workflows/build-android.yml/badge.svg)](https://github.com/air-display/apsdk/actions/workflows/build-android.yml) |
| **macOS**   | [![Build on macOS](https://github.com/air-display/apsdk/actions/workflows/build-macos.yml/badge.svg)](https://github.com/air-display/apsdk/actions/workflows/build-macos.yml) |
| **iOS**     | [![Build on iOS](https://github.com/air-display/apsdk/actions/workflows/build-ios.yml/badge.svg)](https://github.com/air-display/apsdk/actions/workflows/build-ios.yml) |
| **Linux**   | [![Build on Linux](https://github.com/air-display/apsdk/actions/workflows/build-linux.yml/badge.svg)](https://github.com/air-display/apsdk/actions/workflows/build-linux.yml) |

# About fairplay
This project includes a submodule named [fairplay](https://github.com/air-display/fairplay) located in [third-party\fairplay](third-party\fairplay). Because the [fairplay](https://github.com/air-display/fairplay) project is currently private, if you perform submodule updating, you will get permission errors. Actually you can build apsdk project without this submodule (obviously it will not work because apsdk will use empty implementation).

# Windows
## Dependencies: 
No extra dependencies.

## Build instruction:
Run the generate_vs_proj.bat to generate the project files. Build the generated solutions and collect the output static library. 

## Runtime requirements
Make sure the Bonjour Service is installed, this is required by APS sdk. Download the runtime library from: https://developer.apple.com/bonjour/

# macOS & iOS
## Dependencies:
No extra dependencies.

## Build instructions:
Run the generate_xcode_proj.bat to generate the project files. Build the xCode project.

## Runtime requirements
With system build-in Bounjour service installed, no extra runtime requirements.


# Android
## Dependencies:
No extra dependencies.

## Build instructions:
Open the project folder with Android Studio and build the airplay module. The output is AAR library.

## Runtime requirements
APS will use the system build-in Bonjour service, no need to install any extra library.

# Linux
## Dependencies:
On Linux system, you need to install libavahi-compat-libdnssd-dev first. For example, on Ubuntu just run the following command before build:
```
sudo apt-get install libavahi-compat-libdnssd-dev
```

## Build instructions:
Run the generate_linux_proj.bat to generate the GUN make files.

## Runtime requirements
[avahi-packages](https://launchpad.net/ubuntu/+source/avahi) are needed, at least the following pacakges are installed:
- avahi-daemon
- libavahi-compat-libdnssd1
