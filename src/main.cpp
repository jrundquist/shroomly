#include "main.h"

void setup()
{

  Serial.begin(115200);
  delay(1000);
  Serial.println();

  statusPixel.init();
  storage.init();
  wifi.init();
  wifi.startAP();
  http.init();
}

void loop()
{
}
