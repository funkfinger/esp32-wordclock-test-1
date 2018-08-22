// #define IGNORE_WIFI
// #define IGNORE_SERIAL

#include <Arduino.h>
#include <RTClib.h>
#include <TimeLib.h>

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
#define UPDATE_INTERVAL 60000
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com", TIME_OFFSET, UPDATE_INTERVAL);

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#define ROWS 19
#define COLS 16
#define NUM_LEDS ROWS*COLS

#define DATA_PIN 13

#define $IT [1, 2]


// leds var...
CRGB leds[NUM_LEDS];

// real time clock var...
RTC_DS3231 rtc;

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

  // begin real time clock...
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // load time from network if rtc has lost power...
  if (rtc.lostPower()) {
    Serial.println("RTC lost power- set time from NTP");
    timeClient.update();
    setTime(timeClient.getEpochTime());
    rtc.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
  }

  // set a now var...
  DateTime now = rtc.now();
  setTime(now.unixtime());
  delay(500);

  Serial.begin(9600);
  // WiFi.begin(SETTINGS_NETWORK_SSID, SETTINGS_NETWORK_PASS);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.println("Connecting to WiFi..");
  // }
  // Serial.println("Connected to the WiFi network");

  WiFiManager wifiManager;
  // wifiManager.autoConnect(SETTINGS_NETWORK_SSID, SETTINGS_NETWORK_PASS);
  wifiManager.startConfigPortal("WordClockAP");
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

  delay(5000);
}
