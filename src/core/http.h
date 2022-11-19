#pragma once

#include "../core/storage.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Http
{
public:
  void init();

private:
  static void handleSetup(AsyncWebServerRequest *request);
};

extern Http http;