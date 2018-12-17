/*
 CODE FOR ESP8266 LoLin V3 modul SET FOR DRIVING SWICH WITH RF433

 NOTES:
 modul has bilt in LED on PIN 2
 */
/*
  See for details:  
  https://github.com/sui77/rc-switch/

  NOTES:
  220V Swich from Lidl:
  Workzone
  Reciever: GT-FSI-07
  Transiever: GT-9000
  Frequency: 433

  Signal: 24Bit, Protocol: 5, PulseLength: 511us

  BUTTON # SENDS (24Bit) :
  #1 ON: 13817968, 13667504, 14368048, 13941056
  #1 OFF: 14327568, 14598288, 14242976, 13703968
  #2 ON: 13667508, 13817972, 14368052, 13941060
  #2 OFF: 14242980, 14598292, 14327572, 13703972
  #3 ON: 13941068, 14368060, 13667516, 13817980
  #3 OFF: 14598300, 14327580, 13703980, 14242988
  #4 ON: 13703970, 14327570, 14598290, 14242978
  #4 OFF: 13667506, 14368050, 13941058, 13817970
  #1-4 ON: 13703978, 14327578, 14598298, 14242986
  #1-4 OFF: 13667514, 14368058, 13941066, 13817978
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

//-----!!!USER SETTINGS!!!--------
//set modul id
String moduleId = "5";
String deviceName = "ESP_SWICH1";

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

  //---RF CONFIG---
  // Transmitter is connected to ESP8266 Pin D1  
  mySwitch.enableTransmit(5);
  
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(5);

  // Optional set pulse length.
   mySwitch.setPulseLength(511);
  
  // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(5);

  
  //connect to WIFI
  WiFi.hostname(deviceName);
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
  String httpReq = "http://192.168.0.102:8000/checkConn/?test=%2799.9%27";

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

void handleRoot() {
  digitalWrite (ledPin, LOW); //turn the built in LED on pin DO of NodeMCU on
  
  int cont = server.arg("swi").toInt();
  
  if(cont == 1)
  {
    cont = 0;
  }
  else if(cont == 0)
  {
    cont = 1;
  }
  else
  {
    cont = 0;
  }
  
  digitalWrite (ledPin, cont);
  bool ledState = digitalRead(ledPin);

 /* Dynamically generate the LED toggle link, based on its current state (on or off)*/
  char ledText[80];
  Serial.println(cont);
  if (cont==1) {
    strcpy(ledText, "SWICH is OFF. <a href=\"/?swi=1\">Turn it ON!</a>"); 
    mySwitch.send(14242976, 24);
  }

  else {
    strcpy(ledText, "SWICH is ON. <a href=\"/?swi=0\">Turn it OFF!</a>");
    mySwitch.send(14368048, 24);
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
