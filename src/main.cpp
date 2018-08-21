#define IGNORE_WIFI

#include <Arduino.h>

#if !(defined(IGNORE_WIFI))
  #include <WiFi.h>
#endif

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#define ROWS 19
#define COLS 16
#define NUM_LEDS ROWS*COLS

#define DATA_PIN 13

#define IGNORE_SERIAL

CRGB leds[NUM_LEDS];

#include "settings.h"

// function to orient correctly...
void light(uint8_t x, uint8_t y, uint8_t h) {
  // uint8_t realRow = ROWS - y + 1;
  // uint16_t pixel = realRow * COLS + (y - 1);
  uint16_t pixel = (ROWS-y+1)*COLS-(COLS-x+1);
  leds[pixel] = CHSV(h, 255, 255); 
}

void setup() {
  delay(1000); // 3 second delay for recovery
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  #if !(defined(IGNORE_SERIAL))
    Serial.begin(9600);
  #endif
  #if !(defined(IGNORE_WIFI))
    WiFi.begin(SETTINGS_NETWORK_SSID, SETTINGS_NETWORK_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
  #endif
}

static int hue=0;

void loop() {
  light(1, 1, 10);
  light(16, 19, 40);
  light(16, 1, 80);
  light(1, 19, 120);
  // light(2, 2, 60);
  // light(2, 3, 100);
 
  leds[5] = CHSV(10, 255, 255);
  FastLED.show();
  delay(5);
}
