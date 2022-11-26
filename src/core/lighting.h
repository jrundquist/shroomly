#include "../include/common.h"
#include "../core/status_pixel.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#define GROW_LIGHT_PIN A5

class Lighting
{
public:
  void init();
  void setGrowLightBrightness(uint8_t brightness);
};

extern Lighting lighting;