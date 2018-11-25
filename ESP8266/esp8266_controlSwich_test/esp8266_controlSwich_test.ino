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

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {

  Serial.begin(115200);
  
  // Transmitter is connected to ESP8266 Pin D1  
  mySwitch.enableTransmit(5);
  
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(5);

  // Optional set pulse length.
   mySwitch.setPulseLength(511);
  
  // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(5);
  
}

void loop() {

  /* Same switch as above, but using decimal code */
  Serial.println("ON");
  mySwitch.send(14368048, 24);
  delay(1000);  
 Serial.println("OFF");
  mySwitch.send(14242976, 24);
  delay(1000);  

  delay(20000);
}
