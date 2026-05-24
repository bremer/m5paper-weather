/*
   Copyright (C) 2021 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file main.cpp
  * 
  * Main file with setup() and loop()
  */

#include <M5EPD.h>
#include <time.h>
// Rename and fill ConfigTemplate.h
#include "Config.h"
#include "Data.h"
#include "Display.h"
#include "Battery.h"
#include "EPD.h"
#include "EPDWifi.h"
#include "SHT30.h"
#include "Utils.h"
#include "WeatherCurrent.h"
#include "Astronaut.h"
#include "OpenLigaLeague.h"
#include "OpenLigaWorldcup.h"
#include "HaSolar.h"
#include "Maps.h"

MyData         myData;            // The collection of the global data
Astronaut      astronaut;         // REST client for astonauts
OpenLigaLeague openLigaLeague;   // REST client for OpenLiga
OpenLigaWorldcup openLigaWorldcup; // REST client for OpenLiga
HaSolar        hasolar;          // REST client for homassistant solar
Maps           maps;             // google maps client
WeatherDisplay myDisplay(myData); // The global display helper class

bool SetRTCDateTime(MyData &myData) {
   time_t time = myData.weather.currentTime;

   if (time > 0) {
      rtc_time_t RTCtime;
      rtc_date_t RTCDate;
   
      Serial.println("Epochtime: " + String(time));
      
      RTCDate.year = year(time);
      RTCDate.mon  = month(time);
      RTCDate.day  = day(time);
      M5.RTC.setDate(&RTCDate);
   
      RTCtime.hour = hour(time);
      RTCtime.min  = minute(time);
      RTCtime.sec  = second(time);
      M5.RTC.setTime(&RTCtime);
      return true;
   } 
   return false;
}

boolean isTimeAfter(int8_t hour, int8_t minute){
   rtc_time_t RTCtime;
   M5.RTC.getTime(&RTCtime);
   if (RTCtime.hour > hour) {
      return true;
   } else if (RTCtime.hour == hour && RTCtime.min >= minute) {
      return true;
   }
   return false;
}

boolean isTimeBefore(int8_t hour, int8_t minute){
   rtc_time_t RTCtime;
   M5.RTC.getTime(&RTCtime);
   if (RTCtime.hour < hour) {
      return true;
   } else if (RTCtime.hour == hour && RTCtime.min < minute) {
      return true;
   }
   return false;
}

void getSleepTime(MyData &myData){
   rtc_time_t RTCtime;
   M5.RTC.getTime(&RTCtime);

   int sleepForMinutes = 30;

   if (!myData.weather.success) {
      sleepForMinutes = 2;
   } else if (isTimeBefore(5, 0) || isTimeAfter(23, 30)) {
      // less frequent update between 0:00 and 5:00 
      sleepForMinutes = min((5 - RTCtime.hour) * 60 + (60 - RTCtime.min), 120);
   }

   sleepForMinutes = max(sleepForMinutes, 2);
   myData.sleepForMinutes = sleepForMinutes;
}

void shutdown(int sleepForMinutes) {   
   ShutdownEPD(sleepForMinutes * 60);
}

/* Start and M5Paper instance */
void setup()
{
   InitEPD(false);
   if (StartWiFi(myData.wifiRSSI)) {
      GetBatteryValues(myData);
      GetSHT30Values(myData);
      if (myData.weather.Get()) {
         SetRTCDateTime(myData);
      }
      getSleepTime(myData);
      astronaut.GetAstronauts(myData);
      openLigaLeague.GetOpenLiga(myData);
      openLigaWorldcup.GetOpenLiga(myData);
      hasolar.GetSolar(myData);
      if(isTimeBefore(21, 0) && isTimeAfter(4, 0)) {
         maps.GetMaps(myData);
      }

      getSleepTime(myData);
      M5.EPD.Clear(true);
      myData.Dump();
      myDisplay.Show();
      StopWiFi();
   }

   shutdown(myData.sleepForMinutes);
}

/* Main loop. Never reached because of shutdown */
void loop()
{
}
