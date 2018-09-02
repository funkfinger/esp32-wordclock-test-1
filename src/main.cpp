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

// setup words...
const PROGMEM int w_IT[] = {1, 1, 2};
const PROGMEM int w_IS[] = {4, 1, 2};

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

  {8, 8, 4}, // half

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
  Serial.println("Serial Started...");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  // begin wifi with wifimanager...
  // WiFiManager wifiManager;
  // // wifiManager.autoConnect(SETTINGS_NETWORK_SSID, SETTINGS_NETWORK_PASS);
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
static int b = 0;

void loop() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  // light(1, 1, 10);
  // light(16, 19, 40);
  // light(16, 1, 80);
  // light(1, 19, 120);
  // light(2, 2, 60);
  // light(2, 3, 100);
 
  lightWord(w_IT, 80);
  lightWord(w_IS, 80);

  // leds[5] = CHSV(10, 255, 255);

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
  // lightWord(HOURS[a], 120);
  a++;

  if(b > 59) b=0;
  Serial.println(b);
  lightWord(MINUTES[b], 200);
  b++;

  FastLED.show();

  delay(1300);
}
