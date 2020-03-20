# [Oilategi automatizatua](https://github.com/pl4netary/Gallinero-Domotizado)
---
###### Hizkuntak: [euskara](./README.md), [castellano](./LEEME.md)
###### [Notak](NOTAK.md)

## Atalak
  - [Hardwarea](#hardwarea)
  	* [Materialak](#materialak)
  	* [Eskematikoa](#eskematikoa)
  	* [Irudia](#irudia)
  	* [Kontsumoa](#kontsumoa)
  - [Firmwarea](#firmwarea)
  	* [Helburuak](#helburuak)

## Hardwarea

### Materialak
  * [ESP-32 Doit Kit V1](https://docs.zerynth.com/latest/official/board.zerynth.doit_esp32/docs/index.html "Web orri ofiziala")
  * [Real Time Clock DS3231](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf "Datasheet")
  * [L298N H-Bridge Module](https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/ "Tutoriala")
  * [Erresistentziak]("Datasheet")
  * [Transistoreak]("Datasheet![](argazkiak/eskematikoa_irudia.jpg)")
  * [Botoia]()

### Eskematikoa
![Eskematikoa](argazkiak/eskematikoa.svg "Eskematikoa")

### Irudia
![Irudia](argazkiak/eskematikoa_irudia.jpg "Irudia")

### Kolore diagrama

|	Izena	|	Kablearen kolorea	|	GPIO	|
|:----------|:-----------------------:|------:|
| FCU 		| Blanco-Azul			|GPIO12	|
| FCD 		| Blanco-Verde			|GPIO13	|
| MOTOR		|						|GPIO14	|
| SDA 		| Morea					|GPIO21	|
| SCL 		| Grisa					|GPIO22	|
| IN2 		| Azul Oscuro			|GPIO26	|
| IN1 		| Verde Oscuro		 	|GPIO27	|
| RTC		|						|GPIO33	|

| FCD FCU | estado |
|:---------:|--------:|
|	0   0   | nada |
|	0   1   | abajo |
|	1   0   | arriba |
|	1   1   | en medio |

### Kontsumoa

| **Sinboloa** | **Parametroa**	  | **Min**	| **Normala** | **Max** | **Unitatea** |
|:------------|:-----------------|:--------:|:-----------:|:-------:|:------------:|
| V<sub>in</sub>          | Sarrerako boltaia |          |		    5 |		    | 			V |
| V<sub>inEsp32</sub>     | ESP-32 sarrerako boltaia | 6 | 7 | 20 | V |
| V<sub>bat</sub> | Bateriaren boltaia |	9.8  | 12 |	13 | V |
| V<sub>RTC</sub> | Erlojuaren sarrerako boltaia | 2.3 | 3.3 | 5.5 | V |
| I<sub>RTC</sub> | Erlojuaren ohizko kontsumoa  | | 200 |  | uA |
| I<sub>in</sub>          | Sarrerako korrontea esnaturik | | | | A |
| I<sub>sleep_deepESP32</sub> | Sarrerako korrontea lo dagoenean | | 10 | | uA |




## Firmwarea

### Helburuak
* Egunean bitan atea mugitzea. Goizean oilategiaren atea irekitzea eta arratsaldean istea
> Kanpo erlojuarekin, ESP32a lo dagoenean esnatu ahal da honek dauzkan iratzargailuekin eta ESP2aren kanpo iratzar moduarekin.

* Sistema autonomoa izatea, beraz bateria batetik energia hartuko du elikatzeko
> Eguzki plaka batzuekin bateria elikatu ahal da.

* Kontsumo baxua izatea, bateria ahalik eta gehien iraun dezan
> ESP32a berez kontsumo gutxi dauka normalean erabiltzen denean, baina nahi bada lo-moduan jarri ahal da erabiltzen ez denean gutxiago kontsumitzeko. Aldi berean, bertatik esnatzerakoan gainontzeko sistemak piztu ahal dira eta erabiltzen ez direnean gutxiago kontsumitzeko.

* Ahalik eta sinpleen izatea funtzionamendua eta diseinua
> Programa eta eskematikoa ia edozein pertsona ulertu behar du, baoitzak bere kabuz egin dezan.

### Kanpo Liburutegiak
* [DS3232RTC.h](https://github.com/JChristensen/DS3232RTC)
  Kanpo erlojua kontrolatzeko behar den liburutegia da. Oso erabilgarria eslojuaren mikrotxiparen programa guztiak kontrolatzeko.
* [Streaming.h](http://arduiniana.org/libraries/streaming/)
  C++ estiloa gustuko duten pertsonentzat egindako libutegia non bakarrik idazkera aldatzen da.
* [rtc_io.h](https://github.com/espressif/arduino-esp32)

### Funtzionamendua

##### TODO
  - Añadir un fusible en el motor
