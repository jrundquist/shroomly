/** 
 * Defines the pins
 */
#ifndef SHROOMLY_PINS
#define SHROOMLY_PINS

#define EPD_DC 10    // can be any pin, but required!
#define EPD_CS 9     // can be any pin, but required!
#define EPD_BUSY 7   // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS 6    // can set to -1 to not use a pin (uses a lot of RAM!)
#define EPD_RESET 8  // can set to -1 and share with chip Reset (can't deep sleep)

#define HUMIDIFIER_PIN A0

#endif