//Código abierto: Luar Biurrun-Lorda Aramayo-Lus
//Versión 2.0
//Fecha: 27/07/2019
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

MEJORAS para hacer:
-Ajuste automático del cambio de hora anual. EL DS3231 no tiene en cuenta el
 cambio de hora, pero las horas de cerrar están hechas con el cambio de hora local
-Entrar en modo sleep para no consumir tanto
-Añadir sensor de temperatura

CONSUMO:
-Módulo solar 10mA
-Módulo solar + Módulo control 77mA
-Módulo solar + Módulo control + Motor

La alarma1 es para abrirlo a la mañana y la alarma2 para cerrarlo a la noche
BASADO EN EL EJEMPLO EX7 DE librería DS3232RTC

PASOS A SEGUIR:
X -Hacer que la alarma1 = 12:00 y alarma2 = 21:30
X -Pasar todo el progrma a -> pruebaGallinero {
      Lo primero que debería hacer al volver del sleepmode es tomar la hora otra vez,
      para así dejarla guardada y no haya errores.
   }
  -Hacer que alarma2 cambie según el día y tome la del día siguiente
  -Añadir el resto de funciones del programa gallinero

ANOTACIÓN:
  Al principio iba a usar el Whatch Dog activarlo, pero al final
  he visto que no sirve si quiero hacerlo más de dos minutos. Es más, se podría
  hacer que se activase.

  El RTC DS3231 consume 4.25mA
*/


#include <Arduino.h>
#include <TimerOne.h>    //Esta librería inutiliza la señal PWM de los pines 9 y 10
#include <avr/sleep.h>   //Libería para dormirlo
#include <avr/power.h>
#include <DS3232RTC.h>   // https://github.com/JChristensen/DS3232RTC
#include <Wire.h>

//Pines para el L298N
#define IN1  12
#define IN2  13
//Pines de los finales de carrera
#define FCU  6 //Final Carrera Arriba ( Up)
#define FCD  7 //Final Carrera Abajo ( Down)
#define SQW  2 //Pin para la interrupción externa

//variables para el reloj
#define ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
#define ALRM1_MATCH_SEC        0b1110  // when seconds match
#define ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match <- Tendría que usar esta para la mañana

#define ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
#define ALRM2_MATCH_MIN        0b110   // when minutes match
#define ALRM2_MATCH_HR_MIN 0b100 // when hours and minutes match <- Tendría que usar esta para la noche

#define SQWpin 2 //Pin para la inerrupción externa
const uint8_t SQW_PIN(2);


