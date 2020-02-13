/*
 CODE FOR ESP8266 LoLin V3 modul WITH TEMP AND HUMIDITY SENSOR

 NOTES:
 modul has bilt in LED on PIN 2

 SENSOR GY-213V-HTU21D PINS:
 V3.3 - VCC
 G - GND
 D1 - SCL
 D2 - SDA
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "Adafruit_Si7021.h"

//-----!!!USER SETTINGS!!!--------
//set modul id
String moduleId = "1";

//set default delay (10min)
int defaultDelay = 600000;

//set WIFI variables
const char* ssid = "Doma Zgoraj";
const char* password = "a1250283";

String server = "http://192.168.0.130:8888/saveData/";
//------------END--------------

//sensor constructor
Adafruit_Si7021 sensor = Adafruit_Si7021();

const int ledPin =  2;  

void setup()
{
  //set serial conn and led pin
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  //check SI7021 sensor
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");

    while(true)
    {
      ledBlink(1, 50, 50);
    }
  }

  //connect to WIFI
  WiFi.begin(ssid, password);

  //wait a little to WIFI begin
  delay(500);

  //wait untill WIFI is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    ledBlink(3, 100, 100);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  //blink for successful wifi conection
  ledBlink(3, 500, 200);
}

void loop() {

  //wait untill WIFI is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  //read sensor
  float temp = sensor.readTemperature();
  float humi = sensor.readHumidity();   

  Serial.print("Sensors values -> temp: ");
  Serial.print(temp);
  Serial.print(" humi: ");
  Serial.println(humi);

  long rssi = WiFi.RSSI();
  
  String httpReq = server+"?id="+moduleId+"&temp="+String(temp, 3)+"&humi="+String(humi, 3)+"&rssi="+String(rssi);
  //String httpReq = server+"?id=1&temp=22.5&humi=65.5&rssi=55";

  String response = sendHttpReq(httpReq);   
    
  if(response == "")
  {
    Serial.println("Server response: Send request... ERROR!");
  }
  else
  {
    Serial.print("Server response: ");
    Serial.println(response);
  }

  delay(defaultDelay); 
}

//----------------FUNCTIONS-------------------

String sendHttpReq(String httpReq)
{
    String payload = "";
    
    HTTPClient http;  //Declare an object of class HTTPClient  
    
    http.begin(httpReq);  //Specify request destination
    int httpCode = http.GET();  //Send the request
    
    if (httpCode > 0) //Check the returning code 
    { 
      payload = http.getString();   //Get the request response payload 
    }
    
    http.end();   //Close connection

    return payload;
}

void ledBlink (int numOfBlinks, int lightOn, int lightOff)
{
  for(int i = 0;i<numOfBlinks;i++)
  {
    digitalWrite(ledPin, LOW);
    delay(lightOn);
    digitalWrite(ledPin, HIGH);
    delay(lightOff);
  }
}
