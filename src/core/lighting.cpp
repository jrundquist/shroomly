#include "./lighting.h"

Adafruit_AW9523 aw;

Lighting lighting;

void Lighting::init()
{

  if (!aw.begin(0x58))
  {
    Serial.println("AW9523 not found? Check wiring!");
    while (1)
      statusPixel.pixelFlash(colors::RED);
  }

  Serial.println("AW9523 found!");
  aw.pinMode(GROW_LIGHT_PIN, AW9523_LED_MODE);
  this->setGrowLightBrightness(255);
}

void Lighting::setGrowLightBrightness(uint8_t brightness)
{
  aw.analogWrite(GROW_LIGHT_PIN, brightness);
}