// Include required libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);


// GPS Connections
static const int RXPin = 4, TXPin = 3;

// GPS Baud rate (change if required)
static const uint32_t GPSBaud = 9600;

// SD Card Select pin
const int chipSelect = 5;

// String to hold GPS data
String gpstext;

// GPS write delay counter variables
// Change gpsttlcount as required
int gpscount = 0;
int gpsttlcount = 60;

// TinyGPS++ object
TinyGPSPlus gps;

// SoftwareSerial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  
  // Start Serial Monitor for debugging
  Serial.begin(115200);

  // Start SoftwareSerial
  ss.begin(GPSBaud);

  if(!mag.begin())
  {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void loop()
{
  sensors_event_t event; 
  mag.getEvent(&event);
 


  // See if data available
  while (ss.available() > 0)
    if (gps.encode(ss.read()))

      // See if we have a complete GPS data string
      if (displayInfo() != "0")
      {

        // Get GPS string
        gpstext = displayInfo();

        // Get magnetic heading
        gpstext += String(event.magnetic.x);
        gpstext += (",");
        gpstext += String(event.magnetic.y);
        gpstext += (",");
        gpstext += String(event.magnetic.z);


        // Check GPS Count
        if (gpscount == gpsttlcount) {


          //Open the file on card for writing
          File dataFile = SD.open("gpslog.csv", FILE_WRITE);

          if (dataFile) {

            // If the file is available, write to it and close the file
            dataFile.println(gpstext);
            dataFile.close();

            // Serial print GPS string for debugging
            Serial.println(gpstext);
          }
          // If the file isn't open print an error message for debugging
          else {
            Serial.println("error opening gpslog.txt");
          }

        }
        // Increment GPS Count
        gpscount = gpscount + 1;
        if (gpscount > gpsttlcount) {
          gpscount = 0;
        }

      }


}

// Function to return GPS string
String displayInfo()
{
  // Define empty string to hold output
  String gpsdata = "";

  // Get Date
  if (gps.date.isValid())
  {
    gpsdata += String(gps.date.year());
    gpsdata += ("-");
    if (gps.date.month() < 10) gpsdata += ("0");
    gpsdata += String(gps.date.month());
    gpsdata += ("-");
    if (gps.date.day() < 10) gpsdata += ("0");
    gpsdata += String(gps.date.day());
    gpsdata += (",");
  }
  else
  {
    return "0";
  }

  // Get time
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
    gpsdata += (",");
  }
  else
  {
    return "0";
  }

  // Get latitude and longitude
  if (gps.location.isValid())
  {
    gpsdata += String(gps.location.lat());
    gpsdata += (",");
    gpsdata += String(gps.location.lng());
    gpsdata += (",");
  }
  else
  {
    return "0";
  }

    // Get altitude
    if (gps.altitude.isValid())
    {
        gpsdata += String(gps.altitude.meters());
        gpsdata += (",");
    }
    else
    {
        return "0";
    }

    // Get speed
    if (gps.speed.isValid())
    {
        gpsdata += String(gps.speed.kmph());
        gpsdata += (",");
    }
    else
    {
        return "0";
    }

    // Get satellites
    if (gps.satellites.isValid())
    {
        gpsdata += String(gps.satellites.value());
        gpsdata += (",");
    }
    else
    {
        return "0";
    }

  return gpsdata;
}