#pragma once

#include <Arduino.h>

class Button {
 public:
  Button(uint8_t pin, bool usePullup, uint32_t debounceMs);

  void begin();
  void update();

  bool pressedEvent() const;

 private:
  uint8_t pin_;
  bool usePullup_;
  uint32_t debounceMs_;

  bool stableState_ = false;
  bool lastRaw_ = false;
  uint32_t lastChangeMs_ = 0;

  bool pressedEvent_ = false;

  bool readRaw_() const;
};
