#pragma once

#include <Arduino.h>

namespace cfg {

static constexpr uint32_t SERIAL_BAUD = 115200;

static constexpr bool RELAY_ACTIVE_LOW = true;

static constexpr uint8_t RELAY_COUNT = 8;

static constexpr uint8_t RELAY_PINS[RELAY_COUNT] = {
    23, 22, 21, 19, 18, 17, 16, 4,
};

static constexpr uint8_t MODE_BUTTON_PIN = 32;
static constexpr bool MODE_BUTTON_PULLUP = true;
static constexpr uint8_t MODE_BUTTON_LED_PIN = 25;

static constexpr uint8_t NEXT_BUTTON_PIN = 33;
static constexpr bool NEXT_BUTTON_PULLUP = true;

static constexpr uint32_t BUTTON_DEBOUNCE_MS = 40;

static constexpr uint32_t INDICATOR_PERIOD_MS = 1000;
static constexpr uint32_t INDICATOR_ON_MS = 500;

static constexpr const char* BT_DEVICE_NAME = "TrailerTester";

}  // namespace cfg
