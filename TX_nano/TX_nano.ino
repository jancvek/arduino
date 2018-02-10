#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include "RF24.h"

struct dataStruct{
  int deviceId;
  int sensId;
  float value;
}myData;

//----USER SETTINGS-----
//unique device id
int deviceIdNum = 1;

//unique temperature sensor id
int tempSensId = 1;

//speed of serial port
long serialSpeed = 115200;

//interval time to measure in mili second
int measureInterval = 3000;

//pin number on which data wire of sensor is plugged on the Arduino
#define ONE_WIRE_BUS 2

//pins for RF24 (ce,csn)
RF24 radio(7,8);

//number of temperature sensors
int numOfSens = 1;

//temperature correction factor (insert for every sensor)
float delTemp[1] = {-1.2};
//---------END--------


// Setup a oneWire instance to communicate with any OneWire devices  
DeviceAddress myThermometer [1];
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(serialSpeed);
  Serial.println(F("Arduino Nano TX Library"));

  sensors.begin();
  sensors.getAddress(myThermometer [0], 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //TODO: read temperature 
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
  //TODO: convert them in the right data format

  //TODO: check JSON library to sent data

  //create data struct
  myData.deviceId = deviceIdNum;
  myData.sensId = tempSensId;
  myData.value = tempArr[0];
  
  //sent data
  if(sendData(myData))
  {
    Serial.println(F("Send data: OK"));
    //ledBlink(13,100,1);
    delay(300000);
  }else{
    Serial.println(F("Send data: FAILED"));
    delay(1000);
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
