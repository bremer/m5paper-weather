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
  * @file OpenLigaLeague.h
  * 
  * Class for reading Soccer data.
  */
#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Data.h"
#include <Config.h>
#include "Logging.h"

class OpenLigaLeague {
protected:
   const String server = "https://api.openligadb.de";
   int leagueId;
   int teamId;
   String league;
   String pathNextMatch;
   String pathSpieltag;

public:
   OpenLigaLeague(int leagueId, int teamId, const String &league)
      : leagueId(leagueId), teamId(teamId), league(league),
        pathNextMatch("/getnextmatchbyleagueteam/" + String(leagueId) + "/" + String(teamId)),
        pathSpieltag("/getcurrentgroup/" + league) {
   }

   bool GetNextMatchJsonDoc(DynamicJsonDocument &doc) {
      HTTPClient http;

      String uri = server + pathNextMatch;
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

   bool GetSpieltagJsonDoc(DynamicJsonDocument &doc) {
      HTTPClient http;

      String uri = server + pathSpieltag;
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
   OpenLigaLeague() : OpenLigaLeague(LEAGUE_M_ID, TEAM_M_ID, String(LEAGUE_M)) {
   }

   /* Start the request and the filling. */
   bool GetOpenLiga(LeagueData &data) {
      DynamicJsonDocument doc(2 * 1024);

      if (GetNextMatchJsonDoc(doc)) {
         data.leagueNextTeam1 = doc.as<JsonObject>()["team1"]["teamName"].as<const char *>();
         data.leagueNextTeam2 = doc.as<JsonObject>()["team2"]["teamName"].as<const char *>();
         data.leagueNextTime = doc.as<JsonObject>()["matchDateTime"].as<const char *>();
      }
      if (GetSpieltagJsonDoc(doc))
      {
         data.leagueSpieltag = doc.as<JsonObject>()["groupName"].as<const char *>();
         return true;
      }
      return false;
   }
};
