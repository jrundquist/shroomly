#include "main.h"

bool setupComplete = false;

void setup()
{

  Serial.begin(115200);
  Serial.println();

  Serial.println("Display Init");
  display.init();
  display.clear();

  Serial.println("statusPixel init");
  statusPixel.init();
  statusPixel.pixelWrite(colors::ORANGE);

  Serial.println("storage init");
  storage.init();

  Serial.println("Lighting init");
  lighting.init();

  Serial.println("environment");
  environment.init();

  Serial.println("wifi init");
  wifi.init();

  Serial.println("config init");
  config.init();

  Serial.println("camera init");
  camera.init();

  if (!config.hasWifiCredentials())
  {
    Serial.println("No wifi creds. Starting Access point.");
    statusPixel.pixelWrite(colors::BLUE);
    // Enter setup mode
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

      // init and get the time
      configTime(3600 * -7, 0, NTP_SERVER);
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo))
        Serial.println("Failed to obtain time");
      else
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

      Serial.print("Initializing AWS...");
      if (!aws.begin())
      {
        Serial.println("FAILED");
      }
      else
      {
        Serial.println("OK");
      }
      setupComplete = true;
    }
    else
    {
      Serial.println("Wifi Creds didn't work?");
      Serial.println("Clearing credentials and restarting....");
      // config.clearWifiCredentials();
      ESP.restart();
    }
  }
}

void loop()
{
  if (setupComplete)
  {
    environment.loop(aws.getState());
    aws.loop();
    display.showEnviroment(environment);
  }
}
