/*
 CODE FOR ESP8266 LoLin V3 modul WITH TEMP AND HUMIDITY SENSOR
 CODE IS FOR TESTING
 
 SENSOR GY-213V-HTU21D PINS:
 V3.3 - VCC
 G - GND
 GPIO5 (D1) - SCL
 GPIO4 (D2) - SDA
 */

#include "Adafruit_Si7021.h"

//sensor constructor
Adafruit_Si7021 sensor = Adafruit_Si7021();

void setup()
{
  //set serial conn
  Serial.begin(115200);

  //check SI7021 sensor
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    //TODO: goto sleep
  }
}

void loop() {
  //read sensor
  float temp = sensor.readTemperature();
  float humi = sensor.readHumidity();   

  Serial.print("Sensors values -> temp: ");
  Serial.print(temp);
  Serial.print(" humi: ");
  Serial.println(humi);

  delay(1000);
}
