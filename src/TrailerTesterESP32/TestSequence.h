#pragma once

enum class TestStep {
  Off,
  Tail,
  Brake,
  LeftIndicator,
  RightIndicator,
  Reverse,
  RearFog,
};

constexpr TestStep nextTestStep(TestStep step) {
  switch (step) {
    case TestStep::Off:
      return TestStep::Tail;
    case TestStep::Tail:
      return TestStep::Brake;
    case TestStep::Brake:
      return TestStep::LeftIndicator;
    case TestStep::LeftIndicator:
      return TestStep::RightIndicator;
    case TestStep::RightIndicator:
      return TestStep::Reverse;
    case TestStep::Reverse:
      return TestStep::RearFog;
    case TestStep::RearFog:
      return TestStep::Off;
  }
  return TestStep::Off;
}
