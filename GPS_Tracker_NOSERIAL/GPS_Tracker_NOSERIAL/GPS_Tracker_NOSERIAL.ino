/*
Not to be used for debugging or testing, this file is 
lighter on the Arduino removing most Serial communications 
with the tethered computer. Reduces memory load for the 
arduinio for increased reliable perfomrance.
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
  
int dataNumber = 0, cyear, cmonth, cday, chour, cminute, csecond;
long huntime, previoushuntime, previousdeltax;
float previousFlat, previousFlon, totaldistance;
char * filename;

void setup() {
  
  Serial.begin(9600);
    //Only Serial coms in the sketch, without this, the sketch will not function
  
  ss.begin(9600);
    //Begin communications with GPS module, baud rate set to 9600, the standard for SKM 255
  
  delay(1000);
    //Wait a second to ensure communication is fixed, this may be unneccessary
  
  ss.print("$PMTK314,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*19<CR><LF>");
    //Ensuring appropriate NMEA sentences will be submitted from the unit
  
  pinMode(10, OUTPUT);
    //Begin communications with SD sheild, pin 10 output
    
  Wire.begin();
  rtc.begin();
    //Begins coms with RTC
    
  DateTime now = rtc.now();
  cmonth = now.month();
  cday = now.day();
  chour = now.hour();
  cminute = now.minute();
    //Grabbing values for time & date
  
  if ((cmonth > 9 and cday > 5) or (cmonth < 4 and cday < 6)) {
      chour = chour + 1;
  }
  
  delay(1000);
    //Wait a second
    
  filename = "AAAA.AAA";
  sprintf(filename, "%d%d%d%d.CSV", cmonth, cday, chour, cminute);
    //Sets filename
  
  myFile = SD.open(filename, FILE_WRITE);
    //Create new file
  myFile.println("type,name,desc,latitude,longitude");
    //Create file headers for CSV format
  myFile.flush();
    //Saves the data onto the file 
}

void loop() {
  
  bool newData = false;
  float flat, flon, altitude, speedmps, deltax, maxspeed = 0;
  int satellites, year, HDOP;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  long deltat;
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
    
    if ((cmonth > 9 and cday > 5) or (cmonth < 4 and cday < 6)) {
      chour = chour + 1;
    }
      //Daylight Savings error fixing for eastern Australia states, Uncomment if elsewhere
    
    
    huntime = (hour*360000) + (minute*6000) + (second*100) + hundredths;
      //Finding change in time between locks, should be 1 Second, using this instead of 1 second for speed calculations allows users to change fix time on GPS
    
    deltat = (huntime - previoushuntime)/100;
    previoushuntime = huntime;
    
    deltax  = TinyGPS::distance_between(flat, flon, previousFlat, previousFlon);
      //Retreiving the change in distance from the previous point
    
    if (deltax > 100){
      deltax = 0;
    }
    
    totaldistance = totaldistance + deltax;
      //Equating total distance travelled NOT displacement
    
    speedmps = deltax / deltat;
    
    if (speedmps > maxspeed) {
      maxspeed = speedmps;
    }
    
    sprintf(sz, "%d/%d/%d %d:%d", cday, cmonth, cyear, chour, cminute);
    
    if (myFile){
      myFile.print(dataType);
      myFile.print(",");
      myFile.print(sz);
        //Printing the time
      myFile.print(",SPEED = ");
      myFile.print(speedmps, 2);
      myFile.print("m/s   Altitude = ");
      myFile.print(altitude);
      myFile.print("m  Satellites = ");
      myFile.print(satellites);
      myFile.print(" Distance Travelled = ");
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
    
    dataNumber = dataNumber + 1;
      //Used to specify data type earlier in sketch
    previousFlat = flat;
    previousFlon = flon;
    previousdeltax = deltax;
      //Used for comparison between points
  }
}

