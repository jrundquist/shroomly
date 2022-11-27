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
    Serial.print(client.lastError());
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return false;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  return true;
};

void Aws::messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
};

void Aws::publishMessage()
{

  StaticJsonDocument<200> doc;

  doc["sample_time"] = getCurrentTime();
  doc["device_id"] = 1002;
  doc["data_sensor"] = rand();
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_SENSOR_PUBLISH_TOPIC, jsonBuffer);
}

void Aws::loop()
{
  if (millis() > nextLoop)
  {
    client.loop();
    nextLoop = millis() + LOOP_INTERVAL;
  }
}