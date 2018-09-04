// #define IGNORE_WIFI
// #define IGNORE_SERIAL

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>

#define I2C_SDA 21
#define I2C_SCL 22
// TwoWire.begin(I2C_SDA, I2C_SCL);
TwoWire myWire(0);

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>          
#else
#include <WiFi.h>          
#endif

//needed for library
#include <DNSServer.h>
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <WiFiManager.h> 

// for ntp time gathering...
#define TIME_OFFSET -25200
// #define UPDATE_INTERVAL 60000
#define UPDATE_INTERVAL 60
WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP);
NTPClient timeClient(ntpUDP, "time.google.com", TIME_OFFSET, UPDATE_INTERVAL);

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#define ROWS 19
#define COLS 16
#define NUM_LEDS ROWS*COLS

#define DATA_PIN 12

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

// setup words...
const PROGMEM int w_IT[] = {1, 1, 2};
const PROGMEM int w_IS[] = {4, 1, 2};
const PROGMEM int w_A[] = {7, 1, 1};
const PROGMEM int w_HALF[] = {8, 8, 4};
const PROGMEM int w_PAST[] = {13, 8, 4};
const PROGMEM int w_TO[] = {1, 9, 2};
const PROGMEM int w_NOON[] = {6, 14, 4};
const PROGMEM int w_MIDNIGHT[] = {2, 15, 8};
const PROGMEM int w_MINUTE[] = {1, 8, 6};
const PROGMEM int w_MINUTES[] = {1, 8, 7};
const PROGMEM int w_OCLOCK[] = {7, 12, 7};
const PROGMEM int w_QUARTER[] = {10, 7, 7};
const PROGMEM int w_IN[] = {3, 13, 2};
const PROGMEM int w_THE[] = {6, 13, 3};
const PROGMEM int w_MORNING[] = {10, 13, 7};
const PROGMEM int w_AT[] = {1, 13, 2};
const PROGMEM int w_AFTERNOON[] = {1, 14, 9};
const PROGMEM int w_EVENING[] = {10, 14, 7};
const PROGMEM int w_NIGHT[] = {5, 15, 5};

const PROGMEM int HOURS[][3] = {
  {1, 1, 0}, // nothing...
  {6, 9, 3}, // one
  {4, 9, 3}, // two
  {1, 12, 5}, // three
  {7, 11, 4}, // four
  {11, 11, 4}, // five
  {14, 9, 3}, // six
  {2, 10, 5}, // seven
  {9, 10, 5}, // eight
  {6, 10, 4}, // nine
  {13, 10, 3}, // ten
  {8, 9, 6}, // eleven
  {1, 11, 6} // twelve
};

const PROGMEM int MINUTES[][3] = {
  {7, 12, 7}, // o'clock
  {14, 2, 3}, // one (minute past)
  {5, 3, 3},  // two (minutes past)
  {12, 4, 5}, // three (minutes past)
  {8, 2, 4}, // four (minutes past)
  {1, 3, 4}, // five (minutes past)
  {1, 6, 3}, // six (minutes past)
  {1, 7, 5}, // seven (minutes past)
  {9, 6, 5}, // eight (minutes past)
  {1, 5, 4}, // nine (minutes past)
  {14, 1, 3}, // ten (minutes past)
  {6, 4, 6}, // eleven (minutes past)
  {1, 4, 6}, // twelve (minutes past)
  {8, 3, 8}, // thirteen (minutes past)
  {9, 5, 8}, // fourteen (minutes past)
  {10, 7, 7}, // quarter
  {1, 6, 7}, // sixteen (minutes past)
  {1, 7, 9}, // seventeen (minutes past)
  {9, 6, 8}, // eighteen (minutes past)
  {1, 5, 8}, // nineteen (minutes past)
  {8, 1, 6}, // twenty (minutes past)

  {14, 2, 3}, // one (twenty)
  {5, 3, 3},  // two (twenty)
  {12, 4, 5}, // three (twenty)
  {8, 2, 4}, // four (twenty)
  {1, 3, 4}, // five (twenty)
  {1, 6, 3}, // six (twenty)
  {1, 7, 5}, // seven (twenty)
  {9, 6, 5}, // eight (twenty)
  {1, 5, 4}, // nine (twenty)

  {1, 2, 6}, // thirty
  // {8, 8, 4}, // half

  {14, 2, 3}, // one (thirty)
  {5, 3, 3},  // two (thirty)
  {12, 4, 5}, // three (thirty)
  {8, 2, 4}, // four (thirty)
  {1, 3, 4}, // five (thirty)
  {1, 6, 3}, // six (thirty)
  {1, 7, 5}, // seven (thirty)
  {9, 6, 5}, // eight (thirty)
  {1, 5, 4}, // nine (thirty)

  {8, 1, 6}, // twenty (till)

  {14, 2, 3}, // one (twenty till)
  {5, 3, 3},  // two (twenty till)
  {12, 4, 5}, // three (twenty till)
  {8, 2, 4}, // four (twenty till)

  {10, 7, 7}, // quarter (till)

  {1, 6, 3}, // six (twenty till)
  {1, 7, 5}, // seven (twenty till)
  {9, 6, 5}, // eight (twenty till)
  {1, 5, 4}, // nine (twenty till)

  {14, 1, 3}, // ten (minutes till)
  {1, 5, 4}, // nine (minutes till)
  {9, 6, 5}, // eight (minutes till)
  {1, 7, 5}, // seven (minutes till)
  {1, 6, 3}, // six (minutes till)
  {1, 3, 4}, // five (minutes till)
  {8, 2, 4}, // four (minutes till)
  {12, 4, 5}, // three (minutes till)
  {5, 3, 3},  // two (minutes till)
  {14, 2, 3} // one (minute till)
};

