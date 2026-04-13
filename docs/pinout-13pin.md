# 13-pin Trailer Connector (ISO 11446) Pinout

This document lists the typical **13-pin (ISO 11446)** trailer connector pinout.

**Important:** Always verify your local standard, the socket/wiring on your vehicle/trailer, and the orientation/numbering of your specific connector.

## Pin table

| Pin | ISO function (typical) | Signal name (common) | Notes | Your channel / Arduino pin |
|---:|---|---|---|---|
| 1 | Left turn indicator | L | Yellow | |
| 2 | Rear fog lamp | 54G | Blue | |
| 3 | Ground / return for pins 1–8 | 31 | White | Lighting ground | |
| 4 | Right turn indicator | R | Green | |
| 5 | Right tail / side light | 58R | Brown | |
| 6 | Brake lights | 54 | Red | |
| 7 | Left tail / side light | 58L | Black | |
| 8 | Reverse light(s) | 58b | Pink | |
| 9 | +12V permanent (battery) | 30 | Orange | Constant 12V supply (fused) | |
| 10 | +12V switched (ignition / charging) | 15 | Grey | Often used for fridge/charging (fused) | |
| 11 | Ground / return for pin 10 | 31 | White/Black | Ground for switched +12V | |
| 12 | Trailer presence / spare | — | Violet | Commonly used as optional / manufacturer-specific | |
| 13 | Ground / return for pin 9 | 31 | White/Red | Ground for permanent +12V | |

## Notes for a tester build

- Pins **9/13** are typically used for a **12V constant output** and its dedicated ground.
- Pins **10/11** are often used for switched supply; whether you support this is up to your design.
- Use fuses and wire gauge appropriate for your maximum load (pumps can draw high current).
