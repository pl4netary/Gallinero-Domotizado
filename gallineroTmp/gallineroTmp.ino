//Código abierto: Luar Biurrun-Lorda Aramayo-Lus
//Versión 2.0
//Fecha: 9/09/2019
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

FCD FCU | estado
----------------
0   0   | nada
0   1   | abajo
1   0   | arriba
1   1   | en medio

*/


#include <Arduino.h>
#include <TimerOne.h>    //Esta librería inutiliza la señal PWM de los pines 9 y 10
#include <avr/sleep.h>   //Libería para dormirlo
#include <avr/power.h>
#include <DS3232RTC.h>   // https://github.com/JChristensen/DS3232RTC
#include <Wire.h>
#include <Streaming.h>   //Replica la manera de escribir por el monitor de C++

//Pines para el L298N
#define IN1  12
#define IN2  13
//Pines de los finales de carrera
#define FCU  6 //Final Carrera Arriba ( Up)
#define FCD  7 //Final Carrera Abajo ( Down)
//#define SQW  2 //Pin para la interrupción externa

//variables para el reloj
//#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match <- Tendría que usar esta para la mañana
//#define ALRM2_MATCH_HR_MIN 0b100 // when hours and minutes match <- Tendría que usar esta para la noche

//#define SQWpin 2 //Pin para la inerrupción externa
//const uint8_t SQW_PIN(2);


int ano;
uint8_t horaAbrir = 12, horaCerrar, horaDiaSiguiente, horaDiaSiguienteAux, minutoCerrar;
uint8_t hora, minut, seg, dia, mes;
int diaDelAno = 0;
long tiempoTotal = 30000;
boolean noAbrirPuerta =  true, estadoFCU, estadoFCD;
boolean anoBisiesto = false;
const uint8_t horaDeDiasCerrar[] = {18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,18,18,18
};
const uint8_t minutoDeDiasCerrar[] = {3,4,5,6,7,8,9,10,11,12,13,14,15,17,18,19,20,21,23,24,25,26,28,29,30,32,33,34,35,37,38,39,41,42,43,45,46,47,49,50,51,53,54,55,57,58,59,1,2,3,4,6,7,8,10,11,12,13,15,16,17,18,19,21,22,23,24,26,27,28,29,30,32,33,34,35,36,37,39,40,41,42,43,44,46,47,48,49,50,51,53,54,55,56,57,58,59,1,2,3,4,5,6,7,9,10,11,12,13,14,16,17,18,19,20,21,22,24,25,26,27,28,29,30,31,33,34,35,36,37,38,39,40,41,42,43,44,45,47,48,49,49,50,51,52,53,54,55,56,57,57,58,59,0,1,1,2,3,3,4,4,5,5,6,6,7,7,7,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,8,8,8,8,7,7,6,6,5,5,4,4,3,2,2,1,0,59,59,58,57,56,55,54,53,52,51,50,49,47,46,45,44,43,41,40,39,37,36,35,33,32,30,29,27,26,24,23,21,20,18,16,15,13,11,10,8,6,5,3,1,0,58,56,54,53,51,49,47,46,44,42,40,39,37,35,33,31,30,28,26,24,22,21,19,17,15,14,12,10,8,6,5,3,1,59,58,56,54,53,51,49,48,46,44,43,41,40,38,36,35,33,32,30,29,28,26,25,23,22,21,19,18,17,15,14,13,12,11,10,9,8,6,5,5,4,3,2,1,0,59,59,58,57,57,56,56,55,55,54,54,54,53,53,53,53,53,52,52,52,52,52,53,53,53,53,53,54,54,54,55,55,56,56,57,58,58,59,0,0,1,2
};
/*-----------------------------------------Funciones-----------------------------------------*/

