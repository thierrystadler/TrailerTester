#include "Button.h"

Button::Button(uint8_t pin, bool usePullup, uint32_t debounceMs)
    : pin_(pin), usePullup_(usePullup), debounceMs_(debounceMs) {}

void Button::begin() {
  if (usePullup_) {
    pinMode(pin_, INPUT_PULLUP);
  } else {
    pinMode(pin_, INPUT);
  }

  lastRaw_ = readRaw_();
  stableState_ = lastRaw_;
  lastChangeMs_ = millis();
}

bool Button::readRaw_() const {
  const int v = digitalRead(pin_);
  if (usePullup_) {
    return v == LOW;
  }
  return v == HIGH;
}

void Button::update() {
  pressedEvent_ = false;

  const bool raw = readRaw_();
  const uint32_t now = millis();

  if (raw != lastRaw_) {
    lastRaw_ = raw;
    lastChangeMs_ = now;
  }

  if ((now - lastChangeMs_) >= debounceMs_) {
    if (stableState_ != raw) {
      stableState_ = raw;
      if (stableState_) {
        pressedEvent_ = true;
      }
    }
  }
}

bool Button::pressedEvent() const { return pressedEvent_; }
