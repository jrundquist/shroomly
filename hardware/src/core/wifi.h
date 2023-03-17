

#pragma once

#include <WiFi.h>
#include "../include/common.h"
#include "../core/status_pixel.h"
#include "../core/config.h"

// Includes for BLE
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEAdvertising.h>

#include <ArduinoJson.h>

// List of Service UUIDs
#define SERVICE_UUID "00010ad1-b808-499e-9379-c9ae7c86611e"
// Characteristic UUIDs
#define STATUS_UUID "00001111-b808-499e-9379-c9ae7c86611e"
#define WIFI_UUID "00005555-b808-499e-9379-c9ae7c86611e"

#define MAX_CONNECTION_ATTEMPTS 10
class Wifi
{
public:
  void init();
  void startAP();
  void stopAP();
  void connect();
  void disconnect();

  int getMode();
  int getRSSI();
  int getRSSI(int n);
  float getQuality();
  int getStatus();

  int scanNetworks();
  int scanNetworks(bool async);
  int scanComplete();
  void scanDelete();
  int getChannel(int n);
  int getEncryptionType(int n);

  void startBluetoothPairing();

  String getSSID();
  String getSSID(int n);
  String getBSSIDstr(int n);
  IPAddress getLocalIp();

  void loop();
};

extern Wifi wifi;