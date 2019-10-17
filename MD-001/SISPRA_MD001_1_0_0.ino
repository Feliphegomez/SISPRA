/* 
#########################################################
#    Processor:          Arduino UNO, MEGA ou Teensy++ 2.0
#    Language:           Wiring / C /Processing /Fritzing / Arduino IDE
#    Objectives:         Watering System - Irrigation Modular
#    Author:             Andres Felipe Gomez Maya
#    Date:               13/10/2019
#    place:              Medellin, Colombia
#########################################################
#    Libs: 
#    http://www.rinkydinkelectronics.com/library.php?id=5
#########################################################
*/
#include <Wire.h>
#include <DS1302.h>
#include <SoftwareSerial.h>  // libreria que permite establecer pines digitales
#include <SimpleDHT.h>

int pinForceAT = 12;     // pin 12 para forzar modo AT de configuraci?n
int pinTX = 11;          // pin 11 como TX
int pinRX = 10;          // pin 10 como RX
int pinVCC_BT = 9;       // pin 9 como alimentacion 3.3V para modulo BT
SoftwareSerial BT1(pinRX, pinTX);  // Crear BT1 - pin 10 como RX, pin 11 como TX

int pinDTH = 8;         // pin 8 para DTH
#define pinDTH pinDTH
#define pinH_FC A0
SimpleDHT22 dht22; // Crear DTH22

int Red_LED_Pin = 6;     // Pin 6 PWM Red LED
int Green_LED_Pin = 5;   // Pin 5 PWM Green LED
int Blue_LED_Pin = 3;    // Pin 3 PWM Blue LED
// Luces
int Red_value = 0;
int Green_value = 0;
int Blue_value = 0;

int RTC_CLK = 7;          // Pin 7 - CLK modulo RTC
int RTC_DAT = 4;          // Pin 4 - I/O DAT modulo RTC
int RTC_RST = 2;          // Pin 2 - RST modulo RTC

// Inicializacion del modulo
DS1302 RTC(RTC_RST, RTC_DAT, RTC_CLK); // RST, DAT, CLK

// system status
const char *system_status_list[] =
{
  "Create by FelipheGomez",
  "Modo Manual",
  "Stand By (AUTO)",
  "Levantando el modulo HC-06",                 // 3
  "Listo para recibir desde BT",
  "Configurando Luces",                         // 5
  "Listo para recibir GW y HW",
  "Configurando FC-28",                         // 7
  "Configurando RTC",
  "",
};

int interval = 10;
const String system_status;
int power_status = 0;
int state_ledSingle = 0;
int flag = 0;
int LED_PWI[] = {255, 0, 0};

int DTH_Enabled = 0;
String DTH_temperature;
String DTH_humidity;

byte temperature = 0;
byte humidity = 0;
int err = SimpleDHTErrSuccess;

int valorHumedad;
String FC_humidity;

String horaActual;
String fechaActual;
Time t;

void setup(){
  setStatus(0);
  pinMode(pinForceAT, OUTPUT);        // Al poner/2 en HIGH forzaremos el modo AT
  pinMode(pinVCC_BT, OUTPUT);         // cuando se alimente de aqui
  // digitalWrite(pinForceAT, HIGH);   //  Forzar AT para configuracion BT
  delay(500);                         // Espera antes de encender el modulo
  Wire.begin(); // Se inicial la interfaz I2c
  Serial.begin(9600);                 // comunicacion de monitor serial a 9600 bps
  setStatus(3);
  digitalWrite(pinVCC_BT, HIGH);     // Enciende el modulo BT
  BT1.begin(38400);                  // comunicacion serie entre Arduino y el modulo a 38400 bps
  setStatus(4);
  
  setStatus(5);
  // Initialise LED pins as outputs
  pinMode(Red_LED_Pin, OUTPUT);       // Establece como salida para el LED Rojo
  pinMode(Green_LED_Pin, OUTPUT);     // Establece como salida para el LED Verde
  pinMode(Blue_LED_Pin, OUTPUT);      // Establece como salida para el LED Azul
  setStatus(6);
  
  setStatus(7);
  pinMode(pinH_FC, INPUT);        // Al poner en INPUT activamos el FC-28
  
  setStatus(8);
  // RTC initialization
  // Desproteger contra escritura
  RTC.halt(false);
  RTC.writeProtect(false);
  /// RTC.setDOW(MONDAY);  // Configurar dia de la semana: MARTES.
  // RTC.setTime(23, 16, 00);  // Configurar hora en formato 24hs con min y seg: 17:00:00 HORAS.
  // RTC.setDate(14, 10, 2019);  // Configurar fecha en formato dia/mes/a?o: 4/3/2014.
  setStatus(2);
}

void setStatus(int statusInt){
  if(statusInt > 0){
    system_status = system_status_list[statusInt];
    mostrarDatos("*z" + system_status + "*"); // escribe en el monitor el nuevo estado
    // delay(500);
  }
}

