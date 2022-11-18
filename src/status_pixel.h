#ifndef SHROOMLY_STATUS_PIXEL
#define SHROOMLY_STATUS_PIXEL

#include <Adafruit_NeoPixel.h>

namespace
{
  // metroPixel takes in both the number of pixels (1, the built-in) and the pin)
  Adafruit_NeoPixel metroPixel = Adafruit_NeoPixel(NEOPIXEL_NUM, PIN_NEOPIXEL);
}

namespace status_pixel
{

  /* Colors */
  // note: the max. of colors in these arrays is 220 instead of 255 (super-bright!!)
  const int RED[] = {10, 0, 0};
  const int GREEN[] = {0, 10, 0};
  const int BLUE[] = {0, 0, 10};
  const int WHITE[] = {155, 155, 155};
  const int BLACK[] = {0, 0, 0};

  void setup()
  {
    metroPixel.begin();
  }

  // takes in a pre-defined color (integer array) and sets the pixel to that color
  void pixelWrite(const int *color)
  {
    metroPixel.setPixelColor(0, metroPixel.Color(color[0], color[1], color[2]));
    // write the pixel color to the Metro's Neopixel
    metroPixel.show();
  }

  // flashes the neopixel on and off rapidly
  void pixelFlash(const int *color)
  {
    for (int i = 0; i < 5; i++)
    {
      pixelWrite(color);
      delay(50);
      pixelWrite(BLACK);
      delay(50);
    }
  }

  // flashes the neopixel on and off rapidly
  void pixelSparkle()
  {
    for (int i = 0; i < 5; i++)
    {
      pixelWrite(BLACK);
      delay(50);
      pixelWrite(WHITE);
      delay(50);
    }
  }
}
#endif
