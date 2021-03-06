

/* Titulo : SISTEMA HIDROPONÍA
 *  Descripción: Este sketch permite al usuario seleccionar entre varios modos de tiempos predeterminados para 
 *  el accionamiento de la motobomba, en un sistema hidropónico. Interactúa con el hardware controlador arduino
 *  nano CH340, lcd I2C 16x2 y el módulo de reloj RTC DS3231. Se modificó parte del codigo "Prueba de Reloj Tiempo 
 *  Real (RTC) y Modulo I2C para LCD 2x16 By: http://dinastiatecnologica.com.
 *  Autores: Norberto Moreno Vélez, Julian Quiceno
 *  Fecha: 26/06/2019
  */
 
// Conexion i2C :Arduino UNO  SCL-> A5   SDA->A4  VCC->5V  GND->GND
// Conexion RTC :Arduino UNO    GND->GND VCC->5V SCL-> SCL  SDA->SDA los dos pines despues del ...12,13,GND,AREF,SDA,SCL
// NOTA: se debe cargar dos veces este programa 1.Con la linea 9= RTC.adjust(DateTime(__DATE__, __TIME__));
//                                              2. Sin esa linea

#include <Wire.h> 
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
RTC_DS1307 RTC;

// Entradas digitales
int rele = 3;
int pulsador_hora = 2;
int pulsador_min = 4;
int pulsador_set = 5;
int pulsador_hora_menos =6;
int pulsador_min_menos = 7;

 // Contadores
int contador_horas =0;
int contador_min =0;
int contador_horas_fin =0;
int contador_min_fin = 0;
int contador_set = 0;
int contador_mod2 = 0; // Contador para tomar referencia de minuto en selección de riego modo 2 ( cada 15 minutos)
int contador_serial = 0;

int flag_modo;


void setup () {
  Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicaci¢n con el RTC
//RTC.adjust(DateTime(__DATE__, __TIME__)); // Establece la fecha y hora (Comentar una vez establecida la hora)
//el anterior se usa solo en la configuracion inicial
  Serial.begin(9600); // Establece la velocidad de datos del puerto serie
  pinMode(rele,OUTPUT);
  pinMode(pulsador_hora,INPUT_PULLUP);
  pinMode(pulsador_min,INPUT_PULLUP);
  pinMode(pulsador_set,INPUT_PULLUP);
  pinMode(pulsador_hora_menos,INPUT_PULLUP);
  pinMode(pulsador_min_menos,INPUT_PULLUP);
  digitalWrite(rele,LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Sistema riego");
  lcd.setCursor(3,1);
  lcd.print("Hidroponia");
  delay(3000);
  lcd.clear();

 // Pantallazo 1
  lcd.setCursor(4,0);
  lcd.print("Elija el modo de riego...");
  for (int positionCounter = 0; positionCounter < 23; positionCounter++) {
    
  // scroll one position left:
    lcd.scrollDisplayLeft();
  // wait a bit:
    delay(300);
  }
  
  delay(2000);
  
  lcd.clear();
  lcd.print("...");
  lcd.clear();


//Inicio selección de modo de riego

  do{
    int set_state = digitalRead(pulsador_set);
    int pulsador_select_state = digitalRead(pulsador_hora);
    // int pulsador_set_state = set_state(contador_set);
    if (set_state == false) {
      contador_set++;
      delay(400);
    }
    else if (contador_set == 1) {
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("1.Riego");
      lcd.setCursor(3,1);
      lcd.print("programado.");
      flag_modo = 1;
      if (pulsador_select_state == false) {
        lcd.clear();
        break;
        }
      delay(200);
    }
    else if (contador_set == 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" 2.Riego 15 min,");
      lcd.setCursor(3,1);
      lcd.print("cada hora.");
      flag_modo = 2;
    
      if (pulsador_select_state == false) {
        lcd.clear();
        break;
        }
      delay(200);
    }
    
 /* else if (pulsador_set_state == 3) {
  *  lcd.clear();
    lcd.print("3.Riego 30 min, cada hora.");
  }*/
  
  else if (contador_set > 2) {
    contador_set = 1; // Reseteo el contador del set para volver a la opción 1
  }
  else {
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pulsar set para"); // Poner a circular
    lcd.setCursor(0,1);
    lcd.print("seleccionar modo...");
    flag_modo = 0;
    delay(200);
  }
  //delay(300);
}while (true); // Fin selección modo de riego


// Set de selección riego programado
  do{
    int state_pul_hora = digitalRead(pulsador_hora);
    int state_pul_min = digitalRead(pulsador_min);
    int state_pul_set = digitalRead(pulsador_set);
    int state_pul_hora_menos = digitalRead(pulsador_hora_menos);
    int state_pul_min_menos = digitalRead(pulsador_min_menos);
  
  
    if (state_pul_hora == false) {
      contador_horas++;
      if (contador_horas >23) {
        contador_horas = 0;
      }
      delay(200);
    }
    //else if (state_pul_hora_menos == false) {
     // contador_horas--;
     // delay(200);
    //}
    else if (state_pul_min == false) {
      contador_min++;
      if (contador_min >59) {
        contador_min = 0;
      }
      delay(200);
    }
    //else if (state_pul_min_menos == false) {
     // contador_min--;
     // delay(200);
    //}
    else if (state_pul_set == false) {
      flag_modo = 3;
      lcd.clear();
      delay(1000);
      break;
    }
    lcd.setCursor(0,0);
    lcd.print("INICIO RIEGO 1: ");
    lcd.setCursor(6,1);
    lcd.print(contador_horas);
    lcd.print(":");
    lcd.print(contador_min);
  
  }while(flag_modo == 1);


// Set de configuración FINAL DE RIEGO 1
do{
  int state_pul_hora = digitalRead(pulsador_hora);
  int state_pul_min = digitalRead(pulsador_min);
  int state_pul_set = digitalRead(pulsador_set);
  int state_pul_hora_menos = digitalRead(pulsador_hora_menos);
  int state_pul_min_menos = digitalRead(pulsador_min_menos);
  
  
  if (state_pul_hora == false) {
    contador_horas_fin++;
    if (contador_horas_fin >23) {
      contador_horas_fin = 0;
    }
    delay(200);
  }
  //else if (state_pul_hora_menos == false) {
   // contador_horas_fin--;
    //delay(200);
 // }
  else if (state_pul_min == false) {
    contador_min_fin++;
    if ( contador_min_fin > 59) {
      contador_min_fin = 0;
    }
    delay(200);
  }
  //else if (state_pul_min_menos == false) {
    //  contador_min_fin--;
      //delay(200);
    //}
  else if (state_pul_set == false) {
    lcd.clear();
    break;
  }
  lcd.setCursor(0,0);
  lcd.print("FINAL RIEGO 1: ");
  lcd.setCursor(6,1);
  lcd.print(contador_horas_fin);
  lcd.print(":");
  lcd.print(contador_min_fin);
  
}while(flag_modo == 3);

//lcd.clear();
lcd.setCursor(4,0);
lcd.print("Inicio de ciclo de riego...");
for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
  // scroll one position left:
  lcd.scrollDisplayLeft();
  // wait a bit:
    delay(300);
  }
