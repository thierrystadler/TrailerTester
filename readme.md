
# Trailer Tester (Arduino + 18V Battery)

Trailer Tester is an Arduino-based device for quickly testing trailer lighting on trailers with **7-pin** or **13-pin** connectors.

It can also provide a **12V constant output** (depending on your power electronics) to test auxiliary functions such as lights, a water pump, or other 12V trailer consumers.

## What this project does

- Tests common trailer light circuits (e.g. left/right indicator, brake, tail, reverse, fog — depending on connector type).
- Allows switching outputs on/off for verification.
- Provides a 12V "always on" supply output (if enabled in your build) for functional testing.
- **Bluetooth control** — connect from the companion mobile app to switch modes and control individual lights.
- **Mobile app** (.NET MAUI) for Android & iOS to remotely operate the tester.
- Designed to be powered from an **18V battery pack** (e.g. a common tool battery) using a suitable DC/DC converter.

## Supported connectors

- **7-pin (ISO 1724)**
- **13-pin (ISO 11446)**

## Hardware overview

- **Wemos D1 Mini ESP32**
- **8-channel relay board**
- 18V battery pack (e.g. common cordless-tool battery)
- DC/DC buck converter **18V → 12V** (sized for your expected current)
- DC/DC buck converter or regulator **12V → 5V** (to power the ESP32)
- **Mode switch** — toggles between *constant 12V power* and *lighting test* mode
- **Step button** — advances to the next lighting circuit during testing
- Fuse(s) on the 12V side (strongly recommended)
- 7-pin and/or 13-pin trailer socket
- Enclosure, wiring, strain relief

## Wiring / pinout

**Important:** Pin assignments can vary by region/application and mistakes can damage equipment.
Verify the ISO standard and your connector/socket wiring before powering anything.

Recommended: document your mapping in separate files, e.g.:

- `docs/pinout-7pin.md`
- `docs/pinout-13pin.md`

Document at least:

- Which trailer pin is connected to which output channel
- Which Arduino pin controls that channel
- Which channels are “switched” vs “constant 12V”

## How to use

1. Connect the trailer plug to the tester's 7-pin or 13-pin socket.
2. Power the tester using the 18V battery.
3. Use the **mode switch** to select:
   - **Constant power** — relay 1 provides 12V permanently (for pumps, lights, etc.)
   - **Lighting test** — step through each circuit one at a time
4. In lighting-test mode, press the **step button** to cycle through:
   - Tail / parking lights
   - Brake lights
   - Left indicator (blinks at normal cycle)
   - Right indicator (blinks at normal cycle)
   - Reverse light
   - Rear fog light
5. Only one circuit is active at a time to keep battery load low.

## Safety notes

- Use appropriately rated wiring, connectors, and **fuses**.
- Trailer circuits can draw significant current (especially incandescent bulbs or pumps).
- Do not power unknown loads without current limiting / fusing.
- Prevent short circuits: secure all wiring, use an enclosure, and add strain relief.
- Ensure your DC/DC converter(s) are rated for the maximum current you will draw.

## Project structure

```bash
src/
  TrailerTesterESP32/          Arduino firmware (ESP32)
    TrailerTesterESP32.ino     Entry point (setup / loop)
    config.h                   Pin assignments, timing, constants
    Relays.h / .cpp            Relay driver abstraction
    Button.h / .cpp            Debounced button input
    StateMachine.h / .cpp      Mode & test-step logic
    TestSequence.h             Pure step-sequencing (shared with tests)
    Commands.h / .cpp          Serial command interface
    BluetoothHandler.h / .cpp  Bluetooth Serial command interface
  TrailerTesterApp/            .NET MAUI mobile app (Android & iOS)
    Services/                  Bluetooth communication service
    ViewModels/                UI state and command logic
    MainPage.xaml              User interface
tests/
  host/
    test_sequence.cpp          Host-side unit test (g++, runs in CI)
  esp32/
    SequenceAUnit/             AUnit test sketch (runs on ESP32 hardware)
docs/
  pinout-7pin.md               7-pin connector pinout (ISO 1724)
  pinout-13pin.md              13-pin connector pinout (ISO 11446)
```

## CI / GitHub Actions

Every push and pull request triggers three jobs:

- **build-esp32** — compiles the main sketch for Wemos D1 Mini ESP32 (`esp32:esp32:d1_mini32`)
- **host-tests** — builds and runs the host-side unit tests with `g++`
- **build-esp32-aunit** — compiles the AUnit test sketch for ESP32

## Mobile app

The companion **TrailerTesterApp** (under `src/TrailerTesterApp/`) is a .NET MAUI app for Android and iOS.
It connects to the ESP32 via Bluetooth and allows:

- Switching between **Power Mode** and **Test Mode**
- Selecting individual lights (Tail, Brake, Left/Right Indicator, Reverse, Rear Fog)
- Viewing real-time status and command responses

See [`src/TrailerTesterApp/README.md`](src/TrailerTesterApp/README.md) for details.

## Disclaimer

This project is provided "as is". You are responsible for verifying wiring, electrical ratings, and safe operation.
The authors/contributors are not liable for damage or injury resulting from the use of this design.
