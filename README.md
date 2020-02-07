# Oilategi automatizatua
---
###### Hizkuntak: [euskara](./README.md), [castellano](./LEEME.md)
###### [Notak](NOTAK.md)

## Atalak
  - 1 [Hardwarea](#hardwarea)
  	- 1 [Materialak](#materialak)
  	2 [Eskematikoa](#eskematikoa)
  	3 [Irudia](#irudia)
  	4 [Kontsumoa](#kontsumoa)
  2 [Firmwarea](#firmwarea)
  	1 [Helburuak](#helburuak)

## Hardwarea

### Materialak
  * [ESP-32 Doit Kit V1]()
  * [Real Time Clock DS3231](https://docs.zerynth.com/latest/official/board.zerynth.doit_esp32/docs/index.html "Web horri ofiziala")
  * [L298N H-Bridge Module](https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/ "Tutoriala")
  * [Erresistentziak]("Datasheet")
  * [Transistoreak]("Datasheet![](argazkiak/eskematikoa_irudia.jpg)")
  * [Botoia]()

### Eskematikoa
![Eskematikoa](eskematikoa.svg "Eskematikoa")

### Irudia
![Irudia](eskematikoa_irudia.jpg "Irudia")

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

### Funtzionamendua
