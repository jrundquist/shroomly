#pragma once
#include <ArduinoJson.h>

namespace
{
  const char *kGrowLightKey = "growLightOn";
  const char *kFanOnKey = "fanOn";
  const char *kHumidifierOnKey = "humidifierOn";
}

struct DeviceState
{
  bool growLightOn;
  bool fanOn;
  bool humidifierOn;

  unsigned long version = 1;

  bool operator==(const DeviceState &other) const
  {
    return std::tie(growLightOn) == std::tie(other.growLightOn);
  }

  void applyJSON(JsonObject doc)
  {
    if (doc.containsKey(kGrowLightKey))
    {
      if (doc[kGrowLightKey] != growLightOn)
      {
        growLightOn = doc[kGrowLightKey];
        Serial.print("<Device State> Grow Light is now: ");
        Serial.println(growLightOn ? String("ON") : String("OFF"));
        version++;
      }
    }

    if (doc.containsKey(kFanOnKey))
    {
      if (doc[kFanOnKey] != fanOn)
      {
        fanOn = doc[kFanOnKey];
        Serial.print("<Device State> Fan is now: ");
        Serial.println(fanOn ? String("ON") : String("OFF"));
        version++;
      }
    }

    if (doc.containsKey(kHumidifierOnKey))
    {
      if (doc[kHumidifierOnKey] != humidifierOn)
      {
        humidifierOn = doc[kHumidifierOnKey];
        Serial.print("<Device State> Humidifier is now: ");
        Serial.println(humidifierOn ? String("ON") : String("OFF"));
        version++;
      }
    }
  }

  void toJSON(JsonObject doc)
  {
    doc[kGrowLightKey] = growLightOn;
    doc[kFanOnKey] = fanOn;
    doc[kHumidifierOnKey] = humidifierOn;
  }
};