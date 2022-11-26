#pragma once
#include <Adafruit_SCD30.h>
#include "./water_level.h"

#define ENVIRONMENT_UPDATE_INTERVAL 1000

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

private:
  uint32_t nextUpdate = 0;
};

extern Environment environment;