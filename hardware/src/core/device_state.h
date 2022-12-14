#pragma once
#include <ArduinoJson.h>

struct DeviceState
{
  bool growLightOn;

  bool operator==(const DeviceState &other) const
  {
    return std::tie(growLightOn) == std::tie(other.growLightOn);
  }

  void applyJSON(JsonObject doc)
  {
    if (doc.containsKey("growLightOn"))
    {
      growLightOn = doc["growLightOn"];
    }
  }
};