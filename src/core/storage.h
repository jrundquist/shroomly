#pragma once

#include <LittleFS.h>
#include <utility>

#define FORMAT_LITTLEFS_IF_FAILED true

class Storage
{
public:
  void init();
  bool fileExists(const char *filename);
  void writeFile(const char *filename, const char *message);
  String readFile(const char *filename);
};

extern Storage storage;