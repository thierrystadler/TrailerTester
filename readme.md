
# Trailer Tester (Arduino + 18V Battery)

Trailer Tester is an Arduino-based device for quickly testing trailer lighting on trailers with **7-pin** or **13-pin** connectors.

It can also provide a **12V constant output** (depending on your power electronics) to test auxiliary functions such as lights, a water pump, or other 12V trailer consumers.

## What this project does

- Tests common trailer light circuits (e.g. left/right indicator, brake, tail, reverse, fog — depending on connector type).
- Allows switching outputs on/off for verification.
- Provides a 12V “always on” supply output (if enabled in your build) for functional testing.
- Designed to be powered from an **18V battery pack** (e.g. a common tool battery) using a suitable DC/DC converter.

## Supported connectors

- **7-pin (ISO 1724)**
- **13-pin (ISO 11446)**

## Hardware overview (example)

Your exact parts may differ. Typical build:

- Arduino (e.g. Nano / Uno)
- 18V battery pack
- DC/DC buck converter **18V -> 12V** (sized for your expected current)
- Optional DC/DC buck converter **12V -> 5V** (or use a 5V regulator for the Arduino)
- Output drivers (recommended): automotive-rated MOSFETs or relays
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

- Connect the trailer plug to the tester’s 7-pin or 13-pin socket.
- Power the tester using the 18V battery.
- Use the controls/UI (depending on your implementation) to toggle circuits like:
  - Left indicator
  - Right indicator
  - Tail/parking
  - Brake
  - Reverse
  - Rear fog
- If your build supports it, enable **12V constant output** to test loads like a pump.

## Safety notes

- Use appropriately rated wiring, connectors, and **fuses**.
- Trailer circuits can draw significant current (especially incandescent bulbs or pumps).
- Do not power unknown loads without current limiting / fusing.
- Prevent short circuits: secure all wiring, use an enclosure, and add strain relief.
- Ensure your DC/DC converter(s) are rated for the maximum current you will draw.

## Disclaimer

This project is provided “as is”. You are responsible for verifying wiring, electrical ratings, and safe operation.
The authors/contributors are not liable for damage or injury resulting from the use of this design.

