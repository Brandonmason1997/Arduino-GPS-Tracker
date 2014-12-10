#include <Wire.h>
#include "RTClib.h"
  //Library for Real Time Clock, setting the file name
#include <TinyGPS++.h>
  //The cornerstone of the project
#include <SoftwareSerial.h>
  //Communicating with GPS I suppose
#include <SPI.h>
  //Something to do with Data Logging Sheild
#include <SD.h>
  //The SD Library

const int chipSelect = 10;
  //This is for the Datalogger to communicate with the Uno
static const int RXPin = 4, TXPin = 3;
  //Pins assigned for GPS
static const uint32_t GPSBaud = 9600;
  //GPS Baudrate
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
  // The serial connection to the GPS device
RTC_DS1307 rtc;
String fileName = "POOP";

void setup() {
  Wire.begin();
  rtc.begin();
  Serial.begin(9600);
  DateTime now = rtc.now();
}

void loop() {
  Serial.print(now.year(), DEC);
  Serial.print(fileName);
  delay(3000);
}
