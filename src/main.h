#pragma once
#include <Arduino.h>

#include "config.h"

#include "core/status_pixel.h"
#include "core/http.h"
#include "core/storage.h"
#include "core/wifi.h"
#include "core/config.h"
#include "core/lighting.h"

#define DEBUG
#define WIFI_STATUS_INTERVAL 1000

void setup();
void loop();