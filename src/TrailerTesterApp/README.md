# Trailer Tester App

.NET MAUI mobile application for controlling the TrailerTester ESP32 device via Bluetooth.

## Features

- **Bluetooth Connection**: Scan and connect to TrailerTester devices
- **Mode Switching**: Toggle between Power Mode and Test Mode
- **Light Control**: Control individual trailer lights (Tail, Brake, Indicators, Reverse, Fog)
- **Real-time Feedback**: View command responses and status messages
- **Cross-Platform**: Runs on Android and iOS

## Requirements

- .NET 8.0 or later
- Visual Studio 2022 or Visual Studio Code with .NET MAUI workload
- Android SDK (for Android deployment)
- Xcode (for iOS deployment on macOS)

## Setup

1. Ensure the TrailerTester ESP32 device is powered on and Bluetooth is enabled
2. Build and deploy the app to your Android or iOS device
3. Grant Bluetooth and Location permissions when prompted

## Usage

### Connecting to Device

1. Tap "Scan for Devices"
2. Select "TrailerTester" from the list of discovered devices
3. Wait for connection confirmation

### Mode Selection

- **Test Mode**: Allows individual light control and testing
- **Power Mode**: Provides constant power supply

### Light Control (Test Mode Only)

Select individual lights:
- **Tail**: Tail lights
- **Brake**: Brake lights
- **Left/Right Indicator**: Turn signals
- **Reverse**: Reverse lights
- **Fog**: Rear fog lights

Use "Next Step" to cycle through all lights sequentially.

## Bluetooth Commands

The app sends the following commands to the ESP32:

- `MODE POWER` - Switch to power mode
- `MODE TEST` - Switch to test mode
- `LIGHT <type>` - Activate specific light (TAIL, BRAKE, LEFT, RIGHT, REVERSE, FOG)
- `NEXT` - Next test step

## Permissions

### Android
- BLUETOOTH
- BLUETOOTH_ADMIN
- BLUETOOTH_SCAN
- BLUETOOTH_CONNECT
- ACCESS_FINE_LOCATION
- ACCESS_COARSE_LOCATION

### iOS
- NSBluetoothAlwaysUsageDescription
- NSBluetoothPeripheralUsageDescription

## Troubleshooting

**Cannot find device:**
- Ensure Bluetooth is enabled on your phone
- Check that the ESP32 is powered on
- Grant location permissions (required for Bluetooth scanning on Android)

**Connection fails:**
- Try restarting the ESP32 device
- Ensure no other device is connected to the TrailerTester
- Check Bluetooth permissions are granted

**Commands not working:**
- Verify connection status
- Check the Messages log for error responses
- Ensure you're in the correct mode (Test/Power)

## Architecture

- **Services**: `BluetoothService` handles BLE communication using Plugin.BLE
- **ViewModels**: `MainViewModel` manages UI state and command logic
- **Views**: `MainPage` provides the user interface
- **Converters**: `InvertedBoolConverter` for UI binding logic

## Dependencies

- Plugin.BLE (3.1.0) - Cross-platform Bluetooth Low Energy library