delay(3000);
lcd.clear();


}


// *Función de tiempo real: Devuelve un dato de tipo DateTime

DateTime tiempo_real() {
  
  DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
  Serial.print(now.year(), DEC); // A§o
  Serial.print('/');
  Serial.print(now.month(), DEC); // Mes
  Serial.print('/');
  Serial.print(now.day(), DEC); // Dia
  Serial.print(' ');
  Serial.print(now.hour(), DEC); // Horas
  Serial.print(':');

  Serial.print(now.minute(), DEC); // Minutos
  Serial.print(':');
  Serial.print(now.second(), DEC); // Segundos
  Serial.println();
  // delay(1000);

  lcd.setCursor(4,0);
  //lcd.print("");
  lcd.print(now.year(), DEC);
  lcd.print("/");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.day(), DEC);
  lcd.print(" ");
  lcd.setCursor(4,1);
 // lcd.print("T: ");
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  lcd.print(now.second(), DEC);
  delay(1000); // La informaci¢n se actualiza cada 1 seg.
 /* lcd.setCursor(0,0);
  lcd.print("               ");
  lcd.setCursor(0,1);
  lcd.print("               "); */

  
  return now;
}

void loop(){

DateTime minuto_referencia;
DateTime diferencia_minuto;
DateTime t1;
DateTime actual = tiempo_real();

if ((contador_horas - contador_horas_fin) == 0 && flag_modo == 3) {
  if (actual.minute() >= contador_min && actual.minute() < contador_min_fin) {
    digitalWrite(rele,HIGH);
  }
  else if (actual.minute() >= contador_min_fin) {
    digitalWrite(rele,LOW);
  }
  
}
/*
// Cuando se activa el riego mas de una hora
if ((contador_horas - contador_horas_fin) != 0 ) {
  int diferencia_en_min = ((contador_horas_fin - now.hour())*60)+(contador_min_fin- now.minute()); // Tiempo en horas conversion a minutos
  if (now.hour() >= contador_horas && now.hour() < contador_horas_fin) {
    if (diferencia_en_min <= contador_min && diferencia_horas contador_min_fin) { // Mala logica
      digitalWrite(rele,HIGH);
    }
    
  }
  else if (now.minute() >= contador_min_fin) {
    digitalWrite(rele,LOW);
  }
  
}
/*
else if (now.hour() >= contador_horas && now.hour() < contador_horas_fin) {  // Hora programada
  if (now.minute() >= contador_min && now.minute() < contador_min_fin) {
    digitalWrite(rele,HIGH);
  }
  
}*/

else if (flag_modo == 2) {   //Modo riego 15 minutos cada hora
  if (contador_mod2 == 0) { // Si contador modo de riego 2 está en cero
    minuto_referencia = actual.minute(); //Minuto de referencia para comparación
    contador_mod2++;
  }
  else if (contador_mod2 != 0) {
    //t1 = fecha_referencia + TimeSpan(0,0,15,0); //diferencia_minuto = (now.minute() - minuto_referencia); si le añado 15 minutos a la fecha de referencia entonces...
    //if (actual.minute() - minuto_referencia == 0){
      if ( actual.minute() >=0 && actual.minute() < 15) {
        digitalWrite(rele,HIGH);
        //contador_serial++;
        Serial.println(contador_serial);
    }
    else if (actual.minute()>= 15) {
      digitalWrite(rele,LOW);
    }
  }
}

else {
  digitalWrite(rele,LOW);
}


}
