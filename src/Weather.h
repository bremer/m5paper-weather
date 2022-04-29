/*
   Copyright (C) 2021 SFini, mbremer

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
  * @file Weather.h
  * 
  * Class for reading all the weather data from openweathermap.
  */
#pragma once
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Utils.h"

#define MAX_FORECAST_DAILY 5
#define MAX_FORECAST_HORLY 25
#define MIN_RAIN 10

/**
  * Class for reading all the weather data from openweathermap.
  */
class Weather
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

   time_t dailyTime[MAX_FORECAST_DAILY];          //!< timestamp of the hourly forecast
   float  dailyMaxTemp[MAX_FORECAST_DAILY];       //!< max temperature forecast
   String dailyMain[MAX_FORECAST_DAILY];          //!< description of the hourly forecast
   String dailyIcon[MAX_FORECAST_DAILY];          //!< openweathermap icon of the forecast weather

   int    maxRain;                         //!< maximum rain in mm of the hourly forecast
   int    maxTemp;                         //!< maximum temp in C of the hourly forecast
   int    minTemp;                         //!< minimum temp in C of the hourly forecast
   float  forecastHourlyTemp[MAX_FORECAST_HORLY];   //!< hourly temperature
   float  forecastHourlyRain[MAX_FORECAST_HORLY];      //!< rain in mm/h
   float  forecastHourlySnow[MAX_FORECAST_HORLY];      //!< rain in mm/h

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
      
      uri += "/data/2.5/onecall";
      uri += "?lat=" + String((float) LATITUDE, 5);
      uri += "&lon=" + String((float) LONGITUDE, 5);
      uri += "&units=metric&lang=de&exclude=minutely";
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

      currentTimeOffset = root["timezone_offset"].as<int>();
      currentTime       = LocalTime(root["current"]["dt"].as<int>());

      sunrise           = LocalTime(root["current"]["sunrise"].as<int>());
      sunset            = LocalTime(root["current"]["sunset"].as<int>());
      windspeed         = root["current"]["wind_speed"].as<float>();
      temp              = root["current"]["temp"].as<float>();
      tempFeelsLike     = root["current"]["feels_like"].as<float>();
      humidity          = root["current"]["humidity"].as<float>();

      JsonArray daily_list = root["daily"];
      for (int i = 0; i < MAX_FORECAST_DAILY; i++) {
         if (i < daily_list.size()) {
            dailyTime[i]    = LocalTime(daily_list[i]["dt"].as<int>());
            dailyMaxTemp[i] = daily_list[i]["temp"]["max"].as<float>();
            dailyMain[i]    = daily_list[i]["weather"][0]["main"].as<char *>();
            dailyIcon[i]    = daily_list[i]["weather"][0]["icon"].as<char *>();
         }
      }

      JsonArray hourly_list = root["hourly"];
      maxRain = 1;
      minTemp = 0;
      maxTemp = 5;
      for (int i = 0; i < MAX_FORECAST_HORLY; i++) {
         if (i < hourly_list.size()) {
            forecastHourlyTemp[i]  = hourly_list[i]["temp"].as<float>();
            if (forecastHourlyTemp[i] > maxTemp) {
               maxTemp = forecastHourlyTemp[i] + 1;
            }
            if (forecastHourlyTemp[i] < minTemp) {
               minTemp = forecastHourlyTemp[i] - 1;
            }
            forecastHourlyRain[i]     = hourly_list[i]["rain"]["1h"].as<float>();
            if (forecastHourlyRain[i] > maxRain) {
               maxRain = forecastHourlyRain[i] + 1;
            }
            forecastHourlySnow[i]     = hourly_list[i]["snow"]["1h"].as<float>();
            if (forecastHourlySnow[i] > maxRain) {
               maxRain = forecastHourlySnow[i] + 1;
            }
         }
      } 
      

      return true;
   }

public:
   Weather()
      : currentTime(0)
      , currentTimeOffset(0)
      , sunrise(0)
      , sunset(0)
      , windspeed(0)
      , temp(0)
      , tempFeelsLike(0)
      , humidity(0)
      , maxRain(MIN_RAIN)
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
      maxRain           = MIN_RAIN;
      memset(dailyMaxTemp,       0, sizeof(dailyMaxTemp));
      memset(forecastHourlyTemp, 0, sizeof(forecastHourlyTemp));
      memset(forecastHourlyRain, 0, sizeof(forecastHourlyRain));
      memset(forecastHourlySnow, 0, sizeof(forecastHourlySnow));
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
