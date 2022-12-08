#include "../include/common.h"
#include "../include/pins.h"
#include "../core/status_pixel.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class Lighting
{
public:
  void init();
  void setGrowLightBrightness(uint8_t brightness);
};

extern Lighting lighting;