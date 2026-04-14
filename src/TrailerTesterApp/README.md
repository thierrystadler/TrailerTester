# Trailer Tester App

.NET MAUI mobile app (Android & iOS) for controlling the TrailerTester ESP32 via Bluetooth.

## Features

- **Bluetooth connection**: Scan and connect to TrailerTester devices
- **Mode switching**: Toggle between constant power and lighting test mode
- **Light control**: Select individual trailer lights (only one active at a time)
- **Real-time feedback**: View command responses and status messages from the ESP32
- **Cross-platform**: Runs on Android and iOS

## Requirements

- .NET 10.0 or later
- Visual Studio 2022+ or VS Code with .NET MAUI workload
- Android SDK (for Android deployment)
- Xcode (for iOS deployment, macOS only)

## Setup

1. Power on the TrailerTester ESP32 (Bluetooth enabled)
2. Build and deploy the app to your Android or iOS device
3. Grant Bluetooth and location permissions when prompted

## Usage

### Connecting to a device

1. Tap "Scan for Devices"
2. Select "TrailerTester" from the discovered devices list
3. Wait for connection confirmation

### Mode selection

- **Test Mode**: Control individual lighting circuits
- **Power Mode**: Constant 12V supply at the trailer connector

### Light control (Test Mode only)

Select a single light (only one active at a time):

- **Tail** — Tail / parking lights
- **Brake** — Brake lights
- **Left Indicator** — Left turn signal
- **Right Indicator** — Right turn signal
- **Reverse** — Reverse light
- **Rear Fog** — Rear fog light

Use "Next Step" to cycle through all lights sequentially.

## Bluetooth commands

The app sends the following commands to the ESP32:

| Command | Description |
|---|---|
| `MODE POWER` | Activate constant power mode |
| `MODE TEST` | Activate lighting test mode |
| `LIGHT TAIL` | Turn on tail lights |
| `LIGHT BRAKE` | Turn on brake lights |
| `LIGHT LEFT` | Turn on left indicator |
| `LIGHT RIGHT` | Turn on right indicator |
| `LIGHT REVERSE` | Turn on reverse light |
| `LIGHT FOG` | Turn on rear fog light |
| `NEXT` | Next test step |
| `STATUS` | Query current status |

## Permissions

### Android

- `BLUETOOTH`, `BLUETOOTH_ADMIN`
- `BLUETOOTH_SCAN`, `BLUETOOTH_CONNECT`
- `ACCESS_FINE_LOCATION`, `ACCESS_COARSE_LOCATION`

### iOS

- `NSBluetoothAlwaysUsageDescription`
- `NSBluetoothPeripheralUsageDescription`

## Troubleshooting

- **Device not found**: Is Bluetooth enabled? Is the ESP32 powered on? Location permission granted (required on Android)?
- **Connection failed**: Restart the ESP32, ensure no other device is connected
- **Commands not working**: Check connection status, review the messages log, verify you are in the correct mode

## Architecture

- **Services/** — `BluetoothService`: BLE communication via Plugin.BLE
- **ViewModels/** — `MainViewModel`: UI state and command logic
- **MainPage.xaml** — User interface
- **Converters/** — `InvertedBoolConverter` for UI bindings

## Dependencies

- [Plugin.BLE](https://github.com/dotnet-bluetooth-le/dotnet-bluetooth-le) 3.1.0
