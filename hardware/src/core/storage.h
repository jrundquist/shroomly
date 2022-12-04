#pragma once

#include <LittleFS.h>
#include <utility>
#include <functional>
#include <algorithm>

#define FORMAT_LITTLEFS_IF_FAILED true

class Storage
{
public:
  void init();
  bool fileExists(const char *filename);
  bool fileExists(String filename) { return this->fileExists(filename.c_str()); };
  void writeFile(const char *filename, const char *message);
  void writeFile(const char *filename, uint32_t len, std::function<uint8_t *(uint8_t)> read);
  void deleteFile(const char *filename);
  void deleteFile(String filename) { this->deleteFile(filename.c_str()); };
  String readFile(const char *filename);
};

extern Storage storage;