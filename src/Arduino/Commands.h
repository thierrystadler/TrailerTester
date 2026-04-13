#pragma once

#include <Arduino.h>

#include "StateMachine.h"

class Commands {
 public:
  explicit Commands(StateMachine& sm);

  void begin();
  void update();

 private:
  StateMachine& sm_;
  String line_;

  void handleLine_(const String& line);
  static String nextToken_(String& s);
};
