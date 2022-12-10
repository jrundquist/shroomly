#include "./aws.h"

Aws aws;

namespace
{
  WiFiClientSecure net = WiFiClientSecure();
  MQTTClient client = MQTTClient(256);

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
}

bool Aws::begin()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage([&](String &topic, String &payload) -> void
                   {
  Serial.println("incoming >> " + topic + " - " + payload);
  this->messageHandler(topic, payload);
  return; });

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return false;
  }

  // Serial.print(" { ");
  client.subscribe(SHADOW_GET_ACCEPTED_TOPIC);
  // Serial.println(SHADOW_UPDATE_DELTA_TOPIC);
  client.subscribe(SHADOW_UPDATE_DELTA_TOPIC);
  client.subscribe(SHADOW_SEND_UPDATE_ACCEPTED_TOPIC);

  auto lastWill = StaticJsonDocument<100>();
  auto reported = lastWill.createNestedObject("state").createNestedObject("reported");
  reported["connected"] = false;
  char lastWillBuffer[200];
  serializeJson(lastWill, lastWillBuffer); // print to client
  client.setWill(SHADOW_SEND_UPDATE_TOPIC, lastWillBuffer);

  reportDeviceState();

  return true;
};

void Aws::reportDeviceState()
{
  auto deviceState = StaticJsonDocument<100>();
  auto reported = deviceState.createNestedObject("state").createNestedObject("reported");
  reported["connected"] = true;

  char msgBuffer[200];
  serializeJson(deviceState, msgBuffer); // print to client
  client.publish(SHADOW_SEND_UPDATE_TOPIC, msgBuffer);
};

void Aws::messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
};

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
  Serial.println(topic);
  return client.publish(topic, jsonBuffer);
}

void Aws::sendEnvUpdate()
{
  if (millis() > nextMQTTPush && environment.latestTimestamp() != lastPushedEnvTimestamp)
  {
    auto msg = createMessage<200>();
    JsonObject env = msg.createNestedObject("env");
    env["co2"] = environment.getCO2();
    env["hum"] = environment.getHumidity();
    env["temp"] = environment.getTempF();
    lastPushedEnvTimestamp = environment.latestTimestamp();

    if (publishMessage(AWS_IOT_SENSOR_TOPIC, msg))
    {
      Serial.println("Published Sensor Readings");
      nextMQTTPush = millis() + SENSOR_PUBLISH_INTERVAL;
    }
    else
    {
      Serial.println("Publish failed..");
    }
  }
}

void Aws::loop()
{
  client.loop();
  sendEnvUpdate();
}