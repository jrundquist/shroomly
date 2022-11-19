#include "./storage.h"

Storage storage;

void Storage::init()
{
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
  {
#ifdef DEBUG
    Serial.println("LittleFS Mount Failed");
#endif
    return;
  }
#ifdef DEBUG
  Serial.println("LittleFS Mounted!");
#endif
}