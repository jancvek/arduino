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
String moduleId = "2";

//set default delay
int defaultDelay = 300000;

//set WIFI variables
const char* ssid = "Doma 3";
const char* password = "a1250283";
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

  //check connection on raspberry pi
  String httpReq = "http://192.168.0.101:8000/checkConn/?test=%2799.9%27";

  //send http request
  String response = sendHttpReq(httpReq);

  delay(100);
  
  if(response.indexOf("OK") > -1)
  {
     //blink for successful respberry pi conection
    ledBlink(3, 500, 200);
  }
  
  Serial.print("Test Server response: ");
  Serial.println(response);
}

void loop() {
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) 
  { 
    //read sensor
    float temp = sensor.readTemperature();
    float humi = sensor.readHumidity();   

    Serial.print("Sensors values -> temp: ");
    Serial.print(temp);
    Serial.print(" humi: ");
    Serial.println(humi);

    long rssi = WiFi.RSSI();
    
    String httpReq = "http://192.168.0.101:8000/input/?module_id="+moduleId+"&temp="+String(temp, 3)+"&humi="+String(humi, 3)+"&rssi="+String(rssi);

    String response = sendHttpReq(httpReq);   
    
    if(response == "")
    {
      Serial.println("Server response: Send request... ERROR!");
    }
    else if(response.indexOf("delay")>-1)
    {
      String getDelay = response.substring(response.indexOf("delay")+6);

      if(getDelay)
      {
        Serial.print("Server response: ");
        Serial.println(response);
        
        defaultDelay = getDelay.toInt();
        
        Serial.print("Server defined delay: ");
        Serial.println(String(defaultDelay));
      }
    }
    else
    {
      Serial.print("Server response: ");
      Serial.println(response);
    }
  }

  //in case of to low delay
  if(defaultDelay<10000)
  {
    Serial.print("Delay is set to low from server! Delay: ");
    Serial.println(defaultDelay);
    delay(10000);
  }
  else
  {
    delay(defaultDelay); 
  }
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
