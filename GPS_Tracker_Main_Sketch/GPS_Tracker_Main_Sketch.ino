#include <Wire.h>
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
File myFile;
  //Sets precedents for files
SoftwareSerial ss(4, 3);

void setup() {
 
  Serial.begin(9600);
  
  pinMode(10, OUTPUT);
  
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  
  if (SD.exists("Kayak.CSV")){
    SD.remove("Kayak.CSV");
  }
  
  myFile = SD.open("Kayak.CSV", FILE_WRITE);
  myFile.println("time,latitude,longitude,speed");
  myFile.close();
  
  ss.begin(9600);
}

void loop() {
  
  myFile = SD.open("Kayak.CSV", FILE_WRITE);
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
    float flat, flon, speed;
    int year;
    byte month, day, hour, minute, second;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second);
    char sz[32];
    speed = gps.speed();
       
    if (Serial) {
      Serial.print("LAT = ");
      Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 8);
      Serial.print(" LON = ");
      Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 8);
      Serial.println();
      Serial.print("SPEED = ");
      Serial.print(speed);
      Serial.print(" Time = ");
      sprintf(sz, "%02d:%02d:%02d",
        hour, minute, second);
      Serial.print(sz);
      Serial.println();
    }
    
    if (myFile){
      myFile.print(sz);
      myFile.print(",");
      myFile.print(flat, 8);
      myFile.print(",");
      myFile.print(flon, 8);
      myFile.print(",");
      myFile.print(speed, 2);
      myFile.println();
    }
    myFile.close();
  }
}

