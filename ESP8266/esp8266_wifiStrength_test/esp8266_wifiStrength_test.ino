/* RSSI:
 * -30dBm -> Amazing
 * -67dBm -> Very Good
 * -70dBm -> Okey
 * -80dBm -> Not Good (Minimum signal strength for basic connectivity)
 * -90dBm -> Unusable
 */

#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* ssid = "Doma 3"; //replace this with your WiFi network name
const char* password = "a1250283"; //replace this with your WiFi network password

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

  if (WiFi.status() == WL_CONNECTED)
  {
    // Measure Signal Strength (RSSI) of Wi-Fi connection
    long rssi = WiFi.RSSI();

    Serial.print("RSSI: ");
    Serial.println(String(rssi));
  }

  delay(500);
}
