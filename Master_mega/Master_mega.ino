/*
 CODE FOR ARDUINO MEGA AS MASTER 
  IMPORTANT: arduino mega use SOFT SPI due to using Ethernet shield on 
  hardware SPI 50, 51, 52, 53 (which are directly connected with ICSP). Library NRF24 
  have already implemented SOFT SPI but it is disabled by default. To enable SOFT SPI 
  uncomment #define SOFTSPI in NRF24_config.h file. Second include DigitalIO.h lib in 
  the code. Also is needed defined new SOFT SPI in NRF24_config.h file on digital pins: 5,6,7.
  
  This version of code do not have implemented SD functionality due to uncompatibility 
  between DigitalIO and SD library
  
  NRF24 PINS:
  VCC - 5V/3.3V (5V - with VCC module)
  D3 - CE
  D30 -CSN
  D7 - MI
  D6 - MO
  D5 - SCK

  RTC PINS:
  VCC - 5V/3.3V (better use 3.3V to not over charge the battery)
  SDA -SDA
  SCL - SCL

  ETHERNET USE PINS:
  ICPS/52,51,50 (10SS)

  SD CARD USE PINS:
  ICPS/52,51,50 (4SS)
*/

#include <DS3231.h>
#include <SPI.h>
#include <Ethernet.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "DigitalIO.h"

//----!!!USER SETTINGS!!!-----
//set device id
int deviceId = 1;

//set server ip
String serverIp = "192.168.0.101";
//---------END--------

//-----DATA STRUCT-------
struct dataStruct1{
  int syncId;
  int deviceId;
  int sensId;
  float value;
  String measureTime;
}myData;

struct dataStruct2{
  int deviceId;
  int sensId;
  float value;
}myRfData;

//-----------NRF24----------------
// Hardware configuration:
RF24 radio(3,30);

//set listen and read pipes up to 6 pipes
const uint64_t pipesRX[1] = { 0xF0F0F0F0AA }; 
//const uint64_t pipesTX[1] = { 0xF0F0F0F0BA }; NOT IN USE YET

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

unsigned long timeAfterFirstCheck = 0;             // last time you connected to the server, in milliseconds
const unsigned long timeInterval = 1L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

//TMP: for testing
int serverCalls = 0;
//TMP: end

void setup() {
  // put your setup code here, to run once:
  
  // Setup Serial connection
  Serial.begin(115200); 

  //----------NRF24 SETUP-------------
  //only for MEGA we need to set 53 as output!
  //we need also do that if we not using 53 pin as SS
  pinMode(53,OUTPUT);
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  //radio.openWritingPipe(pipesTX[0]);
  radio.openReadingPipe(1,pipesRX[0]);
  radio.startListening();                 
  //-------------END-------------

  //--------ETHERNET SETUP-------------
  while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SETUP: Ethernet initializing...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("SETUP FAILED: Ethernet not connected using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("SETUP: Ethernet connecting...");

  // if you get a connection, report back via serial:
  
  if (client.connect(server, 80)) {
    Serial.println("SETUP OK: Ethernet connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("SETUP FAILED: Ethernet connection failed");
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
  Serial.print("SETUP OK RTC: current time = ");
  Serial.print(rtc.getDateStr(FORMAT_LONG,FORMAT_BIGENDIAN,'-'));
  Serial.println(rtc.getTimeStr());
  //----------END-----------
   

  Serial.println("--------------SETUP ENDED---------------");  
  Serial.println("RF: Start listening...");
}

void loop() {
  
    //trigger when NRF24 data are available
    byte pipeNo;                                     
    while( radio.available(&pipeNo))
    {
      Serial.println("RF: Get sensors data"); 
      
      radio.read( &myRfData,sizeof(myRfData));
      
      myData.deviceId = myRfData.deviceId;
      myData.sensId = myRfData.sensId;
      myData.value = myRfData.value;

      //check if is device Id or sens id is 0 then skip
      if(myData.deviceId == 0 || myData.deviceId == 0)
      {
        Serial.print("RF FAILED: device id or sens id is 0"); 
        break;
      }
      
      //TMP: set syncId
       myData.syncId = 1;

      Serial.print("OK RF: Sensors data: "); 
      Serial.println(myData.value); 

      Serial.println("RTC: Geting date..."); 
      //get datetime from RTC
      String rtcDate = rtc.getDateStr(FORMAT_LONG,FORMAT_BIGENDIAN,'-');
      String rtcTime = rtc.getTimeStr();
      rtcTime.replace(":","%3A");
      String dateTime = rtcDate +"%20"+ rtcTime;

      myData.measureTime = dateTime; 
    
      Serial.print("OK RTC: ");
      Serial.println(myData.measureTime);  
      delay (10);

      //bild POST body data
      String postData = "syncId="+String(myData.syncId)+"&deviceId="+String(myData.deviceId)+"&sensId="+String(myData.sensId)+"&value="+String(myData.value)+"&measureTime="+myData.measureTime;

      Serial.println("SERVER: Send data");
   
      if(!sentDataToServer(postData))
      {
        Serial.println("FAILED SERVER: Can not send data");
        //Serial.println(postData);
      }
      else
      {     
        Serial.println("OK SERVER: Data was send");
      }
   }     
}

//------------------------------FUNCTIONS-------------------------------------

bool sentDataToServer(String postData)
{ 
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

      timeAfterFirstCheck = 0;

      //check response
      while(timeAfterFirstCheck < timeInterval){
        if (client.available()) {
        char c = client.read();
          Serial.write(c);
        }
      }            
    } else {
      Serial.println("connection failed");

      return false;
    }  
}
