/*
   Copyright (C) 2021 SFini

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
  * @file Display.h
  * 
  * Main class for drawing the content to the e-paper display.
  */
#pragma once
#include "Data.h"
#include "Icons.h"

M5EPD_Canvas canvas(&M5.EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class WeatherDisplay
{
protected:
   MyData &myData; //!< Reference to the global data
   int maxX;       //!< Max width of the e-paper
   int maxY;       //!< Max height of the e-paper

protected:
   void DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom = 0, int32_t degTo = 360);
   // void DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int radius);

   void DrawIcon(int x, int y, const uint16_t *icon, int dx = 64, int dy = 64, bool highContrast = false);

   void DrawHead();
   void DrawRSSI(int x, int y);
   void DrawBattery(int x, int y);

   void DrawSunInfo(int x, int y, int dx, int dy);
   void DrawOutdoorInfo(int x, int y, int dx, int dy);
   void DrawIndoorInfo(int x, int y, int dx, int dy);
   void DrawStatusInfo(int x, int y, int dx, int dy);

   void DrawDaily(int x, int y, int dx, int dy, Weather &weather, int index);

   void DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[]);

public:
   WeatherDisplay(MyData &md, int x = 960, int y = 540)
       : myData(md), maxX(x), maxY(y)
   {
   }

   void Show();

   void ShowStatusInfo();
};

/* Draw a circle with optional start and end point */
void WeatherDisplay::DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */)
{
   for (int i = degFrom; i < degTo; i++)
   {
      double radians = i * PI / 180;
      double px = x + r * cos(radians);
      double py = y + r * sin(radians);

      canvas.drawPixel(px, py, color);
   }
}

/* Draw a the rssi value as circle parts */
void WeatherDisplay::DrawRSSI(int x, int y)
{
   int iQuality = WifiGetRssiAsQualityInt(myData.wifiRSSI);

   if (iQuality >= 80)
      DrawCircle(x + 12, y, 16, M5EPD_Canvas::G15, 225, 315);
   if (iQuality >= 40)
      DrawCircle(x + 12, y, 12, M5EPD_Canvas::G15, 225, 315);
   if (iQuality >= 20)
      DrawCircle(x + 12, y, 8, M5EPD_Canvas::G15, 225, 315);
   if (iQuality >= 10)
      DrawCircle(x + 12, y, 4, M5EPD_Canvas::G15, 225, 315);
   DrawCircle(x + 12, y, 2, M5EPD_Canvas::G15, 225, 315);
}

/* Draw a the battery icon */
void WeatherDisplay::DrawBattery(int x, int y)
{
   canvas.drawRect(x, y, 40, 16, M5EPD_Canvas::G15);
   canvas.drawRect(x + 40, y + 3, 4, 10, M5EPD_Canvas::G15);
   for (int i = x; i < x + 40; i++)
   {
      canvas.drawLine(i, y, i, y + 15, M5EPD_Canvas::G15);
      if ((i - x) * 100.0 / 40.0 > myData.batteryCapacity)
      {
         break;
      }
   }
}

/* Draw a the head */
void WeatherDisplay::DrawHead()
{
   canvas.drawString(String(myData.astronauts) + " Astronauten", 20, 10); // top left corner
   canvas.drawCentreString(CITY_NAME, maxX / 2, 10, 1);
   canvas.drawString(WifiGetRssiAsQuality(myData.wifiRSSI) + "%", maxX - 200, 10);
   DrawRSSI(maxX - 155, 25);
   canvas.drawString(String(myData.batteryCapacity) + "%", maxX - 110, 10);
   DrawBattery(maxX - 65, 10);
}

/* Draw one icon from the binary data */
void WeatherDisplay::DrawIcon(int x, int y, const uint16_t *icon, int dx /*= 64*/, int dy /*= 64*/, bool highContrast /*= false*/)
{
   for (int yi = 0; yi < dy; yi++)
   {
      for (int xi = 0; xi < dx; xi++)
      {
         uint16_t pixel = icon[yi * dx + xi];

         if (highContrast)
         {
            if (15 - (pixel / 4096) > 0)
               canvas.drawPixel(x + xi, y + yi, M5EPD_Canvas::G15);
         }
         else
         {
            canvas.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
         }
      }
   }
}

