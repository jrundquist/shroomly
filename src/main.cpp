#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
#include "status_pixel.h"

#define FORMAT_LITTLEFS_IF_FAILED true

void setup()
{
  status_pixel::pixelWrite(status_pixel::BLUE);

  Serial.begin(115200);

  status_pixel::setup();

  Serial.println("Starting test...");

  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
  {
    Serial.println("LittleFS Mount Failed");
    pinMode(LED_BUILTIN, OUTPUT);
    while (true)
    {
      status_pixel::pixelWrite(status_pixel::RED);
    }
  }
  Serial.println("LittleFS Mounted!");
  status_pixel::pixelFlash(status_pixel::GREEN);

  Serial.println("Test complete");
}

void loop()
{
}
