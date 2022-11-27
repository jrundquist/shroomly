#pragma once

#include "time.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "storage.h"
#include "../include/secrets.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_SENSOR_PUBLISH_TOPIC "shroomly/sensors"
#define AWS_IOT_SUBSCRIBE_TOPIC "shroomly/sub"
#define LOOP_INTERVAL 1000

#define THINGNAME "shroomly_proto"
#define AWS_ENDPOINT "a3feb25p4t37j5-ats.iot.us-west-1.amazonaws.com"

class Aws
{
public:
  bool begin();
  void publishMessage();
  void loop();

private:
  void
  messageHandler(String &topic, String &payload);
};

extern Aws aws;