#include "main.h"

void setup()
{

  Serial.begin(115200);
  delay(3000);
  Serial.println();

  Serial.println("statusPixel init");
  statusPixel.init();
  statusPixel.pixelWrite(colors::ORANGE);
  Serial.println("storage init");
  storage.init();
  Serial.println("wifi init");
  wifi.init();

  Serial.println("config init");
  config.init();

  if (!config.hasWifiCredentials())
  {
    Serial.println("No wifi creds. Starting Access point.");
    statusPixel.pixelWrite(colors::BLUE);
    wifi.startAP();
    Serial.print("Access Point: ");
    Serial.println(AP_SSID);
  }
  else
  {
    Serial.println("WiFi creds found.");
    auto creds = config.getWifiCredentials();
    wifi.connect(creds.first, creds.second);

    if (wifi.getStatus() == WL_CONNECTED)
    {
      Serial.println("Wifi Connected.");
      statusPixel.pixelWrite(colors::GREEN);
      Serial.print("\tConnected to: ");
      Serial.println(wifi.getSSID());
      Serial.print("\tIP: ");
      Serial.println(wifi.getLocalIp());
    }
    else
    {
      Serial.println("Wifi Creds didn't work?");
      Serial.println("Clearing credentials and restarting....");
      config.clearWifiCredentials();
      ESP.restart();
    }
  }

  Serial.println("http init");
  http.init();
}

void loop()
{
}
