#pragma once

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
// #include <iostream>

// #include "./device_state.h"

#define CS_PIN 27
class Camera
{
public:
  void captureImage();
  uint32_t imageLength();

  template <typename T>
  void writeImage(T &outputStream);

  void init();
  void loop();
};

extern Camera camera;