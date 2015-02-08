#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

char * filename;
void setup() {
  Wire.begin();
  rtc.begin();
  
  DateTime now = rtc.now();
  
  
  int number = now.month();
  int letter = now.day();
  int cereal = now.hour();
  int carrot = now.minute();
  
  filename = "AAAA.AAA";
  sprintf(filename, "%d%d%d%d.CSV", number, letter, cereal, carrot);
  
  Serial.begin(9600);
  Serial.print(filename);
 
}

void loop() {
}
