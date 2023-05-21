#pragma once
#include <Arduino.h>

#include "time.h"

#include "config.h"

#include "core/status_pixel.h"
#include "core/storage.h"
#include "core/wifi.h"
#include "core/config.h"
#include "core/lighting.h"
#include "core/camera.h"
#include "core/environment.h"
#include "core/display.h"
#include "core/aws.h"

#include <WebServer.h>

#define DEBUG
#define WIFI_STATUS_INTERVAL 30000

void setup();
void loop();