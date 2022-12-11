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

void Storage::deleteFile(const char *filename)
{
  LittleFS.remove(filename);
  delay(10);
}

char *Storage::readFileAsBuff(const char *path)
{
  File file = LittleFS.open(path);
  if (!file)
  {
    Serial.println("Failed to open " + String(path));
    return NULL;
  }
  char *buffer = (char *)calloc(file.size() + 1, sizeof(char));
  file.readBytes(buffer, file.size());
  file.close();
  return buffer;
}

String Storage::readFile(const char *filename)
{
  auto file = LittleFS.open(filename);
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

void Storage::writeFile(const char *filename, uint32_t len, std::function<uint8_t *(uint8_t)> read)
{
  Serial.printf("Writing file: %s\r\n", filename);

  File file = LittleFS.open(filename, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (len > 0)
  {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = std::min((uint32_t)16, len); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = read(bytesToRead);
    file.write(buffer, bytesToRead);
    if (++wCount >= 64)
    { // Every 2K, give a little feedback so it doesn't appear locked up
      Serial.print('.');
      wCount = 0;
    }
    // Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    len -= bytesToRead;
  }
  Serial.println("\nDone.");
  file.close();
}