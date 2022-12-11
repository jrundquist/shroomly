#pragma once

#include <LittleFS.h>
#include <utility>
#include <functional>
#include <algorithm>

#define FORMAT_LITTLEFS_IF_FAILED false

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

  char *readFileAsBuff(String path) { return this->readFileAsBuff(path.c_str()); };
  char *readFileAsBuff(const char *path);
};

extern Storage storage;