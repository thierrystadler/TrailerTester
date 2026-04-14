#include "BluetoothHandler.h"

BluetoothHandler::BluetoothHandler(StateMachine& sm, const char* deviceName)
    : sm_(sm), deviceName_(deviceName) {}

void BluetoothHandler::begin() {
  line_.reserve(64);
  if (!bt_.begin(deviceName_)) {
    Serial.println("Bluetooth init failed!");
  } else {
    Serial.print("Bluetooth started: ");
    Serial.println(deviceName_);
  }
}

bool BluetoothHandler::isConnected() const {
  return bt_.hasClient();
}

String BluetoothHandler::nextToken_(String& s) {
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

void BluetoothHandler::handleLine_(const String& line) {
  String rest = line;
  String cmd = nextToken_(rest);
  cmd.toUpperCase();

  if (cmd == "MODE") {
    String arg = nextToken_(rest);
    arg.toUpperCase();
    if (arg == "CONST" || arg == "POWER") {
      sm_.setMode(Mode::ConstantPower);
      bt_.println("OK MODE POWER");
      return;
    }
    if (arg == "TEST" || arg == "LIGHT") {
      sm_.setMode(Mode::LightingTest);
      bt_.println("OK MODE TEST");
      return;
    }
    bt_.println("ERR INVALID_MODE");
    return;
  }

  if (cmd == "NEXT") {
    sm_.nextStep();
    bt_.println("OK NEXT");
    return;
  }

  if (cmd == "LIGHT") {
    if (sm_.mode() != Mode::LightingTest) {
      bt_.println("ERR NOT_IN_TEST_MODE");
      return;
    }

    String arg = nextToken_(rest);
    arg.toUpperCase();

    if (arg == "TAIL") {
      sm_.setTestStep(TestStep::Tail);
      bt_.println("OK LIGHT TAIL");
      return;
    }
    if (arg == "BRAKE") {
      sm_.setTestStep(TestStep::Brake);
      bt_.println("OK LIGHT BRAKE");
      return;
    }
    if (arg == "LEFT" || arg == "LEFTIND" || arg == "LEFT_IND") {
      sm_.setTestStep(TestStep::LeftIndicator);
      bt_.println("OK LIGHT LEFT");
      return;
    }
    if (arg == "RIGHT" || arg == "RIGHTIND" || arg == "RIGHT_IND") {
      sm_.setTestStep(TestStep::RightIndicator);
      bt_.println("OK LIGHT RIGHT");
      return;
    }
    if (arg == "REVERSE") {
      sm_.setTestStep(TestStep::Reverse);
      bt_.println("OK LIGHT REVERSE");
      return;
    }
    if (arg == "FOG" || arg == "REARFOG" || arg == "REAR_FOG") {
      sm_.setTestStep(TestStep::RearFog);
      bt_.println("OK LIGHT FOG");
      return;
    }

    bt_.println("ERR INVALID_LIGHT");
    return;
  }

  if (cmd == "RELAY") {
    String indexStr = nextToken_(rest);
    String stateStr = nextToken_(rest);
    stateStr.toUpperCase();

    int index = indexStr.toInt();
    if (index < 0 || index >= 8) {
      bt_.println("ERR INVALID_RELAY");
      return;
    }

    bool state = false;
    if (stateStr == "ON" || stateStr == "1") {
      state = true;
    } else if (stateStr == "OFF" || stateStr == "0") {
      state = false;
    } else {
      bt_.println("ERR INVALID_STATE");
      return;
    }

    sm_.setRelay(index, state);
    bt_.print("OK RELAY ");
    bt_.print(index);
    bt_.print(" ");
    bt_.println(state ? "ON" : "OFF");
    return;
  }

  if (cmd == "STATUS") {
    bt_.print("MODE: ");
    bt_.println(sm_.mode() == Mode::ConstantPower ? "POWER" : "TEST");
    bt_.print("STEP: ");
    bt_.println(static_cast<int>(sm_.step()));
    bt_.println("OK");
    return;
  }

  if (cmd == "HELP") {
    bt_.println("Commands:");
    bt_.println("  MODE POWER|TEST - Set mode");
    bt_.println("  LIGHT TAIL|BRAKE|LEFT|RIGHT|REVERSE|FOG");
    bt_.println("    - Select specific light (TEST mode only)");
    bt_.println("  NEXT - Next test step");
    bt_.println("  RELAY <0-7> ON|OFF - Control relay");
    bt_.println("  STATUS - Get current status");
    bt_.println("  HELP - Show this help");
    bt_.println("OK");
    return;
  }

  bt_.println("ERR UNKNOWN_CMD");
}

void BluetoothHandler::update() {
  while (bt_.available()) {
    char c = bt_.read();
    if (c == '\n' || c == '\r') {
      if (line_.length() > 0) {
        handleLine_(line_);
        line_ = "";
      }
    } else {
      line_ += c;
      if (line_.length() > 128) {
        line_ = "";
        bt_.println("ERR LINE_TOO_LONG");
      }
    }
  }
}
