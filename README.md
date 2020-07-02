# BME280
This library can be used
for reading out
sensordata of the bme280.
The library is capable of 
reading the temperature,
barometic pressure and
the altitude above 
sealevel, the only 
downside of my library is 
that the humidity measurement function 
doesn’t work.

For the demo application i have programmed
a test which provides an advice if you should
see a doctor or not! This is done via an i2c 
small i2c display. If the temperature of your 
breath is higher than 37 degrees centigrade
it will tell you to go and 
see a doctor. 
