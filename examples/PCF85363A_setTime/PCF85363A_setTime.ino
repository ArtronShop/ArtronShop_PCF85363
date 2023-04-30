#include <Arduino.h>
#include <ArtronShop_PCF85363A.h>
#include <Wire.h>
#include <time.h>

ArtronShop_PCF85363A rtc(&Wire);

void setup() {
  Serial.begin(115200);

  Wire.begin();
  while(!rtc.begin()) {
    Serial.println("PCF85363A init error !");
    delay(1000);
  }

  // Set time to RTC
  struct tm t;
  t.tm_hour = 23;
  t.tm_min = 59;
  t.tm_sec = 50;
  t.tm_mday = 30;
  t.tm_mon = 3 - 1; // 0 - 11
  t.tm_year = 2023 - 1900; // since 1900
  if (rtc.setTime(t)) {
    Serial.println("Set time successful!");
  } else {
    Serial.println("Set time to RTC fail !");
  }
}

void loop() {
  delay(1000);
}
