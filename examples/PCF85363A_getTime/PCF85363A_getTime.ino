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
}

void loop() {
  // Get time from RTC
  struct tm t;
  if (rtc.getTime(&t)) {
    Serial.printf(
      "Datetime: %02d:%02d:%02d %d/%d/%d\n",
      t.tm_hour, t.tm_min, t.tm_sec,
      t.tm_mday, t.tm_mon + 1, t.tm_year + 1900
    );
  } else {
    Serial.println("Get time from RTC fail !");
  }
  delay(1000);
}
