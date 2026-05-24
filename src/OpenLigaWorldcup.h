/*
   Copyright (C) 2023 Matthias Bremer

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
  * @file OpenLigaWorldcup.h
  * 
  * Class for reading Soccer data.
  */
#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Data.h"
#include <Config.h>

class OpenLigaWorldcup {
protected:
   const String server = "https://api.openligadb.de";
   const String pathNextMatch = "/getnextmatchbyleagueteam/" + String(WORLDCUP_LEAGUE_ID) + "/" + String(WORLDCUP_TEAM_ID);

   bool GetNextMatchJsonDoc(DynamicJsonDocument &doc) {
      HTTPClient http;

      String uri = server + pathNextMatch;
      Serial.printf("Requesting %s\n", uri.c_str());
      http.begin(uri);
      int httpCode = http.GET();

      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("Error on requesting %s: %s\n", uri.c_str(), http.errorToString(httpCode).c_str());
         http.end();
         return false;
      } else {
         String payload = http.getString();
         DeserializationError error = deserializeJson(doc, payload);
         http.end();
         if (error) {
            Serial.printf("deserializeJson() failed. %s Code:%s\n", error.c_str(), String(error.code()));
            Serial.printf("payload: %s\n", payload.c_str());
            return false;
         } else {
            return true;
         }
      }

   }

public:
   OpenLigaWorldcup() {
   }

   /* Start the request and the filling. */
   bool GetOpenLiga(MyData &myData) {
      DynamicJsonDocument doc(2 * 1024);

      if (GetNextMatchJsonDoc(doc)) {
         myData.worldcupNextTeam1 = doc.as<JsonObject>()["team1"]["teamName"].as<const char *>();
         myData.worldcupNextTeam2 = doc.as<JsonObject>()["team2"]["teamName"].as<const char *>();
         myData.worldcupNextTime = doc.as<JsonObject>()["matchDateTime"].as<const char *>();
         return true;
      }
      return false;
   }
};
