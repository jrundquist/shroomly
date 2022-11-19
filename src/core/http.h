#pragma once

#include "../core/storage.h"
#include "../core/wifi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Http
{
public:
  void init();

private:
  static void handleSetup(AsyncWebServerRequest *request);
  static void handleScanNetworks(AsyncWebServerRequest *request);
};

extern Http http;