#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* ssid = "Doma 3"; //replace this with your WiFi network name
const char* password = "a1250283"; //replace this with your WiFi network password

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for serial to initialize.
  while (!Serial) { }

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

  //sleep for 60s
  ESP.deepSleep(60e6);
}

void loop() {
  // put your main code here, to run repeatedly:

}
