#pragma once
#include <Adafruit_SCD30.h>
#include <ArduinoJson.h>
#include "./water_level.h"
#include "../core/aws.h"

#define ENVIRONMENT_UPDATE_INTERVAL 1000
#define MQTT_PUBLISH_INTERVAL 60000 /*1 minute*/

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
};

extern Environment environment;