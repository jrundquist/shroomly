#include "./http.h"

Http http;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void Http::handleSetup(AsyncWebServerRequest *request)
{
  request->send(LittleFS, "/setup.html", "text/html");
}

void Http::handleScanNetworks(AsyncWebServerRequest *request)
{
  String json = "[";
  int n = wifi.scanComplete();
  if (n == WIFI_SCAN_FAILED)
  {
    wifi.scanNetworks(true);
  }
  else if (n)
  {
    for (int i = 0; i < n; ++i)
    {
      if (i)
        json += ",";
      json += "{";
      json += "\"rssi\":" + String(wifi.getRSSI(i));
      json += ",\"ssid\":\"" + wifi.getSSID(i) + "\"";
      json += ",\"bssid\":\"" + wifi.getBSSIDstr(i) + "\"";
      json += ",\"channel\":" + String(wifi.getChannel(i));
      json += ",\"secure\":" + String(wifi.getEncryptionType(i));
      json += "}";
    }
    wifi.scanDelete();
    if (wifi.scanComplete() == WIFI_SCAN_FAILED)
    {
      wifi.scanNetworks(true);
    }
  }
  json += "]";
  request->send(200, "application/json", json);
  json = String();
}

void Http::handleSetNetwork(AsyncWebServerRequest *request)
{
  String ssid = request->arg("ssid");
  String pass = request->arg("pass");

  ssid.trim();
  pass.trim();

  bool valid = pass.length() > 0 && ssid.length() > 0;
  if (!valid)
  {
    return request->send(400, "application/json", "{}");
  }

  auto existingCreds = config.getWifiCredentials();
  if (existingCreds.first == ssid && existingCreds.second == pass)
  {
    return request->send(200, "application/json", "{\"res\":\"Creds set\"}");
  }

  config.setWifiCredentials(ssid.c_str(), pass.c_str());
  statusPixel.pixelFlash(colors::GREEN);
  ESP.restart();
}

void Http::handleReset(AsyncWebServerRequest *request)
{
  request->send(200, "application/json", "{}");
  config.clear();
  ESP.restart();
}

void Http::init()
{
  // Web Server Root URL
  server.on("/", HTTP_GET, Http::handleSetup);
  server.on("/scanNetworks", HTTP_GET, Http::handleScanNetworks);
  server.on("/setNetwork", HTTP_GET, Http::handleSetNetwork);
  server.on("/reset", HTTP_GET, Http::handleReset);

  server.serveStatic("/", LittleFS, "/");

  server.begin();
}