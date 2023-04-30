#include <Arduino.h>
#include <ArtronShop_PCF85363A.h>
#include <Wire.h>
#include <time.h>

/* Step of test this example
 *    1) Uplod code to your board
 *    2) Open Serial Monitor you will see number count up evary 1 sec
 *    2) Press RESET button on your board, you will see count is up continul after board boot again
*/

ArtronShop_PCF85363A rtc(&Wire);

uint8_t counter = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin();
  while(!rtc.begin()) {
    Serial.println("PCF85363A init error !");
    delay(1000);
  }

  counter = rtc.readRAM(0); // Read data from RAM address 0 and set to counter
}

void loop() {
  counter++; // count up
  rtc.writeRAM(0, counter); // write last counter value to RAM address 0
  Serial.println(counter); // show last counter
  delay(1000);
}
