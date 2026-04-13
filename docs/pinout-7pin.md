# 7-pin Trailer Connector (ISO 1724) Pinout

This document lists the typical **7-pin (ISO 1724)** trailer connector pinout.

**Important:** Always verify your local standard, the socket/wiring on your vehicle/trailer, and the orientation/numbering of your specific connector.

## Pin table

| Pin | ISO function (typical) | Signal name (common) | Notes | Your channel / Arduino pin |
|---:|---|---|---|---|
| 1 | Left turn indicator | L | Yellow | |
| 2 | Rear fog lamp | 54G | Blue | (Sometimes used differently in some regions) | |
| 3 | Ground / return | 31 | White | Common ground for pins 1,2,4,5,6,7 | |
| 4 | Right turn indicator | R | Green | | |
| 5 | Right tail / side light | 58R | Brown | | |
| 6 | Brake lights | 54 | Red | | |
| 7 | Left tail / side light | 58L | Black | | |

## Notes for a tester build

- If you are switching lamps electronically, use automotive-rated **MOSFETs/relays** and add **fuses**.
- Keep grounds robust: ground issues are the most common cause of “weird” trailer light behavior.
- If you provide a separate **12V constant output**, document clearly how it is fused and where it is connected.
