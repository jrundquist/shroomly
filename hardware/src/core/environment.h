#pragma once
#include <Adafruit_SCD30.h>
#include <ArduinoJson.h>
#include "../include/pins.h"
#include "./water_level.h"

namespace
{
  const uint8_t ON = 0xff;
  const uint8_t OFF = 0x00;
}

class Environment
{
public:
  void init();
  void loop();

  float getHumidity();
  float getTempC();
  float getTempF();
  float getCO2();
  uint32_t getWaterLevel();

  unsigned long latestTimestamp()
  {
    return _latest_read_ts;
  }

private:
  unsigned long _latest_read_ts;
};

extern Environment environment;