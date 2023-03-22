#include "config.h"

Config config;

void Config::init()
{
  auto ok = preferences.begin("shroomly-prefs", false);
  if (!ok)
  {
    Serial.println("  !!! Failed to begin preferences !!!");
  }
}

void Config::setPairingInitiator(String pairingInitiator)
{
  preferences.putString(PAIRING_INITIATOR_KEY, pairingInitiator.c_str());
}

String Config::getPairingInitiator()
{
  preferences.getString(PAIRING_INITIATOR_KEY, String(""))
}

bool Config::hasWifiCredentials()
{
  auto creds = this->getWifiCredentials();
  return !(creds.first == "" && creds.second == "");
}

std::pair<String, String> Config::getWifiCredentials()
{
  return std::pair<String, String>(
      preferences.getString(WIFI_SSID_KEY, String("")),
      preferences.getString(WIFI_PASS_KEY, String("")));
}

void Config::setWifiCredentials(const char *ssid, const char *pass)
{
  preferences.putString(WIFI_SSID_KEY, ssid);
  preferences.putString(WIFI_PASS_KEY, pass);
}

void Config::clearWifiCredentials()
{
  preferences.remove(WIFI_SSID_KEY);
  preferences.remove(WIFI_PASS_KEY);
}

void Config::clear()
{
  preferences.clear();
}