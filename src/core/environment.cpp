#include "environment.h"

Adafruit_SCD30 scd30;

Environment environment;
unsigned long nextSensorRead = 0;
unsigned long nextMQTTPush = 0;
bool successfulyReadCO2 = false;
bool hasWater = false;
bool hasScd30 = false;

void Environment::init()
{
  // Try to initialize!
  if (!scd30.begin())
  {
    Serial.println("Failed to find SCD30 chip");
  }
  else
  {
    hasScd30 = true;
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
    hasWater = true;
    Serial.println("OK");
  }
}

void Environment::loop()
{
  if (millis() > nextSensorRead)
  {
    if (hasScd30)
    {
      nextSensorRead = millis() + (scd30.getMeasurementInterval() * 1000);
      if (!scd30.read())
      {
        Serial.println("Error reading sensor data");
        return;
      }
      if (!successfulyReadCO2 && scd30.CO2 > 0)
      {
        successfulyReadCO2 = true;
      }
    }

    if (hasWater)
    {
      if (!waterLevel.read())
      {
        Serial.println("Error reading water level");
        // return;
      }
    }
  }

  if (millis() > nextMQTTPush && successfulyReadCO2)
  {
    auto msg = aws.createMessage();
    JsonObject env = msg.createNestedObject("env");
    env["co2"] = this->getCO2();
    env["hum"] = this->getHumidity();
    env["temp"] = this->getTempF();

    if (aws.publishSensorMessage(msg))
    {
      Serial.println("Published Sensor Readings");
      nextMQTTPush = millis() + MQTT_PUBLISH_INTERVAL;
    }
    else
    {
      // nextMQTTPush = millis() + 1000;
      Serial.println("Publish failed..");
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