static const unsigned char font_data[] = {
  0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x21, 0x54, 0x54, 0x78, 0x41,
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0xF0, 0x29, 0x24, 0x29, 0xF0,
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x32, 0x48, 0x48, 0x48, 0x32,
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x39, 0x44, 0x44, 0x44, 0x39,
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0xAA, 0x00, 0x55, 0x00, 0xAA,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0x7C, 0x2A, 0x2A, 0x3E, 0x14,
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00
};

// leds var...
CRGB leds[NUM_LEDS];

// real time clock var...
RTC_DS3231 rtc;

#include "settings.h"

// function to orient correctly...
void light(uint8_t x, uint8_t y, uint8_t h) {
  uint16_t pixel = (ROWS-y+1)*COLS-(COLS-x+1);
  leds[pixel] = CHSV(h, 255, 255); 
}

void lightWord(const int word[], int h) {
  for(int i = 0; i < word[2]; i++) {
    light(word[0]+i, word[1], h);
  }
}

void lightLetter(int letter, uint8_t h) {
  uint8_t l;
  uint8_t offsetX = 6;
  uint8_t offsetY = 10;
  for(int i = 0; i < 5; i++) {
    l = font_data[(letter * 5) + i];
    for(int j = 0; j < 8; j++) {
      if(CHECK_BIT(l, j)) {
        light(offsetX + i, offsetY + j, h);
      } else {
      }
    }
  }
}

