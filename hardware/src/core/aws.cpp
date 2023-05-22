#include "./aws.h"

Aws aws;

namespace
{
  WiFiClientSecure net = WiFiClientSecure();
  MQTTClient client = MQTTClient(2048);

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
  const int startupColor[3]{0x00, 0x10, 0x10};
  statusPixel.pixelWrite(startupColor);
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

  const int connectedColor[3]{0x00, 0xa0, 0x20};
  statusPixel.pixelWrite(connectedColor);

  if (!client.connected())
  {
    Serial.println("<AWS> IoT Timeout!");
    return false;
  }

  client.subscribe(SHADOW_GET_ACCEPTED_TOPIC);
  client.subscribe(SHADOW_UPDATE_DELTA_TOPIC);
  client.subscribe(SHADOW_UPDATE_ACCEPTED_TOPIC);
  client.subscribe(IMAGE_UPLOAD_ACCEPTED_TOPIC);

  statusPixel.pixelWrite(colors::GREEN);

  return true;
};

void Aws::reportDeviceState()
{
  auto deviceState = StaticJsonDocument<512>();
  auto reported = deviceState.createNestedObject("state").createNestedObject("reported");
  reported["connected"] = true;
  this->state.toJSON(reported);

  char msgBuffer[512];
  serializeJson(deviceState, msgBuffer); // print to client
  if (client.publish(SHADOW_SEND_UPDATE_TOPIC, msgBuffer))
    lastPushedStateVersion = this->state.version;
};

void Aws::messageHandler(String &topic, String &payload)
{
  log_i("<AWS> Msg on topic: \"" + topic + "\" - " + payload);
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, payload);
  JsonObject root = doc["state"].as<JsonObject>();
  if (topic == SHADOW_UPDATE_DELTA_TOPIC)
  {
    handleDelta(root);
  }

  if (topic == IMAGE_UPLOAD_ACCEPTED_TOPIC)
  {
    auto url = doc["url"].as<String>();
    log_d("<AWS> Image upload accepted: %s", url);
    camera.uploadImage(url);
  }
};

void Aws::handleDelta(JsonObject delta)
{
  Serial.println("<AWS> Applying incoming delta");
  this->state.applyJSON(delta);
}

template <size_t T>
StaticJsonDocument<T> Aws::createMessage()
{
  StaticJsonDocument<T> doc;
  doc["time"] = getCurrentTime();
  doc["device_id"] = THINGNAME;
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
  Serial.println("<AWS> Publish :: " + topic);
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
        Serial.println("<AWS> Sensor readings failed to publish");
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
      statusPixel.pixelFlash(colors::RED);
      statusPixel.pixelWrite(colors::RED);
      Serial.print("<AWS> Disconnected: ");
      Serial.println(client.lastError());

      if (client.lastError() == 0)
      {
        Serial.println("<AWS> Reconnecting...");
        begin();
      }
    }
    return;
  }
  if (lastPushedStateVersion < this->state.version)
  {
    Serial.println("<AWS> Reporting changed device state: " + String(this->state.version));
    this->reportDeviceState();
  }
  sendEnvUpdate();
}

void Aws::sendImage(Camera &camera)
{
  auto msg = createMessage<200>();
  msg["timestamp"] = getCurrentTime();
  msg["device_id"] = String(THINGNAME);
  msg["req_id"] = camera.getImageId();
  if (!publishMessage(IMAGE_UPLOAD_TOPIC, msg))
  {
    Serial.println("<AWS> Image upload failed to publish");
  }
}