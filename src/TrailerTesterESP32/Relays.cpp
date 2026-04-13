#include "Relays.h"

Relays::Relays(const uint8_t* pins, uint8_t count, bool activeLow)
    : pins_(pins), count_(count), activeLow_(activeLow) {}

void Relays::begin() {
  for (uint8_t i = 0; i < count_; i++) {
    pinMode(pins_[i], OUTPUT);
    write_(pins_[i], false);
  }
}

void Relays::write_(uint8_t pin, bool on) {
  if (activeLow_) {
    digitalWrite(pin, on ? LOW : HIGH);
  } else {
    digitalWrite(pin, on ? HIGH : LOW);
  }
}

void Relays::set(uint8_t index, bool on) {
  if (index >= count_) return;
  write_(pins_[index], on);
}

void Relays::setAll(bool on) {
  for (uint8_t i = 0; i < count_; i++) {
    write_(pins_[i], on);
  }
}

void Relays::allOff() { setAll(false); }
