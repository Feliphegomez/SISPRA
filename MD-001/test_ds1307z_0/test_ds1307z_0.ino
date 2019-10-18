/* 
#########################################################
#    Processor:          Arduino UNO, MEGA ou Teensy++ 2.0
#    Language:           Wiring / C /Processing /Fritzing / Arduino IDE
#    Objectives:         Test Module - Tiny RTC DS1307 Z
#    Author:             Andres Felipe Gomez Maya
#    Date:               17/10/2019
#    place:              Medellin, Colombia
#########################################################
*/

//  Prueba de Reloj Tiempo Real (RTC) y Modulo I2C
//  Conexion:
//    Arduino UNO  SCL->SCL  SDA->SDA  VCC->5V  GND->GND
//  
//    NOTA: se debe cargar dos veces este programa 1.Con la linea 9= RTC.adjust(DateTime(__DATE__, __TIME__));

#include <Wire.h> 
#include "RTClib.h"
RTC_DS1307 RTC;
#define A0 A0

void setup () {
  Serial.begin(9600); // Establece la velocidad de datos del puerto serie
  Serial.println("Iniciando el puerto I2C");
  Wire.begin();  
  Serial.println("Abriendo Wire.");  
  pinMode(A0, INPUT);
  
  RTC.begin(); // Inicia la comunicaci¢n con el RTC
  // RTC.adjust(DateTime(__DATE__, __TIME__)); // Establece la fecha y hora (Comentar una vez establecida la hora)
} 


void loop(){
    DateTime now = RTC.now(); // Obtiene la fecha y hora del   RTC
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
}
