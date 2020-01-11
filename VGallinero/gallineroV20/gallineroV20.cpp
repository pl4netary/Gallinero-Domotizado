//Código abierto: Luar Biurrun-Lorda Aramayo-Lus
//Versión 2.4
//Fecha: 02/04/2019
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

MEJORAS para hacer:
  -Ajuste automático del cambio de hora anual
  -Entrar en modo sleep para no consumir tanto
  -Añadir sensor de temperatura
  -Implementar el ajuste de hora y solicitud abrir/cerrar por terminal
CONSUMO:
-Módulo solar 10mA
-Módulo solar + Módulo control 77mA
-Módulo solar + Módulo control + Motor
*/


#include <Arduino.h>
#include <TimerOne.h> // Esta librería inutiliza la señal PWM de los pines 9 y 10
#include <RTClib.h>   // pero se puede usar la función pwm()
#include <Wire.h>


//Pines para el L298N
#define ENA  11 //Señal conPWM
#define IN1  12
#define IN2  13
#define FCU  6 //Final Carrera Arriba ( Up)
#define FCD  7 //Final Carrera Abajo ( Down)

#define AjusteMS1	20
#define AjusteMS2	20

RTC_DS3231 RTC;
const int DS3231 = 0x68; //Dirección I2C del RTC

boolean estadoFCU = false;
boolean estadoFCD = true;
int hora, minut, seg, dia, mes, ano, diaSiguiente, horaAbrir, horaCerrar;
boolean pasadoHora;
boolean pasadoSegundo;
const uint8_t horaDeDiasAbrir[ ] = { 8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 };
const uint8_t horaDeDiasCerrar[ ] = { 17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,19 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,18 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 ,17 };
int diaDelAno = 0;

/*-----------------------------------------Índice de funciones-----------------------------------------*/
void interrupcion();
void tomarHora();
void puerta();
void datosPorPantalla();
void escribirHora();
boolean leeFinalCarrera( uint8_t pin);
int calculateDayOfYear(int day, int month, int year);
void abrir();
void cerrar();
void parar();

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Empieza la comunicación i2c
  Timer1.initialize(1000000); // Timer1 es de 16 bits con interrupción cada 1*10^6us=1s
  Timer1.attachInterrupt(interrupcion);
  pinMode( ENA, OUTPUT);
  pinMode( IN1, OUTPUT);
  pinMode( IN2, OUTPUT);
  /*Los pines se configuran con una resistencia de 20K interna para los finales de carrera.
  FC pulsado = 1  FC sin pulsar = 0 */
  pinMode( FCU, INPUT_PULLUP);
  pinMode( FCD, INPUT_PULLUP);
  //Toma la hora del DS3231, luego no hará falta repetir la función porque con la función interrupción se va cambiando sola
  delay( 2000);
  Serial.println( F( "Arrancando..."));
  tomarHora();
  Serial.println( "Hora tomada");
  Serial.println( "Empezando...");
}

void loop() {
  diaDelAno = calculateDayOfYear( dia, mes, ano);
  horaAbrir = 11; //= horaDeDiasAbrir[ diaDelAno];
  horaCerrar = horaDeDiasCerrar[ diaDelAno] + 1;
  datosPorPantalla();
  puerta();
  escribirHora();
  delay(500);
  Serial.println();
}

boolean leeFinalCarrera(uint8_t pin) {
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

void tomarHora() {
  DateTime now = RTC.now();
  hora = now.hour();
  minut = now.minute();
  seg = now.second();
  dia = now.day();
  mes = now.month();
  ano = now.year();
}

void interrupcion() {
  seg++;
  if ( seg > 59) {
    seg = 00;
    minut++;
    if ( minut > 59) {
      minut = 0;
      hora++;
      if ( hora > 23) {
        hora = 00;
        pasadoHora = HIGH;
      }
     }
   }
   pasadoSegundo = HIGH;
 }

 void puerta(){
   estadoFCU = leeFinalCarrera( FCU);
   estadoFCD = leeFinalCarrera( FCD);
   //Para abrir
   if(  hora >= horaAbrir && estadoFCD && !estadoFCU && hora < horaCerrar){ // Cuando sean más de la hora de abrir y FCD pulsado
      abrir();
      estadoFCU = leeFinalCarrera(FCU);
      estadoFCD = leeFinalCarrera(FCD);
      delay(1500);
     while(estadoFCD){
       abrir();
       datosPorPantalla();
       escribirHora();
       estadoFCU = leeFinalCarrera(FCU);
       estadoFCD = leeFinalCarrera(FCD);
     }
   }
   //Para cerrar
   else if( (hora > horaCerrar - 1) && estadoFCU && !estadoFCD ){ // Cuando sea más de la hora de cerrar, FCU pulsado y FCD sin pulsar
     cerrar();
     estadoFCU = leeFinalCarrera(FCU);
     estadoFCD = leeFinalCarrera(FCD);
     delay(1500);
    while(estadoFCU){
      cerrar();
      datosPorPantalla();
      escribirHora();
      estadoFCU = leeFinalCarrera(FCU);
      estadoFCD = leeFinalCarrera(FCD);
      delay(2000);
    }
   }
   else parar();
 }

 //Funciones para girar un motor DC con el driver L298N
 void abrir(){
   Serial.println( "Abriendo");
   digitalWrite( IN1, LOW);
   digitalWrite( IN2, HIGH);
   analogWrite( ENA, 255);
   //delay(AjusteMS1);
 }

 void cerrar(){
   Serial.println( "Cerrando");
   digitalWrite( IN1, HIGH);
   digitalWrite( IN2, LOW);
   analogWrite( ENA, 255);
   //delay(AjusteMS1);
 }

void parar(){
   Serial.println( "Parado");
   digitalWrite( IN1, LOW);
   digitalWrite( IN2, LOW);
   analogWrite( ENA, 0);
   //delay(AjusteMS1);
 }

//Escribe la fecha por el monitor Serial
void escribirHora(){
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

void datosPorPantalla(){
  Serial.print(F("Día: "));
  Serial.print( diaDelAno);
  Serial.println();
  Serial.println( F( "FCU     FCD   Abrir   Cerrar"));
  Serial.print(estadoFCU);
  Serial.print(F( "       "));
  Serial.print(estadoFCD);
  Serial.print("     ");
  Serial.print(horaAbrir);
  Serial.print(":00");
  Serial.print(F("   "));
  Serial.print(horaCerrar);
  Serial.print(":00");
  Serial.println();
}
//NO CALCULA EL CAMBIO DE HORA
int calculateDayOfYear(int day, int month, int year) {

  // Given a day, month, and year (4 digit), returns
  // the day of year. Errors return 999.

  int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

  // Verify we got a 4-digit year
  if (year < 1000) {
    return 999;
  }

  // Check if it is a leap year, this is confusing business
  // See: https://support.microsoft.com/en-us/kb/214019
  if (year%4  == 0) {
    if (year%100 != 0) {
      daysInMonth[1] = 29;
    }
    else {
      if (year%400 == 0) {
        daysInMonth[1] = 29;
      }
    }
   }

  // Make sure we are on a valid day of the month
  if (day < 1)
  {
    return 999;
  } else if (day > daysInMonth[month-1]) {
    return 999;
  }

  int doy = 0;
  for (int i = 0; i < month - 1; i++) {
    doy += daysInMonth[i];
  }

  doy += day;
  return doy;
}
