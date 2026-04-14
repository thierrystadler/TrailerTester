# TrailerTester ESP32

Arduino firmware for the ESP32-based trailer lighting tester.

## Features

- **Constant power mode**: Permanent 12V supply at the trailer connector
- **Lighting test mode**: Test individual lighting circuits one at a time
- **Bluetooth**: Control via Bluetooth Serial (Classic) from the companion TrailerTesterApp
- **Serial interface**: Control via USB serial (same commands as Bluetooth)
- **Hardware button**: Physical "Next" button to step through test sequences
- **Hardware switch**: Toggle between constant power and lighting test mode

## Hardware

### Pin assignment

| Pin | Function |
|-----|----------|
| 23 | Relay 0 — Mode (constant power) |
| 22 | Relay 1 — Tail lights |
| 21 | Relay 2 — Brake lights |
| 19 | Relay 3 — Left indicator |
| 18 | Relay 4 — Right indicator |
| 17 | Relay 5 — Reverse light |
| 16 | Relay 6 — Rear fog light |
| 4  | Relay 7 — Aux |
| 32 | Mode switch (INPUT_PULLUP) |
| 33 | Next button (INPUT_PULLUP) |

### Relays

- **8-channel relay module**, active low
- Relay 0 switches between constant power and test mode
- Relays 1–7 control the individual lighting circuits

## Bluetooth commands

Connects as Bluetooth Serial device **"TrailerTester"**.

| Command | Description |
|---|---|
| `MODE POWER` / `MODE CONST` | Constant power mode |
| `MODE TEST` / `MODE LIGHT` | Lighting test mode |
| `LIGHT TAIL` | Turn on tail lights |
| `LIGHT BRAKE` | Turn on brake lights |
| `LIGHT LEFT` | Turn on left indicator |
| `LIGHT RIGHT` | Turn on right indicator |
| `LIGHT REVERSE` | Turn on reverse light |
| `LIGHT FOG` | Turn on rear fog light |
| `NEXT` | Next test step |
| `RELAY <0-7> ON\|OFF` | Control a single relay directly |
| `STATUS` | Query current mode and test step |
| `HELP` | Show help |

During lighting test, only one lighting circuit is active at a time.

## Serial commands (USB)

Same commands as Bluetooth, via USB serial at **115200 baud**.

## Modules

| File | Description |
|---|---|
| `TrailerTesterESP32.ino` | Main program (setup / loop) |
| `config.h` | Pin definitions and configuration |
| `StateMachine.h/.cpp` | State machine (mode, test steps, relays) |
| `Relays.h/.cpp` | Relay driver abstraction |
| `Button.h/.cpp` | Debounced button input |
| `Commands.h/.cpp` | Serial command processing |
| `BluetoothHandler.h/.cpp` | Bluetooth Serial command processing |
| `TestSequence.h` | Test step sequence |

## Build

1. Arduino IDE or PlatformIO
2. Board: ESP32 Dev Module
3. Upload sketch
