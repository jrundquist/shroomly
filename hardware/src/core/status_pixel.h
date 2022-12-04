#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../core/colors.h"

class StatusPixel
{
public:
  void init();
  void pixelWrite(const int *color);
  void pixelBrightness(const int *color, float brightness);
  void pixelFlash(const int *color);
};

extern StatusPixel statusPixel;