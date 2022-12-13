#pragma once

#include "time.h"
#include <sys/time.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "../core/environment.h"
#include "../core/wifi.h"
#include "../core/storage.h"
#include "../include/common.h"
#include "../include/files.h"

#define THINGNAME "shroomly_proto_00"
#define AWS_ENDPOINT "a3feb25p4t37j5-ats.iot.us-west-2.amazonaws.com"

// The MQTT topics that this device will publish/subscribe to.
#define AWS_IOT_SENSOR_TOPIC String(F("shroomly/")) + deviceIdStr() + String(F("/sensors"))
#define SENSOR_PUBLISH_INTERVAL 60000 /** 1 Minute */

#define LWT_TOPIC (String("republish/things/") + String(THINGNAME) + String(F("/shadow/update"))).c_str()

#define _SHADOW_PREFIX String(F("$aws/things/")) + String(THINGNAME) + String(F("/shadow"))
#define SHADOW_SEND_GET_TOPIC _SHADOW_PREFIX + String(F("/get"))
#define SHADOW_GET_ACCEPTED_TOPIC _SHADOW_PREFIX + String(F("/get/accepted"))

#define SHADOW_SEND_UPDATE_TOPIC (_SHADOW_PREFIX + String(F("/update"))).c_str()
#define SHADOW_UPDATE_ACCEPTED_TOPIC _SHADOW_PREFIX + String(F("/update/accepted"))
#define SHADOW_UPDATE_DELTA_TOPIC _SHADOW_PREFIX + String(F("/update/delta"))

class Aws
{
public:
  bool begin();
  void loop();

private:
  /// @brief Creates a JsonDoc pre-populated with device information.
  /// @tparam T size of the json document to allocate
  /// @return
  template <size_t T>
  StaticJsonDocument<T> createMessage();

  /// @brief Publishes the provided document to the specifed topic.
  /// @tparam T size of the json document that will be send
  /// @param topic MQTT topic to publish on
  /// @param doc JsonDocument to publish to the topic
  /// @return Returns success
  template <size_t T>
  bool publishMessage(String topic, StaticJsonDocument<T> doc);

  /// @brief Handle incoming messages for a given topic.
  /// @param topic
  /// @param payload
  void messageHandler(String &topic, String &payload);

  /// @brief Send any available environmental updates to the MQTT channel
  /// specified by
  void sendEnvUpdate();

  /// @brief send the device state
  void reportDeviceState();
};

extern Aws aws;