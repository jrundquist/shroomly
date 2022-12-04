#pragma once
#include <Adafruit_VC0706.h>
#include "./storage.h"

#if defined(__AVR__) || defined(ESP8266)
// On Uno: camera TX connected to pin 2, camera RX to pin 3:
#include <SoftwareSerial.h>
SoftwareSerial cameraconnection(2, 3);
// On Mega: camera TX connected to pin 69 (A15), camera RX to pin 3:
// SoftwareSerial cameraconnection(69, 3);
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// Using hardware serial on Mega: camera TX conn. to RX1,
// camera RX to TX1, no SoftwareSerial object is required:
#define cameraconnection Serial1
#endif

typedef std::pair<uint32_t, std::function<uint8_t *(uint8_t)>> Photo;

class Camera
{
public:
  void init();
  void takePhoto(String filename);
  std::pair<uint32_t, std::function<uint8_t *(uint8_t)>> takePhoto();
};

extern Camera camera;