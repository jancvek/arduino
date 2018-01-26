#include <DS3231.h>
#include <SPI.h>
#include <Ethernet.h>

struct dataStruct{
  int syncId;
  int deviceId;
  int sensId;
  float value;
  String measureTime;
}myData;

//----USER SETTINGS-----
//set device id
int deviceId = 1;

//set server ip
String serverIp = "192.168.0.101";
//---------END--------


// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,0,101);  // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 113);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void setup() {
  // put your setup code here, to run once:
  
  // Setup Serial connection
  Serial.begin(115200);

  //--------ETHERNET SETUP-------------
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  
  if (client.connect(server, 80)) {
    Serial.println("connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  //-----------END-----------

  //---------RTC SETUP-----------
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}
  
  // Initialize the rtc object
  rtc.begin();  
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(17, 29, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(26, 01, 2018);   // Set the date to January 1st, 2014
  //----------END-----------
}

void loop() {
  // put your main code here, to run repeatedly:

   if (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {

    //TODO: get data from RF
    
    //TMP: set syncId
    myData.syncId = 1;
    //device id
    myData.deviceId = deviceId;
    //sens id
    myData.sensId = 1;
    //value
    myData.value = 22.50;
  
    //get datetime from RTC
    String rtcDate = rtc.getDateStr(FORMAT_LONG,FORMAT_BIGENDIAN,'-');
    String rtcTime = rtc.getTimeStr();
    rtcTime.replace(":","%3A");
    String dateTime = rtcDate +"%20"+ rtcTime;

    myData.measureTime = dateTime; 
    Serial.println(myData.measureTime);  
    delay (10);
 
    if(!sentDataToServer(myData))
    {
      //TODO: save data for later sending
      Serial.println("Failed sending");
    }
  }
}

bool sentDataToServer(dataStruct myData)
{
  //bild POST body data
  String postData = "syncId="+String(myData.syncId)+"&deviceId="+String(myData.deviceId)+"&sensId="+String(myData.sensId)+"&value="+String(myData.value)+"&measureTime="+myData.measureTime;

  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  
  if (client.connect(server, 80)) {
      Serial.println("connected!");
     
      //store_sensors_data.php
      // Make a HTTP request:
      client.println("POST /main/store_sensors_data.php HTTP/1.1");
      client.print("Host: ");
      client.println(serverIp);   
      client.print("Content-Length: ");
      client.println(postData.length());
      client.println("Cache-Control: no-cache");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println();
      client.print(postData);
      client.println();   

      lastConnectionTime = millis();
      // note the time that the connection was made:
      
    } else {
      Serial.println("connection failed");
      lastConnectionTime = millis();
      return false;
    }
    
}

