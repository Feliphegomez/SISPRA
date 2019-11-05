/* 
#########################################################
#    Processor:          Arduino UNO, MEGA ou Teensy++ 2.0
#    Language:           Wiring / C /Processing /Fritzing / Arduino IDE
#    Author:             Andres Felipe Gomez Maya
#    place:              Medellin, Colombia
#########################################################
#  RTC-I2C: DS:SCL->ARD:SCL  DS:SDA->ARD:SDA  DS:VCC->5V  DS:GND->GND
#           **NOTA**: se debe cargar dos veces este programa 1.Con la linea 9= RTC.adjust(DateTime(__DATE__, __TIME__));
#  DTH:     DTH:OUT->ARD:D2
#    
*/
#include <SoftwareSerial.h>      // Libreria para comunicacion serial
#include <Wire.h> 
#include <TimeLib.h>
#include <DS1307RTC.h>
#include "SimpleDHT.h"           // Libreria para DTH

int pinTX = 6;          // BT1  - PIN 6 como TX
int pinRX = 5;          // BT1  - PIN 5 como RX
int pinForceAT = 4;     // BT1  - P.IN 4 para forzar modo AT de configuración
int pinVCC_BT = 3;      // BT1  - PIN 3 como alimentacion 3.3V para modulo BT
int pinDTH = 2;         // DHT  - PIN 2 para DTH
int pinMoisture = A0;
int pinWaterLevel = A1;

int power_status = 0;   // 0=Auto 1=Manual
int segActual = 0;
int totalLoop = 0;
int totalBackLoop = 0;
String horaActual;
String fechaActual;

String DTH_temperature;
String DTH_humidity;
byte temperature = 0;
byte humidity = 0;
int err = SimpleDHTErrSuccess;
int lastSyncTH = -1;

int WaterLevel;

int valorHumedad;
String FC_humidity;

int minH = 20;
int maxH = 45;

#define pinDTH pinDTH

SimpleDHT22 dht22; // Crear DTH22
SoftwareSerial BT1(pinRX, pinTX);  // Crear BT1 - pin 10 como RX, pin 11 como TX

tmElements_t tm;

void setup () {
  Wire.begin();                       // Se inicial la interfaz I2c
  Serial.begin(9600);                 // comunicacion de monitor serial a 9600 bps
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("Iniciando BT1..");  
  pinMode(pinForceAT, OUTPUT);        // Al poner/2 en HIGH forzaremos el modo AT
  pinMode(pinVCC_BT, OUTPUT);         // cuando se alimente de aqui
  pinMode(pinForceAT, OUTPUT);        // Al poner/2 en HIGH forzaremos el modo AT
  pinMode(pinVCC_BT, OUTPUT);         // cuando se alimente de aqui
  // digitalWrite(pinForceAT, HIGH);   //  Forzar AT para configuracion BT  
  delay(500);                         // Espera antes de encender el modulo
  digitalWrite(pinVCC_BT, HIGH);     // Enciende el modulo BT
  BT1.begin(38400);                  // comunicacion serie entre Arduino y el modulo a 38400 bps

  pinMode(pinMoisture, INPUT);
  pinMode(pinWaterLevel, INPUT);
}

void loop(){ 
  totalBackLoop = totalLoop;
  BT1.listen();
  if (BT1.available()) { recibeInfo(BT1.readString()); } // si hay informacion disponible desde modulo
  if (Serial.available()) { recibeInfo(Serial.readString()); } // si hay informacion disponible desde el monitor serial
  if (RTC.read(tm)) {
    if(totalLoop >= 0){
      if(segActual == (int(tm.Minute) * 60) + int(tm.Second)){
        totalLoop = totalLoop + 1;
      } else {        
        segActual = (int(tm.Minute) * 60) + int(tm.Second);
        totalLoop = 0;
        String Horas = (int(tm.Hour) >= 0 && int(tm.Hour) < 10) ? "0" + String(tm.Hour) : String(tm.Hour);
        String Minutos = (int(tm.Minute) >= 0 && int(tm.Minute) < 10) ? "0" + String(tm.Minute) : String(tm.Minute);
        String Segundos = (int(tm.Second) >= 0 && int(tm.Second) < 10) ? "0" + String(tm.Second) : String(tm.Second);
        
        // horaActual = Horas + ':' + String(tm.Minute) + ':' + String(tm.Second);
        horaActual = Horas + ':' + Minutos + ':' + Segundos;
        fechaActual = String(tm.Day) + '/' + String(tm.Month) + '/' + String(tmYearToCalendar(tm.Year));
        
      }
    }
    if (totalLoop == 0){    
      mostrarDatos("*y" + fechaActual + "*");
      mostrarDatos("*x" + horaActual + "*");
      if(segActual >= (lastSyncTH + 2)){ checkerTH(); }
      
      int humedad = analogRead(pinMoisture);
      float H = map(analogRead(pinMoisture), 0, 1023, 100, 0) ;
      
      Serial.print("humedad 1 ");
      Serial.println(humedad);

      if(humedad > 200){
        if(humedad < 500){
          Serial.println("La humedad del suelo esta por debajo de 500 en nuestra lectura analogica");
        }
        mostrarDatos("*M" + String(H) + "*");
        //delay(1000);
      } else {
        Serial.println("Problemas con el FC.");
      }

      int WaterLevel = analogRead(pinWaterLevel);//read the water sensor value
      mostrarDatos("*W" + String(WaterLevel) + "*");
      
      if (WaterLevel <= 50) {
        Serial.println("Tanque de agua con bajo nivel");
        delay(200);
      }
    }
  } else {
    Serial.println("Error en DS1307.");
    if (RTC.chipPresent()) {
      Serial.println("El DS1307 se detiene. Por favor ejecute el SetTime-RV1, ");
      Serial.println("ejemplo para inicializar el tiempo y comenzar a correr.");
      Serial.println();
    } else {
      Serial.println();
    }
    delay(300);
  }
}

void checkerTH(){
  if(RTC.read(tm)) {
      if(lastSyncTH == -1){ lastSyncTH = (int(tm.Minute) * 60) + int(tm.Second); }
      lastSyncTH = (int(tm.Minute) * 60) + int(tm.Second); // Volver a actulizar la ultima activadad del DHT
      if ((err = dht22.read(pinDTH, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
        Serial.print("No reading , err=");
        Serial.println(err);
        return;
      } else {
        DTH_temperature = ("*T"+String(temperature)+"*");
        DTH_humidity = ("*H"+String((int)humidity)+"*");
        mostrarDatos(DTH_temperature);
        
        mostrarDatos(DTH_humidity);
      }
  }
}

void recibeInfo(String infoRecibe){
  if(infoRecibe == "none"){} else {
      mostrarDatos("Comando recibido: " + infoRecibe);
      // delay(500);
      if(infoRecibe.indexOf("dth_basic") >= 0){
        mostrarDatos(DTH_temperature);
        mostrarDatos(DTH_humidity);
      } else if(infoRecibe.indexOf("temperature") >= 0){
        mostrarDatos(DTH_temperature);
      } else if(infoRecibe.indexOf("humidity") >= 0){
        mostrarDatos(DTH_humidity);
      } else if(infoRecibe.indexOf("moisture") >= 0){
        mostrarDatos(FC_humidity);
      }
  }
}

void mostrarDatos(String message){
  //BT1.println(message);
  Serial.println(message);
}
