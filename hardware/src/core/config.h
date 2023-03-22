#pragma once

#include "../core/storage.h"
#include <ArduinoJson.h>
#include <Preferences.h>

#define CONFIG_STATE_KEY "state"

#define PAIRING_INITIATOR_KEY "pi"
#define WIFI_SSID_KEY "ssid"
#define WIFI_PASS_KEY "pass"

enum ConfigFlags : uint32_t
{
  CONFIG_FLAG_RECEIVED_USER_INFO = 0b1,
  CONFIG_FLAG_WIFI_CONFIGURED = 0b10,
};

class Config
{
public:
  void init();
  void clear();

  // WiFi Information
  bool hasWifiCredentials();
  std::pair<String, String> getWifiCredentials();
  void setWifiCredentials(const char *ssid, const char *password);
  void clearWifiCredentials();

  // Pairing Information
  void setPairingInitiator(String pairingInitiator);
  String getPairingInitiator();

private:
  Preferences preferences;
};

extern Config config;