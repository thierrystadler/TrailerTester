#include "BluetoothHandler.h"

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

static String rxBuffer_;

BluetoothHandler::BluetoothHandler(StateMachine& sm, const char* deviceName)
    : sm_(sm), deviceName_(deviceName) {}

void BluetoothHandler::begin() {
  line_.reserve(64);

  BLEDevice::init(deviceName_);
  pServer_ = BLEDevice::createServer();
  pServer_->setCallbacks(this);

  BLEService* pService = pServer_->createService(SERVICE_UUID);

  pTxCharacteristic_ = pService->createCharacteristic(
      CHARACTERISTIC_TX,
      BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic_->addDescriptor(new BLE2902());

  BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_RX,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pRxCharacteristic->setCallbacks(this);

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();

  Serial.print("BLE started: ");
  Serial.println(deviceName_);
}

bool BluetoothHandler::isConnected() { return connected_; }

void BluetoothHandler::onConnect(BLEServer* pServer) {
  connected_ = true;
  Serial.println("BLE client connected");
}

void BluetoothHandler::onDisconnect(BLEServer* pServer) {
  connected_ = false;
  Serial.println("BLE client disconnected");
  BLEDevice::startAdvertising();
}

void BluetoothHandler::onWrite(BLECharacteristic* pCharacteristic) {
  String value = pCharacteristic->getValue().c_str();
  if (value.length() > 0) {
    rxBuffer_ += value;
  }
}

void BluetoothHandler::println(const String& msg) {
  if (!connected_ || pTxCharacteristic_ == nullptr) return;

  String out = msg + "\n";
  const size_t mtu = 20;
  for (size_t i = 0; i < out.length(); i += mtu) {
    String chunk = out.substring(i, i + mtu);
    pTxCharacteristic_->setValue(chunk.c_str());
    pTxCharacteristic_->notify();
    delay(10);
  }
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
      println("OK MODE POWER");
      return;
    }
    if (arg == "TEST" || arg == "LIGHT") {
      sm_.setMode(Mode::LightingTest);
      println("OK MODE TEST");
      return;
    }
    println("ERR INVALID_MODE");
    return;
  }

  if (cmd == "NEXT") {
    sm_.nextStep();
    println("OK NEXT");
    return;
  }

  if (cmd == "LIGHT") {
    if (sm_.mode() != Mode::LightingTest) {
      println("ERR NOT_IN_TEST_MODE");
      return;
    }

    String arg = nextToken_(rest);
    arg.toUpperCase();

    if (arg == "TAIL") {
      sm_.setTestStep(TestStep::Tail);
      println("OK LIGHT TAIL");
      return;
    }
    if (arg == "BRAKE") {
      sm_.setTestStep(TestStep::Brake);
      println("OK LIGHT BRAKE");
      return;
    }
    if (arg == "LEFT" || arg == "LEFTIND" || arg == "LEFT_IND") {
      sm_.setTestStep(TestStep::LeftIndicator);
      println("OK LIGHT LEFT");
      return;
    }
    if (arg == "RIGHT" || arg == "RIGHTIND" || arg == "RIGHT_IND") {
      sm_.setTestStep(TestStep::RightIndicator);
      println("OK LIGHT RIGHT");
      return;
    }
    if (arg == "REVERSE") {
      sm_.setTestStep(TestStep::Reverse);
      println("OK LIGHT REVERSE");
      return;
    }
    if (arg == "FOG" || arg == "REARFOG" || arg == "REAR_FOG") {
      sm_.setTestStep(TestStep::RearFog);
      println("OK LIGHT FOG");
      return;
    }

    println("ERR INVALID_LIGHT");
    return;
  }

  if (cmd == "RELAY") {
    String indexStr = nextToken_(rest);
    String stateStr = nextToken_(rest);
    stateStr.toUpperCase();

    int index = indexStr.toInt();
    if (index < 0 || index >= 8) {
      println("ERR INVALID_RELAY");
      return;
    }

    bool state = false;
    if (stateStr == "ON" || stateStr == "1") {
      state = true;
    } else if (stateStr == "OFF" || stateStr == "0") {
      state = false;
    } else {
      println("ERR INVALID_STATE");
      return;
    }

    sm_.setRelay(index, state);
    println("OK RELAY " + String(index) + " " + (state ? "ON" : "OFF"));
    return;
  }

  if (cmd == "STATUS") {
    println("MODE: " + String(sm_.mode() == Mode::ConstantPower ? "POWER" : "TEST"));
    println("STEP: " + String(static_cast<int>(sm_.step())));
    println("OK");
    return;
  }

  if (cmd == "HELP") {
    println("Commands:");
    println("  MODE POWER|TEST - Set mode");
    println("  LIGHT TAIL|BRAKE|LEFT|RIGHT|REVERSE|FOG");
    println("  NEXT - Next test step");
    println("  RELAY <0-7> ON|OFF - Control relay");
    println("  STATUS - Get current status");
    println("  HELP - Show this help");
    println("OK");
    return;
  }

  println("ERR UNKNOWN_CMD");
}

void BluetoothHandler::update() {
  if (rxBuffer_.length() == 0) return;

  String buf = rxBuffer_;
  rxBuffer_ = "";

  for (size_t i = 0; i < buf.length(); i++) {
    char c = buf[i];
    if (c == '\n' || c == '\r') {
      if (line_.length() > 0) {
        handleLine_(line_);
        line_ = "";
      }
    } else {
      line_ += c;
      if (line_.length() > 128) {
        line_ = "";
        println("ERR LINE_TOO_LONG");
      }
    }
  }
}
