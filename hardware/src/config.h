#pragma once

#include "stddef.h"

#define THINGNAME "shroomly_proto_01"

#define NTP_SERVER "pool.ntp.org"

#define HOSTNAME "SHROOMLY"
#define AP_PASS NULL
#define AP_SSID HOSTNAME

#define HTTP_SERVER_PORT 80
#define WIFI_CONNECTION_TIMEOUT 10000

// #define ENABLE_STATIC_IP
#if defined(ENABLE_STATIC_IP)
#define STATIC_IP    \
  {                  \
    192, 168, 0, 254 \
  }
#define GATEWAY    \
  {                \
    192, 168, 0, 1 \
  }
#define SUBNET       \
  {                  \
    255, 255, 255, 0 \
  }
#endif