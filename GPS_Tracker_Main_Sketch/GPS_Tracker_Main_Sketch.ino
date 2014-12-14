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
File myFile;
char filename[] = "00000000.CSV";
  //Sets precedents for files
float maxSpeed = 0;
float distance = 0;



void setup() {
  Wire.begin();
  rtc.begin();
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  
  //Insert way of naming file here
    
  myFile = SD.open(filename, FILE_WRITE);
  Serial.print("Writing to test.txt...");
  myFile.println("testing 1, 2, 3.");
}




void loop() {
  DateTime now = rtc.now();
  Serial.print(now.hour(), DEC);
  Serial.print(now.minute(), DEC);
  Serial.print("\n");
  
  float latitude = gps.location.lat();
  float longditude = gps.location.lng();
  
  float currentSpeed = gps.speed.kmph();
  if (currentSpeed > maxSpeed) {
    maxSpeed = currentSpeed;
  }
  
  myFile.print(currentSpeed); 
  myFile.print(",");
  myFile.print(latitude);
  myFile.print(",");
  myFile.print(longditude);
  myFile.println();
  
  delay (1000);
}

