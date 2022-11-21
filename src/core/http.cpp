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

void Http::handleTakePhoto(AsyncWebServerRequest *request)
{
  if (http.requestCameraImage == false)
  {
    http.requestCameraImage = true;
  }
  // AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/temp.jpg", "image/jpeg");

  // AsyncWebServerResponse *response = request->beginChunkedResponse("image/jpeg", [photo](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
  //                                                                  {
  //   // Write up to "maxLen" bytes into "buffer" and return the amount written.
  //   // index equals the amount of bytes that have been already sent
  //   // You will be asked for more data until 0 is returned
  //   // Keep in mind that you can not delay or yield waiting for more data!
  //   //  Read all the data up to # bytes!
  //   if (index >= photo.first)
  //   {
  //     return 0;
  //   }
  //   uint8_t bytesToRead = std::min(maxLen, photo.first - index);
  //   buffer = photo.second(bytesToRead);
  //   return bytesToRead; });
  // response->addHeader("Server", "ESP Async Web Server");
  request->send(200, "application/json", "{}");
}

void Http::init()
{
  // Web Server Root URL
  server.on("/", HTTP_GET, Http::handleSetup);
  server.on("/scanNetworks", HTTP_GET, Http::handleScanNetworks);
  server.on("/setNetwork", HTTP_GET, Http::handleSetNetwork);
  server.on("/reset", HTTP_GET, Http::handleReset);
  server.on("/takePhoto", HTTP_GET, Http::handleTakePhoto);

  server.serveStatic("/", LittleFS, "/");

  server.begin();
}