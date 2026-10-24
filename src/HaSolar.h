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
  * @file HaSolar.h
  * 
  * Class for reading solor power from Homeassistant.
  */
#pragma once
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Utils.h"
#include "Config.h"

class HaSolar
{
public:
   bool   success;                        // success of request

    String unit_of_measurement;
    String current_power;

protected:

   bool GetSolarCurrentJsonDoc(DynamicJsonDocument &doc)
   {
      WiFiClient client;
      HTTPClient http;
      String     uri;
      
      uri += "/api/states/sensor.solar_leistung_statistik";

      Serial.printf("Requesting %s to %s:%d\n", uri.c_str(), HOMEASSISTANT_SRV, HOMEASSISTANT_PORT);

      client.stop();
      
      http.begin(client, HOMEASSISTANT_SRV, HOMEASSISTANT_PORT, uri);
      http.addHeader("Authorization", "Bearer " + String(HOMEASSISTANT_API));
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.GET();
      
      Serial.printf("Read %d bytes\n", http.getSize());

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("GetSolar failed, HTTP-Code: %s\n", http.errorToString(httpCode).c_str());
         Serial.printf("Body: %s\n", http.getString().c_str());
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
   
   bool GetSolarTodayJsonDoc(DynamicJsonDocument &doc)
   {
      WiFiClient client;
      HTTPClient http;
      String     uri;
      
      uri += "/api/states/sensor.solar_heutige_erzeugung";

      Serial.printf("Requesting %s to %s:%d\n", uri.c_str(), HOMEASSISTANT_SRV, HOMEASSISTANT_PORT);

      client.stop();
      
      http.begin(client, HOMEASSISTANT_SRV, HOMEASSISTANT_PORT, uri);
      http.addHeader("Authorization", "Bearer " + String(HOMEASSISTANT_API));
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.GET();
      
      Serial.printf("Read %d bytes\n", http.getSize());

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("GetSolar failed, HTTP-Code: %s\n", http.errorToString(httpCode).c_str());
         Serial.printf("Body: %s\n", http.getString().c_str());
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


public:
   HaSolar()
   {
      Clear();
   }

   /* Clear the internal data. */
   void Clear()
   {
      unit_of_measurement = "";
      current_power  = "";   
    }

   /* Start the request and the filling. */
   bool GetSolar(MyData &myData)
   {
      DynamicJsonDocument doc(2 * 512);
      bool currentSuccess = false;
      bool todaySuccess = false;
   
      currentSuccess = GetSolarCurrentJsonDoc(doc);
      if (currentSuccess)
      {
         myData.solar_current_power = doc.as<JsonObject>()["state"].as<int>();
         myData.solar_current_unit = doc.as<JsonObject>()["attributes"]["unit_of_measurement"].as<String>();
      }
      
      todaySuccess = GetSolarTodayJsonDoc(doc);
      if (todaySuccess)
      {
         myData.solar_today_production = doc.as<JsonObject>()["state"].as<float>();
         myData.solar_today_unit = doc.as<JsonObject>()["attributes"]["unit_of_measurement"].as<String>();
      }
      
      myData.solar_success = (currentSuccess && todaySuccess);
      return myData.solar_success;
   }
};
