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

bool Storage::fileExists(const char *filename)
{
  return LittleFS.exists(filename);
}

String Storage::readFile(const char *filename)
{
  auto file = LittleFS.open(filename, "r");
  size_t filesize = file.size();
  // Read into temporary Arduino String
  String data = file.readString();
  // Don't forget to clean up!
  file.close();
  return data;
}

void Storage::writeFile(const char *filename, const char *message)
{
  Serial.printf("Writing file: %s\r\n", filename);

  File file = LittleFS.open(filename, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}