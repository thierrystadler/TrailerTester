#include <Arduino.h>

#include "BluetoothHandler.h"
#include "Button.h"
#include "Commands.h"
#include "Relays.h"
#include "StateMachine.h"
#include "config.h"

static Relays relays(cfg::RELAY_PINS, cfg::RELAY_COUNT, cfg::RELAY_ACTIVE_LOW);
static Button nextButton(cfg::NEXT_BUTTON_PIN, cfg::NEXT_BUTTON_PULLUP,
                         cfg::BUTTON_DEBOUNCE_MS);
static StateMachine sm(relays);
static Commands commands(sm);
static BluetoothHandler bluetooth(sm, cfg::BT_DEVICE_NAME);

static bool readModeSwitch() {
  const int v = digitalRead(cfg::MODE_SWITCH_PIN);
  if (cfg::MODE_SWITCH_PULLUP) {
    return v == LOW;
  }
  return v == HIGH;
}

void setup() {
  Serial.begin(cfg::SERIAL_BAUD);

  if (cfg::MODE_SWITCH_PULLUP) {
    pinMode(cfg::MODE_SWITCH_PIN, INPUT_PULLUP);
  } else {
    pinMode(cfg::MODE_SWITCH_PIN, INPUT);
  }

  relays.begin();
  nextButton.begin();

  sm.setIndicatorTiming(cfg::INDICATOR_PERIOD_MS, cfg::INDICATOR_ON_MS);

  commands.begin();
  bluetooth.begin();
}

void loop() {
  commands.update();
  bluetooth.update();

  const bool constantModeSelected = readModeSwitch();
  sm.setMode(constantModeSelected ? Mode::ConstantPower : Mode::LightingTest);

  nextButton.update();
  if (nextButton.pressedEvent()) {
    sm.nextStep();
  }

  sm.update();
}
