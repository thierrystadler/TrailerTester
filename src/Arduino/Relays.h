#pragma once

#include <Arduino.h>

class Relays {
 public:
  Relays(const uint8_t* pins, uint8_t count, bool activeLow);

  void begin();

  void set(uint8_t index, bool on);
  void setAll(bool on);
  void allOff();

  uint8_t count() const { return count_; }

 private:
  const uint8_t* pins_;
  uint8_t count_;
  bool activeLow_;

  void write_(uint8_t pin, bool on);
};
