#include "./device.h"

String deviceIdStr()
{
  return String(WiFi.macAddress());
}