/* Draw the sun information with sunrise and sunset */
void WeatherDisplay::DrawSunInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("", x + dx / 2, y + 7, 1); // Sun headline
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.setTextSize(3);
   DrawIcon(x + 25, y + 55, (uint16_t *)SUNRISE64x64);
   canvas.drawString(getHourMinString(myData.weather.sunrise), x + 105, y + 80, 1);

   DrawIcon(x + 25, y + 150, (uint16_t *)SUNSET64x64);
   canvas.drawString(getHourMinString(myData.weather.sunset), x + 105, y + 175, 1);
}

/* Outdoor weather */
void WeatherDisplay::DrawOutdoorInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("Aussen", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.drawString("Wind " + String(toKmh(myData.weather.windspeed), 0) + " km/h", x + 10, y + 45, 1);

   DrawIcon(x + 25, y + 85, (uint16_t *)TEMPERATURE64x64);
   canvas.drawString(String(myData.weather.temp, 0) + " C", x + 105, y + 110, 1);
   canvas.setTextSize(2);
   canvas.drawString("gefuehlt " + String(myData.weather.tempFeelsLike, 0) + " C", x + 25, y + 150, 1);
    

   canvas.setTextSize(3);
   DrawIcon(x + 25, y + 180, (uint16_t *)HUMIDITY64x64);
   canvas.drawString(String(myData.weather.humidity, 0) + "%", x + 105, y + 205, 1);
}

/* Indoor temp and hum */
void WeatherDisplay::DrawIndoorInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("Innen", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.setTextSize(3);
   DrawIcon(x + 25, y + 85, (uint16_t *)TEMPERATURE64x64);
   canvas.drawString(String(myData.sht30Temperatur) + " C", x + 105, y + 110, 1);

   DrawIcon(x + 25, y + 180, (uint16_t *)HUMIDITY64x64);
   canvas.drawString(String(myData.sht30Humidity) + "%", x + 105, y + 205, 1);
}

void WeatherDisplay::DrawStatusInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("Status", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.setTextSize(3);
   canvas.drawCentreString(getRTCDateString(), x + dx / 2, y + 100, 1);
   canvas.drawCentreString(getRTCTimeString(), x + dx / 2, y + 140, 1);
   canvas.setTextSize(2);
   canvas.drawCentreString("updated", x + dx / 2, y + 120, 1);
}

