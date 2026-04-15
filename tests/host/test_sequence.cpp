#include <cassert>

#include "../../src/TrailerTesterESP32/TestSequence.h"

int main() {
  TestStep s = TestStep::Off;
  s = nextTestStep(s);
  assert(s == TestStep::Tail);
  s = nextTestStep(s);
  assert(s == TestStep::Brake);
  s = nextTestStep(s);
  assert(s == TestStep::LeftIndicator);
  s = nextTestStep(s);
  assert(s == TestStep::RightIndicator);
  s = nextTestStep(s);
  assert(s == TestStep::Reverse);
  s = nextTestStep(s);
  assert(s == TestStep::RearFog);
  s = nextTestStep(s);
  assert(s == TestStep::Off);

  return 0;
}