void loop(){
  delay(500);
  if(horaActual != RTC.getTimeStr(FORMAT_LONG)){
    horaActual = RTC.getTimeStr(FORMAT_LONG); // Invia giorno della settimana
    mostrarDatos("*x" + horaActual + "*");
  }
  if(fechaActual != RTC.getDateStr(FORMAT_LONG)){
    fechaActual = RTC.getDateStr(FORMAT_BIGENDIAN); // Invia giorno della settimana
  }
  t = RTC.getTime();
  
  
  if (BT1.available()){ recibeInfo(BT1.readString()); } // si hay informacion disponible desde modulo
  if (Serial.available()){ recibeInfo(Serial.readString()); } // si hay informacion disponible desde el monitor serial
  if(power_status == 1){
    // Serial.println("Arrancando Ciclo.");
    LED_PWI[0] = 0;
    LED_PWI[1] = 255;
    LED_PWI[2] = 0;
  } else {
    // Serial.println("Ciclo Detenido.");
    LED_PWI[0] = 0;
    LED_PWI[1] = 0;
    LED_PWI[2] = 0;
  }
  
  if(state_ledSingle == 3){ ledParty(); }
  else if(state_ledSingle == 1){
    //update LED Brightness
    analogWrite(Red_LED_Pin, Red_value);
    analogWrite(Green_LED_Pin, Green_value);
    analogWrite(Blue_LED_Pin, Blue_value);
  }
  
  if ((err = dht22.read(pinDTH, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    // Serial.print("No reading , err=");
    // Serial.println(err);
    DTH_Enabled = 0;
    delay(100);
    return;
  } else { DTH_Enabled = 1; }
  
  if(DTH_Enabled == 1){
    DTH_temperature = ("*T"+String(temperature)+"*");
    DTH_humidity = ("*H"+String((int)humidity)+"*");
    if(flag == interval){
      mostrarDatos(DTH_temperature);
      mostrarDatos(DTH_humidity);
    }
  }
  
  valorHumedad = map(analogRead(pinH_FC), 0, 1023, 100, 0);
  FC_humidity = ("*S"+String(valorHumedad)+"*");
  
  if(flag == interval){
    if((int) valorHumedad > 0){
       mostrarDatos(FC_humidity);
       // delay(150);
    }
    mostrarDatos("*z" + system_status + "*"); // escribe en el monitor el nuevo estado
    // delay(150);
    mostrarDatos("*y" + fechaActual + "*");
    // delay(150);
    mostrarDatos("*LR" + String(LED_PWI[0]) + "G" + String(LED_PWI[1]) + "B" + String(LED_PWI[2]) + "*");
    // delay(150);
  }
  
  // Reiniciar
  if(flag >= interval){ flag = 0; } else {
    flag = flag+1;
  }
}

void ledParty(){
  if(state_ledSingle == 3){
        Red_value = random(0, 255);
        Green_value = random(0, 255);
        Blue_value = random(0, 255);
        analogWrite(Red_LED_Pin, Red_value);
        analogWrite(Green_LED_Pin, Green_value);
        analogWrite(Blue_LED_Pin, Blue_value);
    delay(250);
        Red_value = random(0, 255);
        Green_value = random(0, 255);
        Blue_value = random(0, 255);
        analogWrite(Red_LED_Pin, Red_value);
        analogWrite(Green_LED_Pin, Green_value);
        analogWrite(Blue_LED_Pin, Blue_value);
    delay(250);
  }
}

void recibeInfo(String infoRecibe){
  if(infoRecibe == "none"){} else {
      mostrarDatos("Comando recibido: " + infoRecibe);
      // delay(500);
      if(infoRecibe.indexOf("device_status") >= 0){
        mostrarDatos(system_status);
      } else if(infoRecibe.indexOf("power_on") >= 0){
        power_status = 1;
        setStatus(1);
      } else if(infoRecibe.indexOf("power_off") >= 0){
        power_status = 0;
        setStatus(2);
      } else if(infoRecibe.indexOf("led_on") >= 0){
        state_ledSingle = 1;
        Red_value = random(0, 255);
        Green_value = random(0, 255);
        Blue_value = random(0, 255);
      } else if(infoRecibe.indexOf("led_off") >= 0){
        state_ledSingle = 0;
        Red_value = 0;
        Green_value = 0;
        Blue_value = 0;
        analogWrite(Red_LED_Pin, LOW);
        analogWrite(Green_LED_Pin, LOW);
        analogWrite(Blue_LED_Pin, LOW);
      } else if(infoRecibe.indexOf("led_status") >= 0){
        mostrarDatos("Estado: " + state_ledSingle);
      } else if(infoRecibe.indexOf("led_party_on") >= 0){
        state_ledSingle = 3;
      } else if(infoRecibe.indexOf("dth_basic") >= 0){
        infoDTH();
      } else if(infoRecibe.indexOf("temperature") >= 0){
        mostrarDatos(DTH_temperature);
      } else if(infoRecibe.indexOf("humidity") >= 0){
        mostrarDatos(DTH_humidity);
      } else if(infoRecibe.indexOf("moisture") >= 0){
        mostrarDatos(FC_humidity);
      } else if(infoRecibe.indexOf("LR") >= 0){
        int s = infoRecibe.indexOf("LR") + 2;
        int l = infoRecibe.length();
        String b = infoRecibe.substring(s, (l - 2));
        Red_value = b.toInt();
      } else if(infoRecibe.indexOf("LG") >= 0){
        int s = infoRecibe.indexOf("LG") + 2;
        int l = infoRecibe.length();
        String b = infoRecibe.substring(s, (l - 2));
        Green_value = b.toInt();
      } else if(infoRecibe.indexOf("LB") >= 0){
        int s = infoRecibe.indexOf("LB") + 2;
        int l = infoRecibe.length();
        String b = infoRecibe.substring(s, (l - 2));
        Blue_value = b.toInt();
      }
  }
}

void mostrarDatos(String message){
    Serial.println(message);
    BT1.println(message);
}

void infoDTH(){
  mostrarDatos(DTH_temperature);
  mostrarDatos(DTH_humidity);
}