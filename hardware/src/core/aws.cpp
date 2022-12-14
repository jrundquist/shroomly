#include "./aws.h"

Aws aws;

namespace
{
  WiFiClientSecure net = WiFiClientSecure();
  MQTTClient client = MQTTClient(256);

  bool disconnected = false;

  unsigned long nextLoop = 0;
  unsigned long nextMQTTPush = 0;
  unsigned long lastPushedEnvTimestamp = 0;

  unsigned long getCurrentTime()
  {
    struct tm timeinfo;
    time_t now;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return (0);
    }
    time(&now);
    return now;
  }

  void setWill()
  {
    auto lastWill = StaticJsonDocument<48>();
    auto reported = lastWill.createNestedObject("state").createNestedObject("reported");
    reported["connected"] = false;
    char lastWillBuffer[48];
    serializeJson(lastWill, lastWillBuffer); // print to client
    client.setWill(LWT_TOPIC, lastWillBuffer);
  }
}

bool Aws::begin()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(storage.readFileAsBuff(AWS_ROOT_CA));
  net.setCertificate(storage.readFileAsBuff(AWS_DEVICE_CERT));
  net.setPrivateKey(storage.readFileAsBuff(AWS_DEVICE_PRIVATE_KEY));

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_ENDPOINT, 8883, net);
  setWill();

  // Create a message handler
  client.onMessage([&](String &topic, String &payload)
                   { messageHandler(topic, payload); });

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    Serial.print("[");
    Serial.print(client.lastError());
    Serial.print("]");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return false;
  }

  client.subscribe(SHADOW_GET_ACCEPTED_TOPIC);
  client.subscribe(SHADOW_UPDATE_DELTA_TOPIC);
  client.subscribe(SHADOW_UPDATE_ACCEPTED_TOPIC);

  reportDeviceState();

  return true;
};

void Aws::reportDeviceState()
{
  auto deviceState = StaticJsonDocument<100>();
  auto reported = deviceState.createNestedObject("state").createNestedObject("reported");
  reported["connected"] = true;
  reported["growLightOn"] = this->state.growLightOn;

  char msgBuffer[200];
  serializeJson(deviceState, msgBuffer); // print to client
  client.publish(SHADOW_SEND_UPDATE_TOPIC, msgBuffer);
};

void Aws::messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  JsonObject root = doc["state"].as<JsonObject>();
  if (topic == SHADOW_UPDATE_DELTA_TOPIC)
  {
    handleDelta(root);
  }
};

void Aws::handleDelta(JsonObject delta)
{
  Serial.println("Handling incoming delta");
  this->state.applyJSON(delta);
  reportDeviceState();
}

template <size_t T>
StaticJsonDocument<T> Aws::createMessage()
{
  StaticJsonDocument<T> doc;
  doc["time"] = getCurrentTime();
  doc["device_id"] = deviceIdStr();
  doc["uptime"] = (int)(millis() / 1000);
  doc["rand_number"] = rand();
  doc["wifi_rssi"] = wifi.getRSSI();
  return doc;
}

template <size_t T>
bool Aws::publishMessage(String topic, StaticJsonDocument<T> doc)
{
  char jsonBuffer[T];
  serializeJson(doc, jsonBuffer); // print to client
  Serial.println("AWS :: Publish :: " + topic);
  return client.publish(topic, jsonBuffer);
}

void Aws::sendEnvUpdate()
{
  if (millis() > nextMQTTPush)
  {
    if (environment.latestTimestamp() != lastPushedEnvTimestamp)
    {
      auto msg = createMessage<200>();
      // Overwrite the time, setting it to when the sensor data was read.
      msg["time"] = environment.latestTimestamp();
      JsonObject env = msg.createNestedObject("env");
      env["co2"] = environment.getCO2();
      env["hum"] = environment.getHumidity();
      env["temp"] = environment.getTempF();
      lastPushedEnvTimestamp = environment.latestTimestamp();

      if (!publishMessage(AWS_IOT_SENSOR_TOPIC, msg))
      {
        Serial.println("AWS :: Sensor readings failed to publish");
      }
    }
    nextMQTTPush = millis() + SENSOR_PUBLISH_INTERVAL;
  }
}

void Aws::loop()
{
  client.loop();
  if (!client.connected())
  {
    if (!disconnected)
    {
      disconnected = true;
      statusPixel.pixelFlash(colors::RED);
      statusPixel.pixelWrite(colors::RED);
    }
    return;
  }
  sendEnvUpdate();
}