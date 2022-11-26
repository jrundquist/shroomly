#include "environment.h"

Adafruit_SCD30 scd30;

Environment environment;

void Environment::init()
{
  // Try to initialize!
  if (!scd30.begin())
  {
    Serial.println("Failed to find SCD30 chip");
  }
  Serial.println("SCD30 Found!");
  Serial.print("Measurement Interval: ");
  Serial.print(scd30.getMeasurementInterval());
  Serial.println(" seconds");

  Serial.print("Looking for water Sesnor...");
  if (!waterLevel.begin())
  {
    Serial.println(" NOT OK !");
  }
  else
  {
    Serial.println("OK");
  }
}

void Environment::loop()
{
  if (millis() > this->nextUpdate && scd30.dataReady())
  {
    this->nextUpdate = millis() + (scd30.getMeasurementInterval() * 1000);
    if (!scd30.read())
    {
      Serial.println("Error reading sensor data");
      return;
    }

    if (!waterLevel.read())
    {
      Serial.println("Error reading water level");
      return;
    }

    Serial.print("Temperature: ");
    Serial.print(scd30.temperature);
    Serial.println(" degrees C");

    Serial.print("Relative Humidity: ");
    Serial.print(scd30.relative_humidity);
    Serial.println(" %");

    Serial.print("CO2: ");
    Serial.print(scd30.CO2, 3);
    Serial.println(" ppm");
    Serial.println("");
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