void lightTime(uint8_t hours, uint8_t minutes) {
  uint8_t h = 80;
  bool past = false;
  lightWord(w_IT, 80);
  lightWord(w_IS, 80);

  switch(minutes) {
    case 0 : // this needs to be fixed...
      switch(hours) {
        case 0 :
          lightWord(w_MIDNIGHT, h);
          break;

        case 12 :
          lightWord(w_NOON, h);
          break;

        default :
          lightWord(w_OCLOCK, h);
          break;
      }
      past = false;
      break;

    case 1 : 
      lightWord(w_PAST, h);
      lightWord(w_MINUTE, h);
      lightWord(MINUTES[minutes], h);
      past = true;
      break;

    case 2 ... 14 :
      lightWord(w_PAST, h);
      lightWord(MINUTES[minutes], h);
      lightWord(w_MINUTES, h);
      past = true;
      break;

    case 15 :
      lightWord(w_A, h);
      lightWord(w_QUARTER, h);
      lightWord(w_PAST, h);
      past = true;
      break;

    case 16 ... 19 :
      lightWord(w_PAST, h);
      lightWord(MINUTES[minutes], h);
      lightWord(w_MINUTES, h);
      past = true;
      break;

    case 20 ... 29 :
      lightWord(w_PAST, h);
      lightWord(MINUTES[20], h);
      lightWord(MINUTES[minutes - 20], h);
      lightWord(w_MINUTES, h);
      past = true;
      break;

    case 30 :
      lightWord(w_HALF, h);
      lightWord(w_PAST, h);
      past = true;
      break;

    case 31 ... 39 :
      lightWord(w_PAST, h);
      lightWord(MINUTES[30], h);
      lightWord(MINUTES[minutes - 30], h);
      lightWord(w_MINUTES, h);
      past = true;
      break;

    case 40 ... 44 :
      lightWord(w_TO, h);
      lightWord(MINUTES[minutes - 40], h);
      lightWord(w_MINUTES, h);
      past = false;
      break;

    case 45 :
      lightWord(w_A, h);
      lightWord(w_QUARTER, h);
      lightWord(w_TO, h);
      past = false;
      break;

    case 46 ... 58 :
      lightWord(w_TO, h);
      lightWord(MINUTES[minutes - 40], h);
      lightWord(w_MINUTES, h);
      past = false;
      break;

    case 59 :
      lightWord(w_TO, h);
      lightWord(MINUTES[minutes - 40], h);
      lightWord(w_MINUTE, h);
      past = false;
      break;

  }

  if(!past) {
    hours++;
    if(hours > 23) hours = 0;
  }

  switch(hours) {
    case 0 :
      lightWord(w_MIDNIGHT, h);
      break;
    case 1 ... 11 :
      lightWord(HOURS[hours], h);
      lightWord(w_IN, h);
      lightWord(w_THE, h);
      lightWord(w_MORNING, h);
      break;
    case 12 :
      lightWord(w_NOON, h);
      break;
    case 13 ... 17 :
      lightWord(HOURS[hours - 12], h);
      lightWord(w_IN, h);
      lightWord(w_THE, h);
      lightWord(w_AFTERNOON, h);
      break;
    case 18 ... 19 :
      lightWord(HOURS[hours - 12], h);
      lightWord(w_IN, h);
      lightWord(w_THE, h);
      lightWord(w_EVENING, h);
      break;
    case 20 ... 23 :
      lightWord(HOURS[hours - 12], h);
      lightWord(w_AT, h);
      lightWord(w_NIGHT, h);
      break;
  }

}

void setup() {
  delay(1000); // 3 second delay for recovery
  myWire.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.begin(9600);
  Serial.println("Serial Started...");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  // begin wifi with wifimanager...
  WiFiManager wifiManager;
  wifiManager.autoConnect(SETTINGS_NETWORK_SSID, SETTINGS_NETWORK_PASS);
  // wifiManager.startConfigPortal("WordClockAP");

  // begin real time clock...
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // load time from network if rtc has lost power...
  if (rtc.lostPower()) {
    Serial.println("RTC lost power- set time from NTP");
    timeClient.update();
    delay(10);
    Serial.println(timeClient.getFormattedTime());
    timeClient.forceUpdate();
    delay(10);
    Serial.println(timeClient.getFormattedTime());
    setTime(timeClient.getEpochTime());
    rtc.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
  }

  // set a now var...
  DateTime now = rtc.now();
  setTime(now.unixtime());
  delay(500);
}

static uint8_t hue = 0;
static int a = 0;
static int b = 0;

void loop() {
  hue++;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  // light(1, 1, 10);
  // light(16, 19, 40);
  // light(16, 1, 80);
  // light(1, 19, 120);
  // light(2, 2, 60);
  // light(2, 3, 100);
 

  Serial.print(year(), DEC);
  Serial.print('/');
  Serial.print(month(), DEC);
  Serial.print('/');
  Serial.print(day(), DEC);
  Serial.print(" (");
  Serial.print(monthStr(month()));
  Serial.print(") ");
  Serial.print(hour(), DEC);
  Serial.print(':');
  Serial.print(minute(), DEC);
  Serial.print(':');
  Serial.print(second(), DEC);
  Serial.println();

  // if(a > 11) a=0;
  // // Serial.println(a);
  // lightWord(HOURS[a], 120);
  // a++;

  // if(b > 59) b=0;
  // // Serial.println(b);
  // lightWord(MINUTES[b], 200);
  // b++;

  if(b > 59) b=0;
  lightTime(hour(), minute());
  b++;

  // lightLetter(64+b, hue);
  // Serial.println();
  // lightLetter(66);
  // Serial.println();
  // lightLetter(67);
  // Serial.println();
  // lightLetter(68);
  // Serial.println();

  FastLED.show();

  delay(1200);
}
