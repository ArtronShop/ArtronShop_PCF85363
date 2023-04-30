# ArtronShop_PCF85363A

Arduino library of PCF85363A Real-Time Clock/calendar with 64 byte RAM

## Examples

initinitialize 

```C++
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
```

Set time

```C++
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
```

Get time

```C++
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
```

Full test


```C++
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

  // Test set time to RTC
  struct tm t;
  t.tm_hour = 23;
  t.tm_min = 59;
  t.tm_sec = 50;
  t.tm_mday = 28;
  t.tm_mon = 2 - 1; // 0 - 11
  t.tm_year = 2024 - 1900; // since 1900
  if (!rtc.setTime(t)) {
    Serial.println("Set time to RTC fail !");
  }
}

void loop() {
  // Test get time from RTC
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
```