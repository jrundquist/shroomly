#pragma once

#include "../core/status_pixel.h"
#include "../core/storage.h"
#include "../core/wifi.h"
#include "../core/config.h"
#include "../core/camera.h"
#include "../core/environment.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class Http
{
public:
  void init();
  bool requestCameraImage;

private:
  static void handleSetup(AsyncWebServerRequest *request);
  static void handleScanNetworks(AsyncWebServerRequest *request);
  static void handleSetNetwork(AsyncWebServerRequest *request);
  static void handleReset(AsyncWebServerRequest *request);
  static void handleTakePhoto(AsyncWebServerRequest *request);
  static void handleGetSensorReadings(AsyncWebServerRequest *request);
};

extern Http http;