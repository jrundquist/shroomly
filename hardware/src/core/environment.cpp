#include "environment.h"

Adafruit_SCD30 scd30;

Environment environment;

namespace
{
  unsigned long nextSensorRead = 0;
  bool successfulyReadCO2 = false;
  bool hasWater = false;
  bool hasScd30 = false;

  uint8_t fanOnSpeed = 0xff;

  bool isControllingHumidity = false;
  unsigned long nextFanStateChange = 1000; // Dont start the fan immediately
  unsigned long fanOnForTime = 5000;       // 5s
  unsigned long fanOffForTime = 10000;     // 10s
  bool isFanOn = false;
}

void Environment::init()
{

  // Set Pin modes
  pinMode(HUMIDIFIER_FAN_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  pinMode(GROW_LIGHT_PIN, OUTPUT);

  // Try to initialize!
  if (!scd30.begin())
  {
    Serial.println("Failed to find SCD30 chip");
  }
  else
  {
    Serial.println("SCD30 Found!");
    Serial.print("\tMeasurement Interval: ");
    Serial.print(scd30.getMeasurementInterval());
    Serial.println(" seconds");
    hasScd30 = true;
  }

  Serial.print("Looking for water Sesnor...");
  if (!waterLevel.begin())
  {
    Serial.println(" NOT OK !");
  }
  else
  {
    hasWater = true;
    Serial.println("OK");
  }
}

void Environment::loop()
{

  if (isControllingHumidity)
  {
    if (millis() >= nextFanStateChange)
    {
      auto speed = isFanOn ? OFF : fanOnSpeed;
      auto inThisStateFor = isFanOn ? fanOffForTime : fanOnForTime;
      nextFanStateChange = millis() + inThisStateFor;
      isFanOn = !isFanOn;
      analogWrite(HUMIDIFIER_FAN_PIN, speed);
    }
  }

  if (millis() > nextSensorRead)
  {
    if (hasScd30)
    {
      nextSensorRead = millis() + (scd30.getMeasurementInterval() * 1000);
      if (!scd30.read())
      {
        Serial.println("Error reading sensor data");
      }
      if (!successfulyReadCO2 && scd30.CO2 > 0)
      {
        successfulyReadCO2 = true;
      }
      this->_latest_read_ts = millis();
    }

    if (hasWater)
    {
      if (!waterLevel.read())
      {
        Serial.println("Error reading water level");
        // return;
      }
      this->_latest_read_ts = millis();
    }
  }
}

/// @brief Most recent humidity reading
/// @return
float Environment::getHumidity()
{
  return scd30.relative_humidity;
}

/// @brief Most recent Temperature reading in Celsius
/// @return
float Environment::getTempC()
{
  return scd30.temperature;
}

/// @brief Most recent Temperature reading in Fahrenheit
/// @return
float Environment::getTempF()
{
  return scd30.temperature * 9.0 / 5.0 + 32.0;
}

/// @brief Most recent CO2 reading
/// @return
float Environment::getCO2()
{
  return scd30.CO2;
}

uint32_t Environment::getWaterLevel()
{
  return waterLevel.level;
}
