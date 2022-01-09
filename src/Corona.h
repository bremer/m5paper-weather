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

class Corona
{
protected:
   const char *uri = "https://api.corona-zahlen.org/states/HB";

   bool GetCoronaJsonDoc(DynamicJsonDocument &doc)
   {
      HTTPClient http;

      Serial.printf("Requesting %s\n", uri);
      http.begin("https://api.corona-zahlen.org/states/HB");
      int httpCode = http.GET();

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("Error on requesting %s: %s\n", uri, http.errorToString(httpCode).c_str());
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

      if (GetCoronaJsonDoc(doc))
      {
         Serial.printf("Corona name: %s", doc.as<JsonObject>()["data"]["HB"]["name"].as<String>().c_str());
         myData.coronaWeekIncidenceHb = doc.as<JsonObject>()["data"]["HB"]["weekIncidence"].as<float>();
         myData.coronaName = doc.as<JsonObject>()["data"]["HB"]["name"].as<char *>();
         myData.coronaUpdated = doc.as<JsonObject>()["meta"]["lastUpdate"].as<char *>();
         return true;
      }
      return false;
   }
};
