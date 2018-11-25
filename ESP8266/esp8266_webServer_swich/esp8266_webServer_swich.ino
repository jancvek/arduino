/*
 CODE FOR ESP8266 LoLin V3 modul SET FOR DRIVING SWICH WITH RF433

 NOTES:
 modul has bilt in LED on PIN 2
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

//-----!!!USER SETTINGS!!!--------
//set modul id
String moduleId = "2";

//set WIFI variables
const char* ssid = "Doma 3";
const char* password = "a1250283";
//------------END--------------

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

const int ledPin =  2;  

void setup()
{
  //set serial conn and led pin
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

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
  //ledBlink(3, 500, 200);

  //check connection on raspberry pi
  String httpReq = "http://192.168.0.101:8000/checkConn/?test=%2799.9%27";

  //send http request
  String response = sendHttpReq(httpReq);

  delay(100);
  
  if(response.indexOf("OK") > -1)
  {
     //blink for successful respberry pi conection
    //ledBlink(3, 500, 200);
  }
  
  Serial.print("Test Server response: ");
  Serial.println(response);

  server.on ( "/", handleRoot );
  server.on ( "/swi=1", handleRoot);
  server.on ( "/swi=0", handleRoot);
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
server.handleClient();
}

//----------------FUNCTIONS-------------------

void handleRoot() {
  digitalWrite (LED_BUILTIN, LOW); //turn the built in LED on pin DO of NodeMCU on
  digitalWrite (ledPin, server.arg("swi").toInt());
  ledState = digitalRead(ledPin);

 /* Dynamically generate the LED toggle link, based on its current state (on or off)*/
  char ledText[80];
  
  if (ledState) {
    strcpy(ledText, "SWICH is ON. <a href=\"/?led=0\">Turn it OFF!</a>");
  }

  else {
    strcpy(ledText, "SWICH is OFF. <a href=\"/?led=1\">Turn it ON!</a>");
  }
 
  ledState = digitalRead(ledPin);

  char html[1000];

// Build an HTML page to display on the web-server root address
  snprintf ( html, 1000,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='10'/>\
        <title>ESP8266 WiFi Network</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
          h1 { Color: #AA0000; }\
        </style>\
      </head>\
      <body>\
        <h1>ESP8266 Wi-Fi Web Server Swich</h1>\
        <p>%s<p>\
      </body>\
    </html>",

    ledText
  );
  server.send ( 200, "text/html", html );
  digitalWrite ( LED_BUILTIN, 1 );
}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in LED on pin DO of NodeMCU off
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
