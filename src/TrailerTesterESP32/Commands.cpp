#include "Commands.h"

Commands::Commands(StateMachine& sm) : sm_(sm) {}

void Commands::begin() {
  line_.reserve(64);
}

String Commands::nextToken_(String& s) {
  s.trim();
  if (s.length() == 0) return String();

  int idx = s.indexOf(' ');
  if (idx < 0) {
    String t = s;
    s = "";
    return t;
  }

  String t = s.substring(0, idx);
  s = s.substring(idx + 1);
  s.trim();
  return t;
}

void Commands::handleLine_(const String& line) {
  String rest = line;
  String cmd = nextToken_(rest);
  cmd.toUpperCase();

  if (cmd == "MODE") {
    String arg = nextToken_(rest);
    arg.toUpperCase();
    if (arg == "CONST") {
      sm_.setMode(Mode::ConstantPower);
      Serial.println("OK");
      return;
    }
    if (arg == "TEST") {
      sm_.setMode(Mode::LightingTest);
      Serial.println("OK");
      return;
    }
    Serial.println("ERR");
    return;
  }

  if (cmd == "NEXT") {
    sm_.nextStep();
    Serial.println("OK");
    return;
  }

  if (cmd == "RELAY") {
    String indexStr = nextToken_(rest);
    String stateStr = nextToken_(rest);
    stateStr.toUpperCase();

    int index = indexStr.toInt();
    if (index < 0 || index >= 8) {
      Serial.println("ERR");
      return;
    }

    bool state = false;
    if (stateStr == "ON" || stateStr == "1") {
      state = true;
    } else if (stateStr == "OFF" || stateStr == "0") {
      state = false;
    } else {
      Serial.println("ERR");
      return;
    }

    sm_.setRelay(index, state);
    Serial.println("OK");
    return;
  }

  Serial.println("ERR");
}

void Commands::update() {
  while (Serial.available() > 0) {
    const char c = static_cast<char>(Serial.read());

    if (c == '\n' || c == '\r') {
      if (line_.length() > 0) {
        handleLine_(line_);
        line_ = "";
      }
      continue;
    }

    if (line_.length() < 120) {
      line_ += c;
    }
  }
}
