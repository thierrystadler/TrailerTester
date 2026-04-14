#pragma once

#include <Arduino.h>
#include <BluetoothSerial.h>

#include "StateMachine.h"

class BluetoothHandler {
 public:
  BluetoothHandler(StateMachine& sm, const char* deviceName);

  void begin();
  void update();

  bool isConnected() const;

 private:
  StateMachine& sm_;
  BluetoothSerial bt_;
  const char* deviceName_;
  String line_;

  void handleLine_(const String& line);
  String nextToken_(String& s);
};