int ano;
uint8_t horaAbrir = 12, horaCerrar, horaDiaSiguiente, horaDiaSiguienteAux, minutoCerrar;
uint8_t hora, minut, seg, dia, mes;
int diaDelAno = 0;
long tiempoTotal = 30000;
boolean noAbrirPuerta =  true;
boolean anoBisiesto = false;
const uint8_t horaDeDiasCerrar[] = {17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 18, 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
};
const uint8_t minutoDeDiasCerrar[] = { 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 57, 58, 59, 0, 1, 3, 4, 5, 6, 8, 9, 10, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23, 25, 26, 27, 29, 30, 31, 33, 34, 35, 37, 38, 39, 41, 42, 43, 44, 46, 47, 48, 50, 51, 52, 53, 55, 56, 57, 58, 59, 1, 2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43, 44, 45, 46, 47, 49, 50, 51, 52, 53, 54, 56, 57, 58, 59, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 41, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 48, 48, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 43, 42,42, 41, 40, 39, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 27, 26, 25, 24, 23, 21, 20, 19, 17, 16, 15, 13, 12, 10, 9, 7, 6, 4, 3, 1, 0, 58, 56, 55, 53, 51, 50, 48, 46, 45, 43, 41, 40, 38, 36, 34, 33, 31, 29, 27, 26, 24, 22, 20, 19, 17, 15, 13, 11, 10, 8, 6, 4, 2, 1, 59, 57, 55, 54, 52, 50, 48, 46, 45, 43, 41, 39, 38, 36, 34, 33, 31, 29, 28, 26, 24, 23, 21, 20, 18, 16, 15, 13, 12, 10, 9, 8, 6, 5, 3, 2, 1, 59, 58, 57, 55, 54, 53, 52, 51, 50, 49, 48, 46, 45, 45, 44, 43, 42, 41, 40, 39, 39, 38, 37, 37, 36, 36, 35, 35, 34, 34, 34, 33, 33, 33, 33, 33, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42
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

void tomarHora()
{
  hora = hour();
  minut = minute();
  seg = second();
  dia = day();
  mes = month();
  ano = year();
}

void interrupcion()
{
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
 }

void puerta()
{
 boolean estadoFCU = leeFinalCarrera(FCU);
 boolean estadoFCD = leeFinalCarrera(FCD);
 //Para abrir
 if((hora = horaAbrir) && estadoFCD && !estadoFCU && (hora = horaCerrar) && (minut < minutoCerrar)){ // Cuando sean más de la hora de abrir y FCD pulsado
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
 else if((hora = horaCerrar) && (minut = minutoCerrar) && estadoFCU && !estadoFCD){ // Cuando sea más de la hora de cerrar, FCU pulsado y FCD sin pulsar
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
void escribirHora()
{
  Serial.print( hora);
  Serial.print( F( ":"));
  Serial.print( minut);
  Serial.print( F( ":"));
  Serial.print( seg);
  Serial.print("  ");
  Serial.print( dia);
  Serial.print( F( "/"));
  Serial.print( mes);
  Serial.print( F( "/"));
  Serial.println( ano);
}

void datosPorPantalla()
{
  Serial.print(F("Día: "));
  Serial.print( diaDelAno);
  Serial.println();
  Serial.println( F( "FCU     FCD   Abrir   Cerrar"));
  Serial.print("     ");
  Serial.print(horaAbrir);
  Serial.print(":00");
  Serial.print(F("   "));
  Serial.print(horaCerrar);
  Serial.print(":00");
  Serial.println();
}

void pin2Interrupt()
{
  /* Aquí se vuelve del modo dormido, se desactiva para así evitar tener que estar
  viniendo a aquí mientras esté activo el pulso
   */
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
}

void enterSleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable(); //Activa el sleep_mode
  sleep_mode(); //Entra en modo dormido

  /*El programa sigue a partir de aquí*/
  sleep_disable(); /*Desactiva el modo dormido*/

  /*Activa todos los periféricos, útil para que no consuman*/
  power_all_enable();
}

void setup() {
  Serial.begin(9600); // Empieza la comunicación serial
  Wire.begin(); // Empieza la comunicación i2c
  Timer1.initialize(1000000); // Timer1 es de 16 bits con interrupción cada 1*10^6us=1s
  Timer1.attachInterrupt(interrupcion);
  pinMode( IN1, OUTPUT);
  pinMode( IN2, OUTPUT);
  pinMode( FCU, INPUT_PULLUP);
  pinMode( FCD, INPUT_PULLUP);
  pinMode( SQW, INPUT_PULLUP); //Entrada para el pulso cuadrado del RTC
  /*En la subida del pulso cuadrado se activa la interrupción y sale del sleep */
  attachInterrupt(SQWpin, pin2Interrupt, RISING);
  //Toma la hora del DS3231, luego no hará falta repetir la función porque con la función interrupción se va cambiando sola
  setSyncProvider(RTC.get);    // setSyncProvider() causes the Time library to synchronize with the
  if(timeStatus() != timeSet)  // external RTC by calling RTC.get() every five minutes by default.
      Serial.println(F("No se pudo conectar con el reloj"));
  else
      Serial.println(F("Tiempo ajustado por el reloj"));

  tomarHora(); //Guardo la hora en las variables locales

  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE); //Es mejor declarar ésto antes de declarar las alarmas

  //Alarma1 a las 12:00
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, 0, 12, 1);
  RTC.alarm(ALARM_1);                   //Limpia el flag del reloj
  RTC.alarmInterrupt(ALARM_1, true);

  diaDelAno = calculateDayOfYear( dia, mes, ano);

  horaCerrar = horaDeDiasCerrar[diaDelAno];
  minutoCerrar = minutoDeDiasCerrar[diaDelAno];

  // La alarma2 cambia para cada día
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, minutoCerrar, horaCerrar, 1);
  RTC.alarm(ALARM_2);                   //Limpia el flag del reloj
  RTC.alarmInterrupt(ALARM_2, true);

}

void loop()
{
  /*Si ha cambiado el día, tomar la hora del día siguiente para cerrar*/


  datosPorPantalla();
  escribirHora();
  Serial.println();
  enterSleep();
}