//Funciones para girar un motor DC con el driver L298N
void abrir(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void cerrar(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void parar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

//NO CALCULA EL CAMBIO DE HORA
int calculateDayOfYear(int day, int month, int year)
{
 // Given a day, month, and year (4 digit), returns
 // the day of year. Errors return 999.

 int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

 // Verify we got a 4-digit year
 if (year < 1000) {
   return 999;
 }

 // Check if it is a leap year, this is confusing business
 // See: https://support.microsoft.com/en-us/kb/214019
 if (year%4  == 0) { //Resultado es 0 para años bisiestos
   if (year%100 != 0) { //Resultado diferente de 0 si es bisiesto
     daysInMonth[1] = 29; //Febrero tiene 29 días para los años bisiestos
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

boolean leeFinalCarrera(uint8_t pin)
{
  int i = 0;
  boolean estado ;
  /*= digitalRead(pin);
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

void tomarHora()
{
  hora = hour();
  minut = minute();
  seg = second();
  dia = day();
  mes = month();
  ano = year();
}

void interrupcion(){
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
   //Imprime los datos por patalla cada segundo
   datosPorPantalla();
   escribirHora();
 }

void puerta()
{
 estadoFCU = leeFinalCarrera(FCU);
 estadoFCD = leeFinalCarrera(FCD);
 //Para abrir
 if((hora >= horaAbrir) && estadoFCD && !estadoFCU && (hora < horaCerrar) && (minut < minutoCerrar)){ // Cuando sean más de la hora de abrir y FCD pulsado
    long tiempo = millis();
    abrir();
    estadoFCU = leeFinalCarrera(FCU);
    estadoFCD = leeFinalCarrera(FCD);
    delay(1500);
   while(estadoFCD){
     abrir();
     estadoFCU = leeFinalCarrera(FCU);
     estadoFCD = leeFinalCarrera(FCD);
     Serial << F("Abriendo") << endl;
   }
   //Sección de control de tiempo mínimo para puerta
   if(tiempo < tiempoTotal)
   {
     tiempoTotal = tiempo;
     if(tiempoTotal < 10000) //Tiempo mínimo que tiene que tener
     {
       tiempoTotal = 10000;
     }
   }
   else
   {
     Serial.println(F("ALARMA"));
     parar();
     noAbrirPuerta = false;
   }
   tiempo = 0;
 }
 //Para cerrar
 else if((hora > horaAbrir) && estadoFCU && !estadoFCD && (hora >= horaCerrar) && (minut >= minutoCerrar)){ // Cuando sea más de la hora de cerrar, FCU pulsado y FCD sin pulsar
   cerrar();
   estadoFCU = leeFinalCarrera(FCU);
   estadoFCD = leeFinalCarrera(FCD);
   delay(1500);
  while(estadoFCU){
    cerrar();
    estadoFCU = leeFinalCarrera(FCU);
    estadoFCD = leeFinalCarrera(FCD);
    Serial << F("Cerrando") << endl;
    delay(1500);
  }
 }
 else{
   parar();
   Serial << F("Parado") << endl;
 }
}

//Escribe la fecha por el monitor Serial
void escribirHora()
{
  Serial << hora << F(":") << minut << F(":") << seg << F(" ") << dia << F( "/") << mes << F( "/") << ano << endl;
}

void datosPorPantalla()
{
  Serial << F("Día: ") << diaDelAno << endl;
  Serial << F("FCU     FCD   Abrir   Cerrar") << endl;
  Serial << estadoFCU << F("       ") << estadoFCD << F("     ") << horaAbrir << F(":00") << F("   ") << horaCerrar << F(":") << minutoCerrar << endl;
}
/*
void pin2Interrupt()
{
  /* Aquí se vuelve del modo dormido, se desactiva para así evitar tener que estar
  viniendo a aquí mientras esté activo el pulso

  detachInterrupt(0);
  tomarHora(); //Así me aseguro de que se guarda y elimino los errores que se hayan generado

  diaDelAno = calculateDayOfYear( dia, mes, ano);
  //Corrección para los años bisiestos, no importa que se repita el dato del día anterior
  if(anoBisiesto == true){
    if(diaDelAno ==  27){
      horaCerrar = horaDeDiasCerrar[26];
      minutoCerrar = minutoDeDiasCerrar[26];
    }
    else{
      horaCerrar = horaDeDiasCerrar[diaDelAno];
      minutoCerrar = minutoDeDiasCerrar[diaDelAno];
    }
  }

  // La alarma2 cambia para cada día
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, minutoCerrar, horaCerrar, 1);
  RTC.alarm(ALARM_2);                   //Limpia el flag del reloj
  RTC.alarmInterrupt(ALARM_2, true);


  puerta(); // Así lo primero que hace al volver del sleepmode es comprobar la puerta
}*/
/*
void enterSleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable(); //Activa el sleep_mode
  sleep_mode(); //Entra en modo dormido

  //El programa sigue a partir de aquí
  sleep_disable(); //Desactiva el modo dormido*/

  /*Activa todos los periféricos, útil para que no consuman
  power_all_enable();
}
*/

void setup() {
  Serial.begin(115200); // Empieza la comunicación serial
  Wire.begin(); // Empieza la comunicación i2c
  Timer1.initialize(1000000); // Timer1 es de 16 bits con interrupción cada 1*10^6us=1s
  Timer1.attachInterrupt(interrupcion);
  pinMode( IN1, OUTPUT);
  pinMode( IN2, OUTPUT);
  pinMode( FCU, INPUT_PULLUP);
  pinMode( FCD, INPUT_PULLUP);
  //pinMode( SQW, INPUT_PULLUP); //Entrada para el pulso cuadrado del RTC
  /*En la subida del pulso cuadrado se activa la interrupción y sale del sleep */
  //attachInterrupt(SQWpin, pin2Interrupt, RISING);
  //Toma la hora del DS3231, luego no hará falta repetir la función porque con la función interrupción se va cambiando sola
  setSyncProvider(RTC.get);    // setSyncProvider() causes the Time library to synchronize with the

  if(timeStatus() != timeSet)  // external RTC by calling RTC.get() every five minutes by default.
      Serial.println(F("No se pudo conectar con el reloj"));
  else
      Serial.println(F("Tiempo ajustado por el reloj"));
  //Serial.println("punto1");
  tomarHora(); //Guardo la hora en las variables locales
  //Serial.println("punto2");
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  /*RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE); //Es mejor declarar ésto antes de declarar las alarmas
  //Serial.println("punto3");

  /*
  //Alarma1 a las 12:00
  RTC.setAlarm(ALM1_MATCH_MINUTES, 0, 10, 0, 0); //Para que ocurra cada 10 segundos
  RTC.alarm(ALARM_1);                   //Limpia el flag del reloj
  RTC.alarmInterrupt(ALARM_1, true);
  Serial.println("punto4");

  diaDelAno = calculateDayOfYear( dia, mes, ano);

  horaCerrar = horaDeDiasCerrar[diaDelAno];
  minutoCerrar = minutoDeDiasCerrar[diaDelAno];

  /*
  // La alarma2 cambia para cada día
  RTC.setAlarm(ALRM2_MATCH_HR_MIN, 0, minutoCerrar, horaCerrar, 1);
  RTC.alarm(ALARM_2);                   //Limpia el flag del reloj
  RTC.alarmInterrupt(ALARM_2, true);
  Serial.println("punto5");
  escribirHora();
  */
}

void loop()
{
  /*Si ha cambiado el día, tomar la hora del día siguiente para cerrar*/
  diaDelAno = calculateDayOfYear( dia, mes, ano);
  horaCerrar = horaDeDiasCerrar[diaDelAno];
  minutoCerrar = minutoDeDiasCerrar[diaDelAno];
  //Serial.println("punto6");//Hasta aquí ha llegado
  //Serial.println("punto7");
  puerta();
  //Serial.println("punto8");
  //enterSleep();
}
