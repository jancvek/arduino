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

Kljub kodi za nizko porabo nodeMCU porabi v deepsleep 1.7mA. To pomeni modul z tako porabo in z AA baterije z 1000mAh deluje približno 1 mesec.

NodeMCU za uporabo na baterije tako ni uporaben! Možno je sicer, da z modifikacijami modula dosežemo manjšo porabo (odstranitev nepotrebnih porabnikov UART, voltage regulator) vendar je boljša izbira esp8266 modul brez dodatnih komponent - barebone.

Potrebno, bi bilo preveriti še ali je esp8266 najboljša izbira za to ali so druge variante tudi uporabne.

# ESP8266 - barebone

Uporabimo ESP8266-12F + adapter, ki ima raster 2.54 in 3 upore za CH_PD(high), GPIO15(low) in GPIO02(high).

![alt text](https://raw.githubusercontent.com/jancvek/arduino/master/pic/esp8266-12F_with_adapter.JPG)

Najprej modul testiramo, brez da bi nanj karkoli nalagali, da preverimo če deluje pravilno. Z modulom lahko komuniciramo z AT komandami. Sledi naslednim korakom: [Link](https://www.instructables.com/id/Getting-Started-with-the-ESP8266-ESP-12/)

Ker adapter nima UART komunikacije moramo uporabiti USB to TTL converter.
Polek tega pa moramo poskrbeti ob nalaganju nove kode (flash), da so izpolnjeni določeni pogoji. Več na: [link](https://www.instructables.com/id/ESP-12F-ESP8266-Module-Minimal-Breadboard-for-Flas/).

Vse pogoje sem upošteval in naredil tako kot je na tej shematiki: 
![alt text](https://raw.githubusercontent.com/jancvek/arduino/master/pic/esp8266_flash_shematic.JPG)

Slika testne plate:
TODO: slika

Postopek nalaganje nove kode:
1. vpni platko z esp in vtakni usb v pc (glej da je črna žica-GPIO02 vpeta na GND in zelena-RESET ni vključena v GND!)
2. izberi prav port in pritisni upload
3. ko se kampajlanje zaključi in se začne nalaganje na esp je potrebno RESET žico vtakniti in iztakniti iz GND, da se esp resetira
4. takoj zatem se bo koda začela nalagati

Več na: [Link](https://www.instructables.com/id/ESP-12F-ESP8266-Module-Minimal-Breadboard-for-Flas/) (v nekaterih navodilih piše da je potrebno izključiti tudi žico GPIO02 takoj po resetu, pa ta korak lahko spustimo!)

Tako imamo novo kodo na esp! Če uporabljamo deepsleep NUJNO povežemo RST in GPIO16 pin, če ne se esp ne bo zbudil.

TODO:
  - preveri ali lahko z ESP8266 modulom razširimo WiFi omrežje, na katerega se lahko potem povežejo drugi moduli in pošiljajo podatke na strežnik,
  - preveri kako lahko na en I2C vežemo več senzorjev,
  - preveri delovanje ESP8266 in releja

To stran je bila oblikovana z [dilinger.io](https://dillinger.io/).
