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
  * @file Maps.h
  * 
  * Class for reading Google Maps data.
  */
#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Data.h"
#include <Config.h>
#include "Logging.h"

class Maps
{
protected:
   const String server = "https://maps.googleapis.com";
   String uri_distance = "/maps/api/distancematrix/json?key=" + String(GOOGLE_API_KEY) 
      + "&language=de&departure_time=now&origins=" + String(HOME_COORD) + "|" + String(WORK_COORD) 
      + "&destinations="  + String(HOME_COORD) + "|" + String(WORK_COORD);

   bool GetMapsJsonDoc(DynamicJsonDocument &doc)
   {
      HTTPClient http;

      String uri = server + uri_distance;
      LOG_HTTP_REQUEST(uri);
      http.begin(uri);
      int httpCode = http.GET();
      LOG_HTTP_RESPONSE_CODE(httpCode);

      if (httpCode != HTTP_CODE_OK) {
         LOG_HTTP_ERROR(uri, httpCode, http);
         http.end();
         return false;
      } else {
         String payload = http.getString();
         DeserializationError error = deserializeJson(doc, payload);
         http.end();
         if (error) {
            LOG_JSON_ERROR(error, payload);
            return false;
         } else {
            return true;
         }
      }

   }

public:
   Maps()
   {
   }

   /* Start the request and the filling. */
   bool GetMaps(MyData &myData)
   {
      DynamicJsonDocument doc(5 * 1024);

      if (GetMapsJsonDoc(doc))
      {
         Serial.println("Maps status: " + String(doc.as<JsonObject>()["status"].as<const char *>()));
         myData.mapsWorkDurationInTraffic = doc.as<JsonObject>()["rows"][0]["elements"][1]["duration_in_traffic"]["value"].as<int>() / 60;
         myData.mapsHomeDurationInTraffic = doc.as<JsonObject>()["rows"][1]["elements"][0]["duration_in_traffic"]["value"].as<int>() / 60;
         return true;
      }
      return false;
   }
};
