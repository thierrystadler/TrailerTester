#include <Arduino.h>

#include <AUnit.h>

#include "../../../src/TrailerTesterESP32/TestSequence.h"

using namespace aunit;

test(next_step_cycles) {
  TestStep s = TestStep::Tail;
  s = nextTestStep(s);
  assertEqual(static_cast<int>(s), static_cast<int>(TestStep::Brake));
  s = nextTestStep(s);
  assertEqual(static_cast<int>(s), static_cast<int>(TestStep::LeftIndicator));
  s = nextTestStep(s);
  assertEqual(static_cast<int>(s), static_cast<int>(TestStep::RightIndicator));
  s = nextTestStep(s);
  assertEqual(static_cast<int>(s), static_cast<int>(TestStep::Reverse));
  s = nextTestStep(s);
  assertEqual(static_cast<int>(s), static_cast<int>(TestStep::RearFog));
  s = nextTestStep(s);
  assertEqual(static_cast<int>(s), static_cast<int>(TestStep::Tail));
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {
  TestRunner::run();
}
