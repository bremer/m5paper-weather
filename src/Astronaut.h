/*
   Copyright (C) 2021 SFini, Matthias Bremer

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
  * @file Astronaut.h
  * 
  * Class for reading astronauts in space data.
  */
#pragma once
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Data.h"

class Astronaut
{
public:

protected:
   /* Calls the open-notify request and deserialisation the json data. */
   bool GetAstronautJsonDoc(DynamicJsonDocument &doc)
   {
      WiFiClient client;
      HTTPClient http;
      String     server;
      int        port;
      String     uri;
      
      server += "api.open-notify.org";
      port = 80;
      uri += "/astros.json";

      Serial.printf("Requesting %s to %s\n", uri.c_str(), server.c_str());

      client.stop();
      http.begin(client, server, port, uri);
      
      int httpCode = http.GET();
      
      Serial.printf("Read %d bytes\n", http.getSize());

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("GetAstronaut failed, error: %s", http.errorToString(httpCode).c_str());
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
   Astronaut()
   {
   }

   /* Start the request and the filling. */
   bool GetAstronauts(MyData &myData)
   {
      DynamicJsonDocument doc(5 * 1024);
   
      if (GetAstronautJsonDoc(doc)) {
         myData.astronauts = doc.as<JsonObject>()["number"].as<int>();
         return true;
      }
      return false;
   }
};
