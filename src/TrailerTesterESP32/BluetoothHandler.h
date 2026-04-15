#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "StateMachine.h"

class BluetoothHandler : public BLEServerCallbacks,
                         public BLECharacteristicCallbacks {
 public:
  BluetoothHandler(StateMachine& sm, const char* deviceName);

  void begin();
  void update();

  bool isConnected();

  void println(const String& msg);

  void onConnect(BLEServer* pServer) override;
  void onDisconnect(BLEServer* pServer) override;
  void onWrite(BLECharacteristic* pCharacteristic) override;

 private:
  StateMachine& sm_;
  const char* deviceName_;
  String line_;
  bool connected_ = false;

  BLEServer* pServer_ = nullptr;
  BLECharacteristic* pTxCharacteristic_ = nullptr;

  void handleLine_(const String& line);
  static String nextToken_(String& s);
};
