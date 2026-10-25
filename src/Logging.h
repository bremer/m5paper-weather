/*
   Copyright (C) 2026 Matthias Bremer

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
 * @file Logging.h
 * 
 * Unified logging macros for HTTP requests and JSON deserialization.
 */
#pragma once
#include <HTTPClient.h>

// Request Logging
#define LOG_HTTP_REQUEST(url) Serial.printf("[HTTP] GET %s\n", (url).c_str())

// HTTP Response Code Logging
#define LOG_HTTP_RESPONSE_CODE(httpCode) Serial.printf("[HTTP] Response code: %d\n", httpCode)

// HTTP Error Logging
#define LOG_HTTP_ERROR(url, httpCode, http) \
  Serial.printf("[HTTP] ERROR %d (%s) for %s\n", httpCode, http.errorToString(httpCode).c_str(), (url).c_str()); \
  Serial.printf("[HTTP] Response: %s\n", http.getString().c_str())

// JSON Deserialization Error Logging
#define LOG_JSON_ERROR(error, payload) \
  Serial.printf("[JSON] ERROR: %s (Code: %s)\n", (error).c_str(), String((error).code())); \
  Serial.printf("[JSON] Payload: %s\n", (payload).c_str())

// Bytes received Logging
#define LOG_HTTP_BYTES(bytes) Serial.printf("[HTTP] Received %d bytes\n", bytes)
