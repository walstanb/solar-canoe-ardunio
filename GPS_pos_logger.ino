
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
const int chipSelect = 5;
const int recLED = 7;
String gpstext;
int gpscount = 0;
int gpsttlcount = 30;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
 
void setup()
  pinMode(recLED, OUTPUT);
  Serial.begin(115200);
  ss.begin(GPSBaud);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  digitalWrite(recLED, HIGH);
  delay(500);
  digitalWrite(recLED, LOW);
  delay(500);
  digitalWrite(recLED, HIGH);
  delay(500);
  digitalWrite(recLED, LOW);
  delay(500);
  digitalWrite(recLED, HIGH);
  delay(500);
  digitalWrite(recLED, LOW);
 
}
 
void loop()
{
  digitalWrite(recLED, LOW);
 
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      if (displayInfo() != "0")
      {
        gpstext = displayInfo();
 
        Serial.println(gpscount);
        if (gpscount == gpsttlcount) {
          digitalWrite(recLED, HIGH);
 
          // Opening the file on card for writing
          File dataFile = SD.open("gps_log_points.csv", FILE_WRITE);
 
          if (dataFile) {
            dataFile.println(gpstext);
            dataFile.close();
            Serial.println(gpstext);
        
          else {
            Serial.println("error opening datalog.txt");
          }
 
    
        gpscount = gpscount + 1;
        if (gpscount > gpsttlcount) {
          gpscount = 0;
        }
 
      }
 
 
}
String displayInfo()
  String gpsdata = "";
  if (gps.location.isValid())
  {
    gpsdata = String(gps.location.lat(), 6);
    gpsdata += (",");
    gpsdata += String(gps.location.lng(), 6);
    gpsdata += (",");
  }
  else
  {
    return "0";
  }
  if (gps.date.isValid())
  {
    gpsdata += String(gps.date.year());
    gpsdata += ("-");
    if (gps.date.month() < 10) gpsdata += ("0");
    gpsdata += String(gps.date.month());
    gpsdata += ("-");
    if (gps.date.day() < 10) gpsdata += ("0");
    gpsdata += String(gps.date.day());
  }
  else
  {
    return "0";
  }
  gpsdata += (" ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) gpsdata += ("0");
    gpsdata += String(gps.time.hour());
    gpsdata += (":");
    if (gps.time.minute() < 10) gpsdata += ("0");
    gpsdata += String(gps.time.minute());
    gpsdata += (":");
    if (gps.time.second() < 10) gpsdata += ("0");
    gpsdata += String(gps.time.second());
  }
  else
  {
    return "0";
  }
  return gpsdata;
}