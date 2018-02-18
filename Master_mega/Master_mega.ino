#include <DS3231.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

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
RF24 radio(9,53);

//set listen and read pipes up to 6 pipes
const uint64_t pipesRX[1] = { 0xF0F0F0F0AA }; //0xF0F0F0F0AB, 0xF0F0F0F0AC, 0xF0F0F0F0AD, 0xF0F0F0F0AE, 0xF0F0F0F0AF
const uint64_t pipesTX[1] = { 0xF0F0F0F0BA }; //0xF0F0F0F0BB, 0xF0F0F0F0BC, 0xF0F0F0F0BD, 0xF0F0F0F0BE, 0xF0F0F0F0BF

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

//-------------SD-----------------
File myFile;
String fileName = "data.txt";
int sdNumberOfLines = 100;
//------------END----------------

//TMP: for testing
int serverCalls = 0;
//TMP: end

void setup() {
  // put your setup code here, to run once:
  
  // Setup Serial connection
  Serial.begin(115200);

  //----------NRF24 SETUP-------------
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(pipesTX[0]);
  radio.openReadingPipe(1,pipesRX[0]);
  radio.startListening();                 
  //-------------END-------------

  //--------ETHERNET SETUP-------------
  while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("SETUP: Ethernet initializing...");
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
  //----------END-----------
   
  //---------SD SETUP---------
  Serial.print("SETUP: SD Initializing...");

  if (!SD.begin(4)) {
    Serial.println("SETUP FAILED: SD initialization failed!");
  }else{
    Serial.println("SETUP OK: Initialization done.");

    Serial.print("SETUP STATUS: Number of unsend data on SD: ");   
    Serial.println(countDataOnSd());

    //OPTION: to delete SD at start
    /*
    Serial.println("SETUP: Deleting SD card");
    SD.remove(fileName);   
    */
  }
  //----------END----------
}

void loop() {

    //trigger when NRF24 data are available
    byte pipeNo;                                     
    while( radio.available(&pipeNo))
    {
      radio.read( &myRfData,sizeof(myRfData));
    
      Serial.println("RF: Get sensors data"); 
      
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
            
        //save data for later sending
        Serial.println("SD: Write data");
        if(writeSdData(postData))
        {
          Serial.println("OK SD: Writting data");       
        }
        else
        {
          Serial.println("FAILED SD: Can not writting data");
        }
      }
      else
      {     
        Serial.println("OK SERVER: Data was send");
        
        //server is accessable...
        //check if data on SD exist and sent them to server
        String dataArr[sdNumberOfLines]; 
  
        Serial.println("SD: Check for data");
        
        if(readSdData(dataArr))
        {
          Serial.println("OK SD! Data was read");
          
          //delete file after reading in case that some data are successfully saved and some not
          //in next steps data which were not saved are saved on new file
          SD.remove(fileName);     
  
          Serial.println("SERVER: Sending data from SD...");
  
          int numberOfFullLines = 0;
          int numOfSendData = 0;
          
          for(int i = 0; i<sdNumberOfLines;i++)
          {
            if(dataArr[i] == ""){continue;}
  
            numberOfFullLines++;          
            String sentData = dataArr[i];
            
            if(!sentDataToServer(sentData))
            { 
              if(!writeSdData(sentData)){
                Serial.println("FAILED SD: Can not write");
                Serial.println("FATAL ERROR: Data lost. Data did not sent to server and not saved on SD");                     
              }         
            } else{
              //Serial.println("SERVER OK: Data was sent to server");
              numOfSendData++;
            }
          } 
  
          if(numberOfFullLines == numOfSendData){
            Serial.println ("SERVER OK: All data was send from SD"); 
          }else{
            Serial.print("SERVER: Not all data was sent to server. Num data: ");  
            Serial.print(numberOfFullLines);   
            Serial.print("sent data: "); 
            Serial.println(numOfSendData);
          }
        }
        else
        {
           Serial.println("SD: There is no data");
        }      
      }
   }     
}

//------------------------------FUNCTIONS-------------------------------------

bool sentDataToServer(String postData)
{
  //TMP: for testing
  /*
  bool res;  
  if(serverCalls == 0){res =  true;}
  else if(serverCalls == 1){res = false;}
  else if(serverCalls == 2){res = false;}
  else if(serverCalls == 3){res = false;}
  else if(serverCalls == 4){res = false;}
  else if(serverCalls == 5){res = false;}
  else if(serverCalls > 5){res = true;}
  serverCalls++;  
  return res;
  */
  
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

bool writeSdData (String dataLine)
{
  //check if file alread exist
  if(SD.exists(fileName))
  {
    //TODO: write now line
    myFile = SD.open(fileName, FILE_WRITE);

    if (myFile) 
    {
      //TODO: chech if '\r\n' works and addopt this code
      //edd character '|' at the end of the line (I have not tested '\r\n' jet)
      dataLine = dataLine + "|";
      
      myFile.println(dataLine);
      // close the file:
      myFile.close();

      return true;
    } 
    else 
    {
      // if the file didn't open, print an error:
      Serial.print("ERROR SD: Can not open file: ");
      Serial.println(fileName);

      return false;
    }
  }
  else
  {
    //create new folder
    File dataFile = SD.open(fileName, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) 
    {
      //TODO: chech if '\r\n' works and addopt this code
      //edd character '|' at the end of the line (I have not tested '\r\n' jet)
      dataLine = dataLine + "|";
      
      dataFile.println(dataLine);
      // close the file:
      dataFile.close();

      return true;
    }
    // if the file isn't open, pop up an error:
    else 
    {
      // if the file didn't open, print an error:
      Serial.print("ERROR SD: Can not create file: ");
      Serial.println(fileName);

      return false;
    }
  }
}

bool readSdData (String *dataArr)
{
  int numOfLines = 0;
  
  //open the file for reading:
  myFile = SD.open(fileName);
  if (myFile) 
  {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
          
      //TODO: chech if '\r\n' trigger

      //read line on SD
      String dataLine = myFile.readStringUntil('|');

      dataArr[numOfLines] = dataLine;
      numOfLines++;
    }
    // close the file:
    myFile.close();

    return true;
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.print("ERROR SD: Can open file: ");
    Serial.println(fileName);

    return false;
  }
}

int countDataOnSd ()
{
  int numOfLines = 0;
  
  //open the file for reading:
  myFile = SD.open(fileName);
  if (myFile) 
  {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
          
      //TODO: chech if '\r\n' trigger

      //read line on SD
      String dataLine = myFile.readStringUntil('|');
      numOfLines++;
    }
    // close the file:
    myFile.close();
  } 
  return numOfLines;
}
