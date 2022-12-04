#include "./status_pixel.h"

StatusPixel statusPixel;

// metroPixel takes in both the number of pixels (1, the built-in) and the pin)
Adafruit_NeoPixel metroPixel = Adafruit_NeoPixel(NEOPIXEL_NUM, PIN_NEOPIXEL);

/// @brief Setup the pixel.
void StatusPixel::init()
{
  metroPixel.begin();
}

/// @brief  Sets the pixel to a given color
/// @param color
void StatusPixel::pixelWrite(const int *color)
{
  metroPixel.setPixelColor(0, metroPixel.Color(color[0], color[1], color[2]));
  // write the pixel color to the Metro's Neopixel
  metroPixel.show();
}

/// @brief  Sets the pixel to a given color
/// @param color
void StatusPixel::pixelBrightness(const int *color, float brightness)
{
  metroPixel.setPixelColor(0, metroPixel.Color(color[0] * brightness, color[1] * brightness, color[2] * brightness));
  // write the pixel color to the Metro's Neopixel
  metroPixel.show();
}

/// @brief Flashes the neopixel on and off rapidly
/// @param color
void StatusPixel::pixelFlash(const int *color)
{
  for (int i = 0; i < 5; i++)
  {
    pixelWrite(color);
    delay(50);
    pixelWrite(colors::BLACK);
    delay(50);
  }
}