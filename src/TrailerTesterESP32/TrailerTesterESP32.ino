#include <Arduino.h>

#include "BluetoothHandler.h"
#include "Button.h"
#include "Commands.h"
#include "Relays.h"
#include "StateMachine.h"
#include "config.h"

static Relays relays(cfg::RELAY_PINS, cfg::RELAY_COUNT, cfg::RELAY_ACTIVE_LOW);
static Button modeButton(cfg::MODE_BUTTON_PIN, cfg::MODE_BUTTON_PULLUP,
                         cfg::BUTTON_DEBOUNCE_MS);
static Button nextButton(cfg::NEXT_BUTTON_PIN, cfg::NEXT_BUTTON_PULLUP,
                         cfg::BUTTON_DEBOUNCE_MS);
static StateMachine sm(relays);
static Commands commands(sm);
static BluetoothHandler bluetooth(sm, cfg::BT_DEVICE_NAME);

static void updateModeLed() {
  digitalWrite(cfg::MODE_BUTTON_LED_PIN,
               sm.mode() == Mode::ConstantPower ? HIGH : LOW);
}

void setup() {
  Serial.begin(cfg::SERIAL_BAUD);

  pinMode(cfg::MODE_BUTTON_LED_PIN, OUTPUT);
  digitalWrite(cfg::MODE_BUTTON_LED_PIN, LOW);

  relays.begin();
  modeButton.begin();
  nextButton.begin();

  sm.setIndicatorTiming(cfg::INDICATOR_PERIOD_MS, cfg::INDICATOR_ON_MS);

  commands.begin();
  bluetooth.begin();
}

void loop() {
  commands.update();
  bluetooth.update();

  modeButton.update();
  if (modeButton.pressedEvent()) {
    if (sm.mode() == Mode::LightingTest) {
      sm.setMode(Mode::ConstantPower);
    } else {
      sm.setMode(Mode::LightingTest);
    }
  }

  nextButton.update();
  if (nextButton.pressedEvent()) {
    sm.nextStep();
  }

  updateModeLed();
  sm.update();
}
