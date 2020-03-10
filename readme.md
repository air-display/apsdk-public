README
===========================
APS(Airplay Server) is an complete implementation of Airplay server including screen mirroring and video streaming. And this project is written for cross-platform.

| Platform | Master |
| --- |  :---: |
| **Windows** |  [![Build status](https://dev.azure.com/sheentian/T-Work/_apis/build/status/AirplayServerSDK-Build-Windows)](https://dev.azure.com/sheentian/T-Work/_build/latest?definitionId=20) |
| **Android** | [![Build status](https://dev.azure.com/sheentian/T-Work/_apis/build/status/AirplayServerSDK-Build-Android)](https://dev.azure.com/sheentian/T-Work/_build/latest?definitionId=21) |
| **macOS** | [![Build status](https://dev.azure.com/sheentian/T-Work/_apis/build/status/AirplayServerSDK-Build-macOS)](https://dev.azure.com/sheentian/T-Work/_build/latest?definitionId=22) |
| **Linux** |[![Build status](https://dev.azure.com/sheentian/T-Work/_apis/build/status/AirplayServerSDK-Build-Linux)](https://dev.azure.com/sheentian/T-Work/_build/latest?definitionId=23)|

# Windows
## Dependencies: 
No extra dependencies.

## Build instruction:
Run the generate_vs_proj.bat to generate the project files. Build the generated solutions and collect the output static library. 

## Runtime requirements
Make sure the Bonjour Service is installed, this is required by APS sdk. Download the runtime library from: https://developer.apple.com/bonjour/

# macOS
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
Run the generate_linux_proj.bat to generate the project files. Build the xCode project.

## Runtime requirements
[avahi-packages](https://launchpad.net/ubuntu/+source/avahi) are needed, at least the following pacakges are installed:
- avahi-daemon
- libavahi-compat-libdnssd1
