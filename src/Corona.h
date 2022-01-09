/*
   Copyright (C) 2021 Matthias Bremer

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
  * @file Corona.h
  * 
  * Class for reading Corona data.
  */
#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Data.h"
#include <Config.h>

class Corona
{
protected:
   const String district = CORONA_AGS;
   const String server = "https://api.corona-zahlen.org";
   const String uri_district = "/districts/" + district;
   const String uri_germany = "/germany";

   bool GetCoronaLocalJsonDoc(DynamicJsonDocument &doc)
   {
      HTTPClient http;

      String uri = server + uri_district;
      Serial.printf("Requesting %s\n", uri.c_str());
      http.begin(uri);
      int httpCode = http.GET();

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("Error on requesting %s: %s\n", uri.c_str(), http.errorToString(httpCode).c_str());
         http.end();
         return false;
      } else {
         DeserializationError error = deserializeJson(doc, http.getStream());
         http.end();
         if (error) {
            Serial.printf("deserializeJson() failed: %s", error.c_str());
            return false;
         } else {
            return true;
         }
      }

   }

   bool GetCoronaGermanyJsonDoc(DynamicJsonDocument &doc)
   {
      HTTPClient http;

      String uri = server + uri_germany;
      Serial.printf("Requesting %s\n", uri.c_str());
      http.begin(uri);
      int httpCode = http.GET();

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("Error on requesting %s: %s\n", uri.c_str(), http.errorToString(httpCode).c_str());
         http.end();
         return false;
      } else {
         DeserializationError error = deserializeJson(doc, http.getStream());
         http.end();
         if (error) {
            Serial.printf("deserializeJson() failed: %s", error.c_str());
            return false;
         } else {
            return true;
         }
      }

   }

public:
   Corona()
   {
   }

   /* Start the request and the filling. */
   bool GetCorona(MyData &myData)
   {
      DynamicJsonDocument doc(5 * 1024);

      if (GetCoronaLocalJsonDoc(doc))
      {
         myData.coronaWeekIncidenceLocal = doc.as<JsonObject>()["data"][district]["weekIncidence"].as<float>();
         myData.coronaName = doc.as<JsonObject>()["data"][district]["name"].as<char *>();
         myData.coronaUpdated = doc.as<JsonObject>()["meta"]["lastUpdate"].as<char *>();
      }
      if (GetCoronaGermanyJsonDoc(doc))
      {
         myData.coronaWeekIncidenceGermany = doc.as<JsonObject>()["weekIncidence"].as<float>();
         return true;
      }
      return false;
   }
};
