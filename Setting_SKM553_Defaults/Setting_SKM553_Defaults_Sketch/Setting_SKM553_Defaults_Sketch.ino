#include <TinyGPS.h>
#include <SoftwareSerial.h>

TinyGPS gps;
SoftwareSerial ss(4, 3);

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  ss.begin(9600);
  
  delay(1000);
  
  if (ss.available()) {
    Serial.println("GPS Initialisation Success");
  }
  
  else {
    Serial.println("GPS Initialisation Fail");
  }
  /*
  What I want:
    GGA
    GSA
    GSV
    RMC
    VTG
    ZDA
  */
  ss.print("$PMTK314,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0*2C<CR><LF>");
  
}

void loop() {
  while (ss.available()) {
    char c = ss.read();
    Serial.print(c);
  }
}
