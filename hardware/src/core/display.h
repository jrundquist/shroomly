#pragma once

#include "environment.h"

#ifdef ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT
#include "Adafruit_LC709203F.h"
#include <Adafruit_ST7789.h>
#include <Fonts/FreeSans12pt7b.h>
#endif

class Display
{
public:
  void init();
  void showEnviroment(Environment &env);
  void showAPInfo(String ssid, String ip);
  void clear();

private:
  void turnOnBacklight();
};

extern Display display;