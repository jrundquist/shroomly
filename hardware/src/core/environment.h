#pragma once
#include <Adafruit_SCD30.h>
#include <ArduinoJson.h>
#include "./water_level.h"

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