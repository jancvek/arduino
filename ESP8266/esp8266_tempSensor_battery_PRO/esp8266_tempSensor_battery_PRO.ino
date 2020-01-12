/*
 CODE FOR ESP8266 LoLin V3 modul WITH TEMP AND HUMIDITY SENSOR
 CODE IS MODIFIED FOR BATTERY (DEEPSLEEP)

 IMPORTANT: we need to connect D0 to RST 

 DEEPSLEEP RESET PIN:
 GPIO16 (D0) - RST
 
 SENSOR GY-213V-HTU21D PINS:
 V3.3 - VCC
 G - GND
 GPIO5 (D1) - SCL
 GPIO4 (D2) - SDA
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "Adafruit_Si7021.h"

//-----!!!USER SETTINGS!!!--------
//set modul id
String moduleId = "1";

//set default delay (5min)
int defaultDelay = 300000;

//set WIFI variables
const char* ssid = "Doma Zgoraj";
const char* password = "a1250283";

String server = "http://192.168.0.130:8888/saveData/";
//------------END--------------

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

  //connect to WIFI
  WiFi.begin(ssid, password);

  //wait a little to WIFI begin
  delay(500);

  //wait untill WIFI is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

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

  float delayInMicroS = defaultDelay*1e3;
  int delayInt = (int)delayInMicroS;

  Serial.print("Go to deepsleep for: ");
  Serial.println(delayInt);
   
  ESP.deepSleep(delayInt);
}

void loop() {
  Serial.println("Error! Deepsleep do not work!");
  delay(1000);
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
