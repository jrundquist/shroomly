#include "./lighting.h"

Adafruit_AW9523 board;

Lighting lighting;

void Lighting::init()
{

  if (!board.begin(0x58))
  {
    Serial.println("AW9523 not found? Check wiring!");
    while (1)
      statusPixel.pixelFlash(colors::RED);
  }

  Serial.println("AW9523 found!");
  board.pinMode(GROW_LIGHT_PIN, AW9523_LED_MODE);
  this->setGrowLightBrightness(250);
}

void Lighting::setGrowLightBrightness(uint8_t brightness)
{
  board.analogWrite(GROW_LIGHT_PIN, brightness);
}