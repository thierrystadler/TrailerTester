#pragma once

#include <Arduino.h>

#include "Relays.h"
#include "TestSequence.h"

enum class Mode {
  ConstantPower,
  LightingTest,
};

class StateMachine {
 public:
  StateMachine(Relays& relays);

  void setMode(Mode mode);
  Mode mode() const { return mode_; }

  void nextStep();
  void setTestStep(TestStep step);
  TestStep step() const { return step_; }

  void update();

  void setIndicatorTiming(uint32_t periodMs, uint32_t onMs);

  void setRelay(uint8_t index, bool on);

 private:
  Relays& relays_;
  Mode mode_ = Mode::LightingTest;
  TestStep step_ = TestStep::Tail;

  uint32_t indicatorPeriodMs_ = 1000;
  uint32_t indicatorOnMs_ = 500;
  uint32_t blinkPhaseStartMs_ = 0;

  void applyOutputs_();
  void setTestOutputs_(bool blinkOn);
};