/* Draw one hourly weather information */
void WeatherDisplay::DrawDaily(int x, int y, int dx, int dy, Weather &weather, int index)
{
   time_t time = weather.dailyTime[index];
   int temp = weather.dailyMaxTemp[index];
   String main = weather.dailyMain[index];
   String icon = weather.dailyIcon[index];

   canvas.setTextSize(2);

   char const *weekdays[] = {"", "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
   const char *wd = weekdays[weekday(time)];
   canvas.drawCentreString(wd, x + dx / 2, y + 10, 1);
   canvas.drawCentreString(String(temp) + " C", x + dx / 2, y + 30, 1);
   // canvas.drawCentreString(main,                        x + dx / 2, y + 70, 1);

   int iconX = x + dx / 2 - 32;
   int iconY = y + 50;

   if (icon == "01d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_01d, 64, 64, true);
   else if (icon == "01n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_03n, 64, 64, true);
   else if (icon == "02d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_02d, 64, 64, true);
   else if (icon == "02n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_02n, 64, 64, true);
   else if (icon == "03d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_03d, 64, 64, true);
   else if (icon == "03n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_03n, 64, 64, true);
   else if (icon == "04d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_04d, 64, 64, true);
   else if (icon == "04n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_03n, 64, 64, true);
   else if (icon == "09d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_09d, 64, 64, true);
   else if (icon == "09n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_09n, 64, 64, true);
   else if (icon == "10d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_10d, 64, 64, true);
   else if (icon == "10n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_03n, 64, 64, true);
   else if (icon == "11d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_11d, 64, 64, true);
   else if (icon == "11n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_11n, 64, 64, true);
   else if (icon == "13d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_13d, 64, 64, true);
   else if (icon == "13n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_13n, 64, 64, true);
   else if (icon == "50d")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_50d, 64, 64, true);
   else if (icon == "50n")
      DrawIcon(iconX, iconY, (uint16_t *)image_data_50n, 64, 64, true);
   else
      DrawIcon(iconX, iconY, (uint16_t *)image_data_unknown, 64, 64, true);
}

/* Draw a graph with x- and y-axis and values */
void WeatherDisplay::DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[])
{
   String yMinString = String(yMin);
   String yMaxString = String(yMax);
   int textWidth = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int graphX = x + 5 + textWidth + 5;
   int graphY = y + 35;
   int graphDX = dx - textWidth - 20;
   int graphDY = dy - 35 - 20;
   float xStep = graphDX / (xMax - xMin);
   int iOldX = 0;
   int iOldY = 0;

   canvas.setTextSize(2);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(1);
   canvas.drawString(yMaxString, x + 5, graphY - 5);
   canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);
   for (int i = 0; i <= (xMax - xMin); i++)
   {
      canvas.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);
   }

   canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);
   if (yMin < 0 && yMax > 0)
   { // null line?
      float yValueDX = (float)graphDY / (yMax - yMin);
      int yPos = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY)
         yPos = graphY + graphDY;
      if (yPos < graphY)
         yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10)
      {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);
      }
   }
   for (int i = xMin; i <= xMax; i++)
   {
      float yValue = values[i - xMin];
      float yValueDY = (float)graphDY / (yMax - yMin);
      int xPos = graphX + graphDX / (xMax - xMin) * i;
      int yPos = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY)
         yPos = graphY + graphDY;
      if (yPos < graphY)
         yPos = graphY;

      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      if (i > xMin)
      {
         canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);
      }
      iOldX = xPos;
      iOldY = yPos;
   }
}

/* Main function to show all the data to the e-paper */
void WeatherDisplay::Show()
{
   Serial.println("WeatherDisplay::Show");

   canvas.createCanvas(960, 540);

   canvas.setTextSize(2);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   DrawHead();

   // x = 960 y = 540
   // 540 - oben 35 - unten 10 = 495

   // top
   canvas.drawRect(14, 34, maxX - 28, maxY - 43, M5EPD_Canvas::G15);

   canvas.drawRect(15, 35, maxX - 30, 251, M5EPD_Canvas::G15);
   canvas.drawLine(232, 35, 232, 286, M5EPD_Canvas::G15);
   canvas.drawLine(465, 35, 465, 286, M5EPD_Canvas::G15);
   canvas.drawLine(697, 35, 697, 286, M5EPD_Canvas::G15);
   DrawSunInfo(15, 35, 217, 251);
   DrawOutdoorInfo(232, 35, 232, 251);
   DrawIndoorInfo(465, 35, 232, 251);
   DrawStatusInfo(697, 35, 245, 251);

   // middle bottom
   canvas.drawRect(15, 286, maxX - 30, 122, M5EPD_Canvas::G15);
   for (int x = 13, i = 0; i < 4; x += 113, i += 1)
   {
      // canvas.drawLine(x, 286, x, 408, M5EPD_Canvas::G15);
      DrawDaily(x, 286, 113, 122, myData.weather, i);
      canvas.drawLine(x + 113, 286, x + 113, 408, M5EPD_Canvas::G15);
   }

   // bottom
   canvas.drawRect(15, 408, maxX - 30, 122, M5EPD_Canvas::G15);
   canvas.setTextSize(1);
   canvas.drawString("stuendlich", 20, 410);
   DrawGraph(13, 415, 210, 115, "Temp. (C)", 0, 5, -10, 30, myData.weather.forecastHourlyTemp);
   DrawGraph(240, 415, 210, 115, "Niederschlag (mm)", 0, 5, 0, myData.weather.maxRain, myData.weather.forecastHourlyRain);
   DrawGraph(240, 415, 210, 115, "Niederschlag (mm)", 0, 5, 0, myData.weather.maxRain, myData.weather.forecastHourlySnow);
   canvas.drawLine(465, 408, 465, 530, M5EPD_Canvas::G15);
   
   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
   delay(1000);
}

void WeatherDisplay::ShowStatusInfo()
{
   Serial.println("WeatherDisplay::ShowStatusInfo");

   canvas.createCanvas(245, 251);

   canvas.setTextSize(2);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   canvas.drawRect(0, 0, 245, 251, M5EPD_Canvas::G15);
   DrawStatusInfo(0, 0, 245, 251);

   canvas.pushCanvas(697, 35, UPDATE_MODE_GC16);
   delay(1000);
}
