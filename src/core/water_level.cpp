#include "./water_level.h"

WaterLevel waterLevel;

bool WaterLevel::begin()
{
  bool ok = true;
  Wire.begin();

  Wire.beginTransmission(ATTINY2_LOW_ADDR);
  if (Wire.endTransmission() != 0)
  {
    ok = false;
    Serial.println("Did not find water sensor low addr!");
  }

  Wire.beginTransmission(ATTINY1_HIGH_ADDR);
  if (Wire.endTransmission() != 0)
  {
    ok = false;
    Serial.println("Did not find water sensor high addr!");
  }
  return ok;
}

bool WaterLevel::read()
{
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;

  uint32_t touch_val = 0;
  uint8_t trig_section = 0;

  uint8_t low_data[NUM_LOW_BYTES] = {0};
  uint8_t high_data[NUM_HIGH_BYTES] = {0};

  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, NUM_LOW_BYTES);
  while (NUM_LOW_BYTES != Wire.available())
    ;

  for (int i = 0; i < NUM_LOW_BYTES; i++)
  {
    low_data[i] = Wire.read(); // receive a byte as character
  }

  delay(100);

  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, NUM_HIGH_BYTES);
  while (NUM_HIGH_BYTES != Wire.available())
    ;

  for (int i = 0; i < NUM_HIGH_BYTES; i++)
  {
    high_data[i] = Wire.read();
  }

  Serial.println("low 8 sections value = ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(low_data[i]);
    Serial.print(".");
    if (low_data[i] >= sensorvalue_min && low_data[i] <= sensorvalue_max)
    {
      low_count++;
    }
    if (low_count == 8)
    {
      Serial.print("      ");
      Serial.print("PASS");
    }
  }
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("high 12 sections value = ");
  for (int i = 0; i < 12; i++)
  {
    Serial.print(high_data[i]);
    Serial.print(".");

    if (high_data[i] >= sensorvalue_min && high_data[i] <= sensorvalue_max)
    {
      high_count++;
    }
    if (high_count == 12)
    {
      Serial.print("      ");
      Serial.print("PASS");
    }
  }

  Serial.println("  ");
  Serial.println("  ");

  for (int i = 0; i < 8; i++)
  {
    if (low_data[i] > THRESHOLD)
    {
      touch_val |= 1 << i;
    }
  }
  for (int i = 0; i < 12; i++)
  {
    if (high_data[i] > THRESHOLD)
    {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }

  while (touch_val & 0x01)
  {
    trig_section++;
    touch_val >>= 1;
  }
  Serial.print("water level = ");
  Serial.print(trig_section * 5);
  Serial.println("% ");
  Serial.println(" ");

  waterLevel.level = trig_section * 5;
  return true;
}