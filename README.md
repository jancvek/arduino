# Arduino in ESP8266

Repository vsebuje kodo sensorjev za merjenje temperature in vlage v hiši. Razdeljen je v dve mapi:

  - arduino (stara koda master modula in slave arduino, ki pošilja podatke na master, master pa pošlje podatke na strežnik)
  - ESP8266 (nova koda za esp8266 modul)

# ESP8266

Mapa vsebuje:

  - esp8266 deepsleep test (testna koda, ki preveri ali deepsleep deluje)
  - esp8266 tempSensor (koda, ki jo uporabljam na modulu, ki je priključen na napajanje)
  - esp8266 tempSensor battery (koda z isto funkcionalnostjo kot esp8266 tempSensor, vendar je prilagojena za nizko porabo)
  - esp8266 wifiStrength test (testna koda za preverjanje moči signala WIFI na katerega se želimo povezati)

Slika modula ESP8266 z sensorjem temperature in vlage, s kodo prilagojeno za nizko porabo:
![alt text](https://raw.githubusercontent.com/jancvek/arduino/master/pic/esp8266_onBattery.jpg)

TODO:
  - preveri ali lahko z ESP8266 modulom razširimo WiFi omrežje, na katerega se lahko potem povežejo drugi moduli in pošiljajo podatke na strežnik.

To stran je bila oblikovana z [dilinger.io](https://dillinger.io/).
