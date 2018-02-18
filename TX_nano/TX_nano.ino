#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//----USER SETTINGS-----
//unique device id
int deviceIdNum = 1;

//unique temperature sensor id
int tempSensId = 1;

//interval time to measure in mili second
int measureInterval = 3000000;

//pin number on which data wire of sensor is plugged on the Arduino
#define ONE_WIRE_BUS 2

//number of temperature sensors
int numOfSens = 1;

//temperature correction factor (insert for every sensor)
float delTemp[1] = {-1.2};
//---------END--------

//------DATA STRUCT----
struct dataStruct{
  int deviceId;
  int sensId;
  float value;
}myData;
//--------END---------

//-------NRF24-------
//pins for RF24 (ce,csn)
RF24 radio(7,8);

const uint64_t pipesRX[1] = { 0xF0F0F0F0BA };
const uint64_t pipesTX[1] = { 0xF0F0F0F0AA };
//-------END--------

// Setup a oneWire instance to communicate with any OneWire devices  
DeviceAddress myThermometer [1];
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println(F("Arduino Nano TX Library"));
 
  //-------NRF24 SETUP-------
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(pipesTX[0]);
  radio.openReadingPipe(1,pipesRX[0]);

  //check if Master is accessable
  char test = 'a';
  if (radio.write(&test,sizeof(test))){ 
    Serial.println("RF TEST OK: Connected with master");
  }else{
    Serial.println("RF TEST FAILED: Can not connect to master");
  }
  //----------END------------
  sensors.begin();
  sensors.getAddress(myThermometer [0], 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //send the command to get temperature readings
  float tempArr[numOfSens];
  
  sensors.requestTemperatures();  
  
  for(int i = 0; i < numOfSens; i++)
  {
    // You can have more than one DS18B20 on the same bus.  
    // 0 refers to the first IC on the wire 
    tempArr[i] = sensors.getTempCByIndex(i);
    tempArr[i] = tempArr[i]+delTemp[i];
    
    Serial.print(F("Sensor num: "));
    Serial.print(i);
    Serial.print(F(" temp: "));
    Serial.println(tempArr[i]);
  }

  //create data struct
  myData.deviceId = deviceIdNum;
  myData.sensId = tempSensId;
  myData.value = tempArr[0];
  
  //sent data
  if(sendData(myData))
  {
    Serial.println("RF OK: Data send");
  }else{
    Serial.println("RF FAILED: Data not send");
  }
  
  //dalay
  delay(measureInterval);
}

bool sendData(dataStruct myData)
{
  if (radio.write(&myData,sizeof(myData))){ 
    return true;
  }else{
    return false;
  }
}
