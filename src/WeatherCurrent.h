/*
   Copyright (C) 2024 SFini, mbremer

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
  * @file WeatherCurrent.h
  * 
  * Class for reading current weather data from openweathermap.
  */
#pragma once
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Utils.h"

/**
  * Class for reading all the weather data from openweathermap.
  */
class WeatherCurrent
{
public:
   bool   success;                        // success of request

   time_t currentTime;                     //!< Current timestamp
   int    currentTimeOffset;               //!< Current timezone

   time_t sunrise;                         //!< Sunrise timestamp
   time_t sunset;                          //!< Sunset timestamp
   float  windspeed;                       //!< Wind speed
   float  temp;                       
   float  tempFeelsLike;              
   float  humidity;                   

   String icon;          //!< weather icon

protected:
   /* Convert UTC time to local time */
   time_t LocalTime(time_t time)
   {
      return time + currentTimeOffset;
   }

   /* Calls the openweathermap request and deserialisation the json data. */
   bool GetOpenWeatherJsonDoc(DynamicJsonDocument &doc)
   {
      WiFiClient client;
      HTTPClient http;
      String     uri;
      
      uri += "/data/2.5/weather";
      uri += "?lat=" + String((float) LATITUDE, 5);
      uri += "&lon=" + String((float) LONGITUDE, 5);
      uri += "&units=metric&lang=de";
      uri += "&appid=" + (String) OPENWEATHER_API;

      Serial.printf("Requesting %s to %s:%d\n", uri.c_str(), OPENWEATHER_SRV, OPENWEATHER_PORT);

      client.stop();
      http.begin(client, OPENWEATHER_SRV, OPENWEATHER_PORT, uri);
      
      int httpCode = http.GET();
      
      Serial.printf("Read %d bytes\n", http.getSize());

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("GetWeather failed, error: %s", http.errorToString(httpCode).c_str());
         client.stop();
         http.end();
         return false;
      } else {
         DeserializationError error = deserializeJson(doc, http.getStream());
         
         if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return false;
         } else {
            return true;
         }
      }
   }

   /* Fill from the json data into the internal data. */
   bool Fill(const JsonObject &root) 
   {
      Clear();

      currentTimeOffset = root["timezone"].as<int>();
      currentTime       = LocalTime(root["dt"].as<int>());

      sunrise           = LocalTime(root["sys"]["sunrise"].as<int>());
      sunset            = LocalTime(root["sys"]["sunset"].as<int>());
      windspeed         = root["wind"]["speed"].as<float>();
      temp              = root["main"]["temp"].as<float>();
      tempFeelsLike     = root["main"]["feels_like"].as<float>();
      humidity          = root["main"]["humidity"].as<float>();
      icon              = root["weather"]["icon"].as<char *>();

      return true;
   }

public:
   WeatherCurrent()
      : currentTime(0)
      , currentTimeOffset(0)
      , sunrise(0)
      , sunset(0)
      , windspeed(0)
      , temp(0)
      , tempFeelsLike(0)
      , humidity(0)
      , icon(' ')
   {
      Clear();
   }

   /* Clear the internal data. */
   void Clear()
   {
      currentTime       = 0;
      currentTimeOffset = 0;
      sunrise           = 0;
      sunset            = 0;
      windspeed         = 0;
      temp              = 0;
      tempFeelsLike     = 0;
      humidity          = 0;
      icon              = ' ';
   }

   /* Start the request and the filling. */
   bool Get()
   {
      DynamicJsonDocument doc(35 * 1024);
   
      success = GetOpenWeatherJsonDoc(doc);
      success = success && Fill(doc.as<JsonObject>());
      
      return success;
   }
};
