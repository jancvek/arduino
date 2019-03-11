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
![alt text](https://raw.githubusercontent.com/jancvek/arduino/master/pic/test_bard_for_flash.JPG)

Še skupaj z esp modulom:
![alt text](https://raw.githubusercontent.com/jancvek/arduino/master/pic/test_bard_for_flash_esp.JPG)

Postopek nalaganje nove kode:
1. vpni platko z esp in vtakni usb v pc (glej da je črna žica-GPIO02 vpeta na GND in zelena-RESET ni vključena v GND!)
2. izberi prav port in pritisni upload
3. ko se kampajlanje zaključi in se začne nalaganje na esp je potrebno RESET žico vtakniti in iztakniti iz GND, da se esp resetira
4. takoj zatem se bo koda začela nalagati

Več na: [Link](https://www.instructables.com/id/ESP-12F-ESP8266-Module-Minimal-Breadboard-for-Flas/) (v nekaterih navodilih piše da je potrebno izključiti tudi žico GPIO02 takoj po resetu, pa ta korak lahko spustimo!)

Tako imamo novo kodo na esp! Če uporabljamo deepsleep NUJNO povežemo RST in GPIO16 pin, če ne se esp ne bo zbudil.

## Napajanje z baterijo

Zaradi nizke porabe lahko barebone esp napajamo z baterijami. Z 2xAAA alkalnimi baterijami, ki imajo okoli 800mAh lahko napajamo modul več kot leto dni.

#### Izmerjena dejanska poraba:
Poraba v delovanju: **70 mA**
Poraba v deepsleep: **17 uA**

#### Delovanje na 3.3V

Modul deluje on MAX **3.6V** in MIN **2.6V**.

Alkalne baterije pa imajo napetost od 1.5V (napolnjena) do 0.9V (prazna). To pomeni, da imamo razpon pri **2** x alkalne: **3.0-1.8V** in pri **3** x alkalne: **4.5-2.7V**.

Vidimo, da bomo potrebovali step-up regulator, ki pa bo moral imeti dober izkoristek oz malo porabo. Največkrat sem zasledil naslednjega [Pololu 3.3V](https://www.pololu.com/product/2561), vendar ga v EU ni moč kupiti tudi na na ebay ne. Našel sem alternativo [DC-DC 1V 1.2V 1.5V 1.8V 2.5V 3V to DC 3.3V Step-UP Boost Power Supply Converter](https://www.ebay.com/itm/DC-DC-1V-1-2V-1-5V-1-8V-2-5V-3V-to-DC-3-3V-Step-UP-Boost-Power-Supply-Converter/401408562844?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649).

Na tem [linku](https://iot-playground.com/blog/2-uncategorised/7-low-power-sensor-and-step-up-regulator) sem našel test regulatorja kjer je navedena učinkovitost z grafom: 

![alt text](https://raw.githubusercontent.com/jancvek/arduino/master/pic/step_up_efficiency_graph.JPG)

Omenjen regulator pokriva območje od **1.1-3.3V**, kar je je ravno prav za 2xalcaline baterije.

Ostali linki, ki sem jih rabil:
[https://tttapa.github.io/ESP8266/Chap02%20-%20Hardware.html](https://tttapa.github.io/ESP8266/Chap02%20-%20Hardware.html)


TODO:
  - preveri ali lahko z ESP8266 modulom razširimo WiFi omrežje, na katerega se lahko potem povežejo drugi moduli in pošiljajo podatke na strežnik,
  - preveri kako lahko na en I2C vežemo več senzorjev,
  - preveri delovanje ESP8266 in releja

To stran je bila oblikovana z [dilinger.io](https://dillinger.io/).
