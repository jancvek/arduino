#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* ssid = "Doma Zgoraj"; //replace this with your WiFi network name
const char* password = "a1250283"; //replace this with your WiFi network password

const int ledPin =  2;  

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  
  WiFi.begin(ssid, password);

  // Wait for serial to initialize.
  while (!Serial) { }

  ledBlink(2, 1000, 200);
  
  //wait to wifi connect
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  delay(100);

  //sleep for 120s
  ESP.deepSleep(20e6);
}

void loop() {
  // put your main code here, to run repeatedly:

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
