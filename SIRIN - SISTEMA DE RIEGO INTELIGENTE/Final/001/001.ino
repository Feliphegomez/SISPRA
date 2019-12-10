#include <SoftwareSerial.h>      // Libreria para comunicacion serial
#include <SimpleDHT.h>

int pBB1  = 9;  // Bombeo 1
int pLL1  = 8;  // Luces
int pBZ1  = 7;  // PIN 7 como BUZZER1
int pBTX = 6;   // PIN 6 como TX
int pBRX = 5;   // PIN 5 como RX
int pBAT = 4;   // PIN 4 para forzar modo AT de configuración
int pBVC = 3;   // PIN 3 como alimentacion 3.3V para modulo BT
int pDTH = 2;   // PIN 2 para DTH
int pM1  = A0;  // 
int pWS1 = A1;  // 
float T = 0;
float H = 0;
float M = 0;
float W = 0;

SimpleDHT22 dht22(pinDTH);
SoftwareSerial BT1(pinRX, pinTX);  // Crear BT1 - pin 10 como RX, pin 11 como TX

void setup() {
  Serial.begin(9600);                 // comunicacion de monitor serial a 9600 bps
  while (!Serial) ; // wait for serial
  delay(200);
  pinMode(LED_BUILTIN, OUTPUT); //  PIN 13 - Salida led principal
  pinMode(pBB1, OUTPUT); //  PIN 09 - Salida relé Bombeo 1
  pinMode(pLL1, OUTPUT); //  PIN 08 - Salida relé Luces
  pinMode(pBZ1, OUTPUT); //  PIN 07 - Salida Buzzer
  pinMode(pBAT, OUTPUT); //  PIN 07 - Forzarmodo AT em bluetooth
  
  // pinMode(pBAT, OUTPUT);           // En HIGH/LOW forzaremos el modo AT
  delay(500);                         // Espera antes de encender el modulo
  // pinMode(pBAT, OUTPUT);           // se alimente de aqui
  pinMode(pinVCC_BT, OUTPUT);         // pBVC
  // digitalWrite(pinForceAT, HIGH);   //  Forzar AT para configuracion BT
  BT1.begin(38400);                  // comunicacion serie entre Arduino y el modulo a 38400 bps
  if(digitalRead(pinForceAT) == HIGH){
    Serial.println("Modo configuracion de AT Bluetooth:");
  }
  pinMode(pinEV, OUTPUT);         // Electrovalvula
  digitalWrite(pinEV, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  
  if(digitalRead(pinForceAT) == HIGH){
    if (BT1.available()){ Serial.write(BT1.read()); }
    if (Serial.available()){ BT1.write(Serial.read()); }
  } else {
    BT1.listen();
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Serial.println("Ciclo Nuevo");
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
    Serial.println(err);
    if (BT1.available()){
      BT1.print("Read DHT22 failed, err="); BT1.println(err);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    delay(1000);
    return;
  } else {
     // Serial.println("Nuevo ciclo: " + String(millis()));
  }
}
