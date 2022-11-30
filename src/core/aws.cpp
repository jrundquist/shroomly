#include "./aws.h"

Aws aws;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

unsigned long nextLoop = 0;

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

  // Subscribe to a topic
  // client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  return true;
};

void Aws::messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
};

StaticJsonDocument<1024> Aws::createMessage()
{
  StaticJsonDocument<1024> doc;
  doc["time"] = getCurrentTime();
  doc["device_id"] = deviceIdStr();
  doc["uptime"] = (int)(millis() / 1000);
  doc["rand_number"] = rand();
  doc["wifi_rssi"] = wifi.getRSSI();
  return doc;
}

bool Aws::publishSensorMessage(StaticJsonDocument<1024> doc)
{
  auto topic = String("shroomly/") + deviceIdStr() + String("/sensors");
  char jsonBuffer[1024];
  serializeJson(doc, jsonBuffer); // print to client
  Serial.println(topic);
  return client.publish(topic, jsonBuffer);
}

void Aws::loop()
{
  client.loop();
}