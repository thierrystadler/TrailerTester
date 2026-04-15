#include "StateMachine.h"

static constexpr uint8_t RELAY_MODE = 0;
static constexpr uint8_t RELAY_TAIL = 1;
static constexpr uint8_t RELAY_BRAKE = 2;
static constexpr uint8_t RELAY_LEFT_IND = 3;
static constexpr uint8_t RELAY_RIGHT_IND = 4;
static constexpr uint8_t RELAY_REVERSE = 5;
static constexpr uint8_t RELAY_REAR_FOG = 6;
static constexpr uint8_t RELAY_AUX = 7;

StateMachine::StateMachine(Relays& relays) : relays_(relays) {}

void StateMachine::setIndicatorTiming(uint32_t periodMs, uint32_t onMs) {
  indicatorPeriodMs_ = periodMs;
  indicatorOnMs_ = onMs;
}

void StateMachine::setRelay(uint8_t index, bool on) {
  relays_.set(index, on);
}

void StateMachine::setMode(Mode mode) {
  if (mode_ == mode) return;
  mode_ = mode;
  step_ = TestStep::Off;
  blinkPhaseStartMs_ = millis();
  applyOutputs_();
}

void StateMachine::nextStep() {
  if (mode_ != Mode::LightingTest) return;

  step_ = nextTestStep(step_);

  blinkPhaseStartMs_ = millis();
  applyOutputs_();
}

void StateMachine::setTestStep(TestStep step) {
  if (mode_ != Mode::LightingTest) return;

  step_ = step;

  blinkPhaseStartMs_ = millis();
  applyOutputs_();
}

void StateMachine::update() {
  const uint32_t now = millis();

  if (mode_ == Mode::ConstantPower) {
    relays_.allOff();
    relays_.set(RELAY_MODE, true);
    return;
  }

  const uint32_t phaseMs = (indicatorPeriodMs_ == 0)
                               ? 0
                               : ((now - blinkPhaseStartMs_) % indicatorPeriodMs_);
  const bool blinkOn = phaseMs < indicatorOnMs_;

  setTestOutputs_(blinkOn);
}

void StateMachine::applyOutputs_() {
  update();
}

void StateMachine::setTestOutputs_(bool blinkOn) {
  relays_.allOff();
  relays_.set(RELAY_MODE, false);

  switch (step_) {
    case TestStep::Off:
      break;
    case TestStep::Tail:
      relays_.set(RELAY_TAIL, true);
      break;
    case TestStep::Brake:
      relays_.set(RELAY_BRAKE, true);
      break;
    case TestStep::LeftIndicator:
      relays_.set(RELAY_LEFT_IND, blinkOn);
      break;
    case TestStep::RightIndicator:
      relays_.set(RELAY_RIGHT_IND, blinkOn);
      break;
    case TestStep::Reverse:
      relays_.set(RELAY_REVERSE, true);
      break;
    case TestStep::RearFog:
      relays_.set(RELAY_REAR_FOG, true);
      break;
  }
}
