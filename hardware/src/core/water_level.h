#pragma once

#include <Arduino.h>
#include <Wire.h>

#define NO_TOUCH 0xFE
#define THRESHOLD 100
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77
#define NUM_LOW_BYTES 8
#define NUM_HIGH_BYTES 12

inline void getHigh12SectionValue()
{

}

inline void getLow8SectionValue()
{

}

class WaterLevel
{
public:
  bool begin();
  bool read();

  uint32_t level = 0;
};

extern WaterLevel waterLevel;