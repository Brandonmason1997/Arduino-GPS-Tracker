#include <Wire.h>
#include "RTClib.h"
  //Library for Real Time Clock, setting the file name
#include <TinyGPS.h>
  //The cornerstone of the project
#include <SoftwareSerial.h>
  //Communicating with GPS I suppose
#include <SPI.h>
  //Something to do with Data Logging Sheild
#include <SD.h>
  //The SD Library

static const uint32_t GPSBaud = 9600;
  //GPS Baudrate
TinyGPS gps;
RTC_DS1307 rtc;
File myFile;
  //Sets precedents for files
SoftwareSerial ss(4, 3);
int repeat = 0;


void setup() {
 
  Serial.begin(9600);
  
  pinMode(10, OUTPUT);
  
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  
  if (SD.exists("kayak.CSV")){
    SD.remove("kayak.CSV");
  }
  
  myFile = SD.open("Kayak.CSV", FILE_WRITE);
  myFile.println("BeginingCSV");
  myFile.println();
  myFile.println("Latitude,Londitude,Speed");
  
  ss.begin(9600);
}

void loop() {
  bool newData = false;
  
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon, fkph;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    fkph = gps.f_speed_kmph();
    
    if (fkph == -1.00){
      fkph = 0.00;
    }
    
    Serial.print("LAT = ");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 8);
    Serial.print(" LON = ");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 8);
    Serial.println();
    Serial.print("SPEED = ");
    Serial.print(fkph);
    Serial.println();
    if (Serial){
      myFile.print(flat);
      myFile.print(",");
      myFile.print(flon);
      myFile.print(",");
      myFile.print(fkph);
      myFile.println();
    }
    else {
      myFile.close();
    }
    repeat = repeat + 1;
    
    if (!myFile) {
      Serial.print("Cannot Write To File");
    }
  }
 delay(1000);
}

