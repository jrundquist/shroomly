#include "./lighting.h"

Lighting lighting;

void Lighting::init()
{
  pinMode(GROW_LIGHT_PIN, OUTPUT);
}

void Lighting::setGrowLightBrightness(uint8_t brightness)
{
  analogWrite(GROW_LIGHT_PIN, brightness);
}