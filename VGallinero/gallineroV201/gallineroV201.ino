//Código abierto: Luar Biurrun-Lorda Aramayo-Lus
//Versión 2.0.1
//Fecha: 22/09/2019
/*-----------------------------------------Disposición cables-----------------------------------------
Entradas en el Arduino:
  SCL -> Morado Oscuro
  SDA -> Gris
  GND -> Marrón
  ENA -> Amarillo
  IN1 -> Verde Oscuro
  IN2 -> Azul Oscuro
  FCU -> Blanco-Azul
  FCD -> Blanco-Verde
*/


#include <Arduino.h>
#include <TimerOne.h> // Esta librería inutiliza la señal PWM de los pines 9 y 10
#include <DS3232RTC.h>   // https://github.com/JChristensen/DS3232RTC
#include <Wire.h>
#include <Streaming.h>


//Pines para el L298N
#define IN1  12
#define IN2  13

#define FCU  6 //Final Carrera Arriba ( Up)
#define FCD  7 //Final Carrera Abajo ( Down)
int horaAbrir = 12, horaCerrar, diaDelAno;
boolean estadoFCU, estadoFCD;
const uint8_t horaDeDiasCerrar[] = {18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,18,18,18
};

//Funciones para girar un motor DC con el driver L298N
void abrir(){
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, HIGH);
 Serial << "Abriendo" << endl;
}

void cerrar(){
 digitalWrite(IN1, HIGH);
 digitalWrite(IN2, LOW);
 Serial << "Cerrando" << endl;
}

void parar(){
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, LOW);
   Serial << "Parado" << endl;
}

//NO CALCULA EL CAMBIO DE HORA
int calculateDayOfYear(int day, int month, int year)
{
  boolean anoBisiesto;
 // Given a day, month, and year (4 digit), returns
 // the day of year. Errors return 999.

 int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

 // Verify we got a 4-digit year
 if (year < 1000) {
   return 999;
 }

 // Check if it is a leap year, this is confusing business
 // See: https://support.microsoft.com/en-us/kb/214019
 if (year%4  == 0) { //Resultado es 0 para aÃ±os bisiestos
   if (year%100 != 0) { //Resultado diferente de 0 si es bisiesto
     daysInMonth[1] = 29; //Febrero tiene 29 dÃ­as para los aÃ±os bisiestos
   }
   else {
     if (year%400 == 0) {
       daysInMonth[1] = 29;
     }
   }
   anoBisiesto = true;
  }
  else{
    anoBisiesto = false;
  }

 // Make sure we are on a valid day of the month
 if (day < 1){
   return 999;
 }
 else if (day > daysInMonth[month-1]){
   return 999;
 }

 int doy = 0;
 for (int i = 0; i < month - 1; i++) {
   doy += daysInMonth[i];
 }

 doy += day;
 return doy;
}

boolean leeFinalCarrera(uint8_t pin){
  int i = 0;
  boolean estado ;/*= digitalRead(pin);
  return estado;*/
  while(i<3){
    estado = digitalRead( pin);
    boolean confirmacion = estado ;
    delay(20);
    estado = digitalRead(pin);
    if (confirmacion == estado){
      return estado;
    }
  i++;
  }
  if(i>=3)
  return estado;
}

void tomarHora(){
  setSyncProvider(RTC.get);    
  if(timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
}

/*void interrupcion(){
  seg++;
  if ( seg > 59)
  {
    seg = 00;
    minut++;
    if ( minut > 59)
    {
      minut = 0;
      hora++;
      if ( hora > 23)
      {
        hora = 00;
      }
     }
   }
 }*/

void puerta()
{
 estadoFCU = leeFinalCarrera(FCU);
 estadoFCD = leeFinalCarrera(FCD);
 //Para abrir
 if((hour() >= horaAbrir) && estadoFCD && !estadoFCU && (hour() < horaCerrar)){ // Cuando sean mÃ¡s de la hora de abrir y FCD pulsado
    long tiempo = millis();
    abrir();
    estadoFCU = leeFinalCarrera(FCU);
    estadoFCD = leeFinalCarrera(FCD);
    delay(1500);
   while(estadoFCD){
     abrir();
     estadoFCU = leeFinalCarrera(FCU);
     estadoFCD = leeFinalCarrera(FCD);
   }
 }
 //Para cerrar
 else if((hour() >= horaCerrar) && estadoFCU && !estadoFCD && hour() > horaAbrir){ // Cuando sea mÃ¡s de la hora de cerrar, FCU pulsado y FCD sin pulsar
   cerrar();
   estadoFCU = leeFinalCarrera(FCU);
   estadoFCD = leeFinalCarrera(FCD);
   delay(1500);
  while(estadoFCU){
    cerrar();
    estadoFCU = leeFinalCarrera(FCU);
    estadoFCD = leeFinalCarrera(FCD);
    delay(1500);
  }
 }
 else parar();
}

//Escribe la fecha por el monitor Serial
void escribirHora(){
  Serial << hour() <<  F( ":") <<  minute() << F( ":") << second() << F("  ") << day()<< F( "/") << month() << F( "/") << year() << endl;
}

void datosPorPantalla(){  
  Serial << F("Día: ") << diaDelAno << endl;
  Serial << F("FCU     FCD   Abrir   Cerrar") << endl;
  Serial << estadoFCU << F("       ") << estadoFCD << F("     ") << horaAbrir << F(":00") << F("   ") << horaCerrar << F(":") << endl;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Empieza la comunicación i2c
  //Timer1.initialize(1000000); // Timer1 es de 16 bits con interrupción cada 1*10^6us=1s
  //Timer1.attachInterrupt(interrupcion);
  pinMode( IN1, OUTPUT);
  pinMode( IN2, OUTPUT);
  /*Los pines se configuran con una resistencia de 20K interna para los finales de carrera.
  FC pulsado = 1  FC sin pulsar = 0 */
  pinMode( FCU, INPUT_PULLUP);
  pinMode( FCD, INPUT_PULLUP);
  //Toma la hora del DS3231, luego no hará falta repetir la función porque con la función interrupción se va cambiando sola
  delay( 2000);
  Serial << F( "Arrancando...") << endl;
  tomarHora();
  Serial << F("Hora tomada") << endl;
  Serial << F("Empezando...") << endl;
}

void loop() {
  diaDelAno = calculateDayOfYear(day(), month(), year());
  horaCerrar = horaDeDiasCerrar[diaDelAno];
  datosPorPantalla();
  puerta();
  escribirHora();
  Serial << endl;
}
