#pragma once

#include <LittleFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

class Storage
{
public:
  void init();
};

extern Storage storage;