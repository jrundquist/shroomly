#include "./display.h"

#ifdef ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT
GFXcanvas16 canvas(240, 135);
Adafruit_ST7789 disp = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#endif

Display display;

void Display::init()
{
#ifdef ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT
  disp.init(135, 240); // Init ST7789 240x135
  disp.setRotation(3);
  canvas.setFont(&FreeSans12pt7b);
  canvas.setTextColor(ST77XX_WHITE);
#endif
  display.turnOnBacklight();
}

void Display::clear()
{
#ifdef ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT
  canvas.fillScreen(ST77XX_BLACK);
  disp.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
  display.turnOnBacklight();
#endif
}

void Display::showEnviroment(Environment &env)
{
#ifdef ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT

  canvas.fillScreen(ST77XX_BLACK);
  canvas.setCursor(0, 18);
  canvas.setTextColor(ST77XX_YELLOW);
  canvas.print("Temp: ");
  canvas.setTextColor(ST77XX_WHITE);
  canvas.print(env.getTempF());
  canvas.println(" F");
  canvas.setTextColor(ST77XX_CYAN);
  canvas.print("Humidity: ");
  canvas.setTextColor(ST77XX_WHITE);
  canvas.print(env.getHumidity());
  canvas.println("%");
  canvas.setTextColor(ST77XX_MAGENTA);
  canvas.print("CO2: ");
  canvas.setTextColor(ST77XX_WHITE);
  canvas.print(env.getCO2());
  canvas.println("ppm");

  canvas.setTextColor(ST77XX_BLUE);
  canvas.print("Water Level: ");
  canvas.setTextColor(ST77XX_WHITE);
  canvas.print(env.getWaterLevel());
  canvas.println("%");

  disp.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
  display.turnOnBacklight();
#endif
}

void Display::showAPInfo(String ssid, String ip)
{
#ifdef ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT
  canvas.fillScreen(ST77XX_BLACK);
  canvas.setCursor(0, 18);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.println("SSID : " + ssid);

  canvas.println("IP: " + ip);

  disp.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
  display.turnOnBacklight();
#endif
}

void Display::turnOnBacklight()
{
#ifdef TFT_BACKLITE
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
#endif
}