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

#define DATA_PIN 13

// setup words...
const PROGMEM int w_IT[] = {1, 1, 2};
const PROGMEM int w_IS[] = {4, 1, 2};

// hours...
const PROGMEM int w_ONE[3]   = {6, 9, 3};
const PROGMEM int w_TWO[3]   = {4, 9, 3};
const PROGMEM int w_THREE[3] = {1, 12, 5};
const PROGMEM int w_FOUR[3]  = {6, 9, 3};
const PROGMEM int w_FIVE[3]  = {6, 9, 3};

const PROGMEM int HOURS[][3] = {
  {6, 9, 3}, // one
  {4, 9, 3}, // two
  {1, 12, 5}, // three
  {7, 11, 4}, // four
  {11, 11, 4}, // five
  {14, 9, 3}, // six
  {2, 10, 5}, // seven (beven)
  {9, 10, 5}, // eight
  {6, 10, 4}, // nine
  {13, 10, 3}, // ten
  {8, 9, 6}, // eleven
  {1, 11, 6} // twelve
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

void setup() {
  delay(1000); // 3 second delay for recovery
  myWire.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.begin(9600);

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

static int hue = 0;
static int a = 0;

void loop() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  light(1, 1, 10);
  light(16, 19, 40);
  light(16, 1, 80);
  light(1, 19, 120);
  // light(2, 2, 60);
  // light(2, 3, 100);
 
  lightWord(w_IT, 80);
  lightWord(w_IS, 80);

  leds[5] = CHSV(10, 255, 255);

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

  if(a > 11) a=0;
  Serial.println(a);
  lightWord(HOURS[a], 120);
  a++;

  FastLED.show();

  delay(700);
}
