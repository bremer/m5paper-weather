# M5PaperWeather

Modified version of M5PaperWeather: https://github.com/Bastelschlumpf/M5PaperWeather
You can create your own icons with [LCD Image Converter](https://github.com/riuson/lcd-image-converter) 

## Features:

* Local weather forecast
* Astronauts currently in space 
* Coraona virus incidence
* Travel time e.g. to your work

## Getting Started

Build with platform.io.
You need to rename `ConfigTemplate.h` to `Config.h` and put your own infos in there.

## Add new Icons

Use cd-image-converter, choose a 64x64 image and convert it to C-Source-Code:

Settings:
* Options -> Conversion... -> Preset: Color R5G6B5
* Options -> Image -> Block size: 8 bit

Astronaut from https://www.flaticon.com/free-icon/astronaut_500379
Wind from https://github.com/erikflowers/weather-icons/

## Screen Shot

![ScreenShot](screenshot.jpeg)

