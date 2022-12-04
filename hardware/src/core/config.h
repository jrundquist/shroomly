#pragma once

#include "../core/storage.h"
#include <ArduinoJson.h>
#include <Preferences.h>

#define WIFI_SSID_KEY "ssid"
#define WIFI_PASS_KEY "pass"

class Config
{
public:
  void init();
  void clear();

  bool hasWifiCredentials();
  std::pair<String, String> getWifiCredentials();
  void setWifiCredentials(const char *ssid, const char *password);
  void clearWifiCredentials();

private:
  Preferences preferences;
};

extern Config config;