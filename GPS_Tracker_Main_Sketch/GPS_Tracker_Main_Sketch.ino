/*
To be used for debugging, innovating or testing, this file is 
heavier on the Arduino due to Serial communications 
with the tethered computer. Increases memory load for the 
arduinio causing decreased reliable perfomrance.
*/

#include <TinyGPS.h>
  //The cornerstone of the project
#include <SoftwareSerial.h>
  //Communicating with GPS I suppose
#include <SPI.h>
  //Something to do with Data Logging Sheild
#include <SD.h>
  //The SD Library
#include <Wire.h>
  //Enabling Coms with RTC
#include "RTClib.h"
  //Enabling usage of RTC

RTC_DS1307 rtc;
TinyGPS gps;
File myFile;
  //Sets precedents for RTC, GPS and Files
SoftwareSerial ss(4, 3);
  //Establishes coms pins for GPS
int cyear, cmonth, cday, chour, cminute, dataNumber = 0, previoushuntime;
char * filename;
float maxspeed = 0, previousFlat, previousFlon, totaldistance;

void setup() {
  
  Serial.begin(9600);
    //Begin communications serial, baud rate of 9600

  ss.begin(9600);
    //Begin communications with GPS module, baud rate set to 9600, the standard for SKM 255
  
  delay(1000);
    //Wait a second to ensure communication is fixed, this may be unneccessary
  
  
  if (ss.available()) {
    Serial.println("GPS");
  }
  else {
    Serial.println("!GPS");
  }
    //If the GPS does not communicate, return an error through Serial, uncomment for diagnostics
  
  ss.print("$PMTK314,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*19<CR><LF>");
    //Ensuring appropriate NMEA sentences will be submitted from the unit
  
  pinMode(10, OUTPUT);
    //Begin communications with SD sheild, pin 10 output
  
  if (!SD.begin(10)) {
    Serial.println("!SD");
    return;
  }
  else {
    Serial.println("SD");
  }
    //If the SD did not initiate, return an error through serial, uncomment for diagnostics
  
  Wire.begin();
  rtc.begin();
    //Begins coms with RTC
    
  DateTime now = rtc.now();
  cmonth = now.month();
  cday = now.day();
  chour = now.hour();
  cminute = now.minute();
    //Grabbing values for time & date
  
  delay(1000);
    //Wait a second
    
  filename = "AAAA.AAA";
  sprintf(filename, "%d%d%d%d.CSV", cmonth, cday, chour, cminute);
    //Sets filename
  
  myFile = SD.open(filename, FILE_WRITE);
  //Serial.println(filename);
    //Create new file, uncomment for diagnostics
  myFile.println("type,name,desc,latitude,longitude");
    //Create file headers for CSV format
  myFile.flush();
    //Saves the data onto the file 
}

void loop() {
  
  bool newData = false;
  float flat, flon, altitude, speedmps, speedkmph, deltax;
  int year, HDOP, age, huntime;
  byte month, day, hour, minute, second, hundredths, satellites, csecond, deltat;
  char sz[32];
  String dataType;
    //Setting variables to be used later in program
  
  DateTime now = rtc.now();
  chour = now.hour();
  cminute = now.minute();
  csecond = now.second();
  cmonth = now.month();
  cyear = now.year();
  cday = now.day();
    //Fetching time & date
  
  while (ss.available())
  {
    char c = ss.read();
    // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
    if (gps.encode(c)){ // Did a new valid sentence come in?
      newData = true;
    }
  }
   
  if (newData) {
       
    gps.f_get_position(&flat, &flon);
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);
    HDOP = gps.hdop();
    altitude = gps.f_altitude();
    satellites = gps.satellites();
      //Grabbing data from NMEA stream
    
    if (dataNumber % 8 == 0) {
        dataType = "W";
    }
    else {
        dataType = "T";
    }
    /*Choosing datatype for GPSVisualiser, W for waypoints which allow more data, T for 
    trackpoints which show the path travelled
    */
    
    huntime = (hour*360000) + (minute*6000) + (second*100) + hundredths;
      //Finding change in time between locks, should be 1 Second, using this instead of 1 second for speed calculations allows users to change fix time on GPS
    
    deltat = (huntime - previoushuntime)/100;
    previoushuntime = huntime;
    
    deltax  = TinyGPS::distance_between(flat, flon, previousFlat, previousFlon);
      //Retreiving the change in distance from the previous point
         
    speedmps = deltax / deltat;
      /*Locks are made approximately once every second, finding distance over 2 locks and dividing that by time for the locks 
      gives more realistic results. Also allows errors in deltax to diluted errors in speed. Improving this could be done by 
      equating previous 5 points for speed, achieving an average, not instantaneous.
      */
    speedkmph = speedmps*3.6;
    
    if (speedkmph > maxspeed) {
      maxspeed = speedkmph;
    }
      
    if (Serial) {
      Serial.print(" LAT = ");
      Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 8);
      Serial.print(" LON = ");
      Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 8);
      Serial.println();
    }
    sprintf(sz, "%d/%d/%d %d:%d", cday, cmonth, cyear, chour, cminute);
    
    if (myFile and (deltax < 10000)){
      myFile.print(dataType);
      myFile.print(",");
      myFile.print(sz);
      myFile.print(",Speed = ");
      myFile.print(speedkmph, 2);
      myFile.print("km/h Max Speed = ");
      myFile.print(maxspeed, 2);
      myFile.print("km/h Altitude = ");
      myFile.print(altitude);
      myFile.print("m Satellites = ");
      myFile.print(satellites);
      myFile.print(" Distance = ");
      myFile.print(totaldistance);
      myFile.print(" m,");
      myFile.print(flat, 8);
      myFile.print(",");
      myFile.print(flon, 8);
      myFile.print(",");
      myFile.println();
    }
      //Write data to file in CSV
    
    myFile.flush();
      //Flush data to file, saving progress
    
    if (deltax > 10000){
      deltax = 0;
    }
    
    totaldistance = totaldistance + deltax;
      //Equating total distance travelled NOT displacement
    
    dataNumber = dataNumber + 1;
      //Used to specify data type earlier in sketch
    previousFlat = flat;
    previousFlon = flon;
      //Used for comparison between points
  }
}

