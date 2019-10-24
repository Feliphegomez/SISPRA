/*
##########################################################################
#  File:               BT-CAT.ino                                     #
#  Processor:          Arduino UNO, MEGA ou Teensy++ 2.0                 #
#  Language:           Wiring / C /Processing /Fritzing / Arduino IDE    #
#  Author:            Andres Felipe Gomez Maya                           #
#  Date:              13/10/2019                                         #
#  place:             Medellin, Colombia                                 #
##########################################################################
*/
#include <SoftwareSerial.h>

int pinForceAT = 4;     // pin 12 para forzar modo AT de configuración
int pinTX = 6;          // pin 11 como TX
int pinRX = 5;          // pin 10 como RX
int pinVCC_BT = 3;       // pin 9 como alimentacion 3.3V para modulo BT

SoftwareSerial BTSerial(pinRX, pinTX);  // Crear BT1 - pin 10 como RX, pin 11 como TX

void setup()
{
  pinMode(pinForceAT, OUTPUT);  // 
  pinMode(pinVCC_BT, OUTPUT);  // 
  digitalWrite(pinForceAT, HIGH);
  delay(500);
  digitalWrite(pinVCC_BT, HIGH);
  Serial.begin(9600);
  Serial.println("Enter AT commands:");
  BTSerial.begin(38400);  // HC-05 default speed in AT command more
}

void loop()
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
