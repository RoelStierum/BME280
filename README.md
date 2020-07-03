# BME280

## Introduction
This library can be used for reading out sensordata of the bme280.

The library is capable of reading the temperature, barometic pressure and the altitude above sealevel, the only downside of this library is that the humidity measurement function of the chip doesn’t work since it is not made, but this might be a good point for the ones who want to use a bmp280 the library might work on these chips(not confirmed).

## Demo
For the demo application i have programmed
a test which provides an advice if you should
see a doctor or not. This is done via a
small i2c oled display. If the temperature of your 
breath is higher than 37 degrees centigrade
it will tell you to go and 
see a doctor. 

## Files in this project
The two most important files of the library are 'bme280.hpp' and 'bme280.cpp'.
There is a main file. This is made to demonstrate the functions of this library on a small monochrome oled display.

## Needed libraries
For using this project you will definitely need the hwlib library, provided by user wovo (Wouter Van Ooijen https://github.com/wovo.) I have used the bmptk library also kindly provided by user wovo, i have used the library as a minimalist make-based development environment for programming small micro-controllers using GCC, C, C++. 

## Wiring
The bme280 chip should be connected as the diagram provided in this repository (filename: wiring_diagram).

## Comments about this project
User wovo commented about this project, the only optional changes on this project could be: making enum classes for the registers located in bme280.hpp (line 15 to 62), and changing some of the names of the registors.
