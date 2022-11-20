#include "../include/common.h"
#include "../core/status_pixel.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_AW9523.h>

#define GROW_LIGHT_PIN 1

class Lighting
{
public:
  void init();
  void setGrowLightBrightness(uint8_t brightness);
};

extern Lighting lighting;