#pragma once

#include "time.h"
#include <sys/time.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "../core/environment.h"
#include "../core/wifi.h"
#include "../core/storage.h"
#include "../core/device_state.h"
#include "../include/common.h"

#define THINGNAME "shroomly_proto_00"
#define AWS_ENDPOINT "a3feb25p4t37j5-ats.iot.us-west-2.amazonaws.com"

#define AWS_ROOT_CA "/certs/root.pem"
#define AWS_DEVICE_CERT String("/certs/") + String(THINGNAME) + String(".cert.pem")
#define AWS_DEVICE_PRIVATE_KEY String("/certs/") + String(THINGNAME) + String(".private.key")

// The MQTT topics that this device will publish/subscribe to.
#define AWS_IOT_SENSOR_TOPIC String(F("shroomly/")) + String(THINGNAME) + String(F("/sensors"))
#define SENSOR_PUBLISH_INTERVAL 60000 /** 1 Minute */

#define LWT_TOPIC (String("republish/things/") + String(THINGNAME) + String(F("/shadow/update"))).c_str()

#define _SHADOW_PREFIX String(F("$aws/things/")) + String(THINGNAME) + String(F("/shadow"))
#define SHADOW_SEND_GET_TOPIC _SHADOW_PREFIX + String(F("/get"))
#define SHADOW_GET_ACCEPTED_TOPIC _SHADOW_PREFIX + String(F("/get/accepted"))

#define SHADOW_SEND_UPDATE_TOPIC (_SHADOW_PREFIX + String(F("/update"))).c_str()
#define SHADOW_UPDATE_ACCEPTED_TOPIC (_SHADOW_PREFIX + String(F("/update/accepted"))).c_str()
#define SHADOW_UPDATE_DELTA_TOPIC (_SHADOW_PREFIX + String(F("/update/delta"))).c_str()

class Aws
{
public:
  bool begin();
  void loop();

  const DeviceState getState() { return this->state; }

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

  /// @brief Reconciles incoming deltas with the current device state.
  /// @param delta JsonObject containing the incoming delta
  void handleDelta(JsonObject delta);

  DeviceState state;
  unsigned long lastPushedStateVersion = 0;
};

extern Aws aws;