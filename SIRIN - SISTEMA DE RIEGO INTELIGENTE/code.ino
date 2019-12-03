#include <SoftwareSerial.h>      // Libreria para comunicacion serial
#include <Wire.h>
#include <SimpleDHT.h>

int pinFC = A0;             // 
int pinEV = 9;              // Electrovalvula - PIN 9
int pinTX = 6;              // BT1  - PIN 6 como TX
int pinRX = 5;              // BT1  - PIN 5 como RX
int pinForceAT = 4;         // BT1  - PIN 4 para forzar modo AT de configuración
int pinVCC_BT = 3;          // BT1  - PIN 3 como alimentacion 3.3V para modulo BT
int pinDTH = 2;             // DHT  - PIN 2 para DTH
int pinMoisture = A0;       // 
int pinWaterSensor = A1;    // 

int statusEV = 0;
volatile byte relayEVState = LOW;

SimpleDHT22 dht22(pinDTH);
SoftwareSerial BT1(pinRX, pinTX);  // Crear BT1 - pin 10 como RX, pin 11 como TX

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinForceAT, OUTPUT);        // Al poner/2 en HIGH forzaremos el modo AT
  pinMode(pinVCC_BT, OUTPUT);         // cuando se alimente de aqui
  pinMode(pinForceAT, OUTPUT);        // Al poner/2 en HIGH forzaremos el modo AT
  pinMode(pinVCC_BT, OUTPUT);         // cuando se alimente de aqui
  // digitalWrite(pinForceAT, HIGH);   //  Forzar AT para configuracion BT
  delay(500);                         // Espera antes de encender el modulo
  
  digitalWrite(pinVCC_BT, HIGH);     // Enciende el modulo BT
  BT1.begin(38400);                  // comunicacion serie entre Arduino y el modulo a 38400 bps
  
  if(digitalRead(pinForceAT) == HIGH){
    Serial.println("Modo configuracion de AT Bluetooth:");
  }
  
  pinMode(pinEV, OUTPUT);         // Electrovalvula
  digitalWrite(pinEV, HIGH);
  // attachInterrupt(digitalPinToInterrupt(PIRInterrupt), detectMotion, RISING);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);                 // comunicacion de monitor serial a 9600 bps
  if(digitalRead(pinForceAT) == HIGH){
    if (BT1.available()){ Serial.write(BT1.read()); }
    if (Serial.available()){ BT1.write(Serial.read()); }
  } else {
    BT1.listen();
  }
  
  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err);
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
      //delay(631);
      Serial.println("DHT: ");
     // Serial.print((float)temperature); Serial.print(" *C, ");
      //Serial.print((float)humidity); Serial.println(" RH%");
      Serial.println("*T"+String(temperature)+"*");
      Serial.println("*H"+String(humidity)+"*");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      
      //Send data over Bluetooth
      BT1.println("*T"+String(temperature)+"*");
      BT1.println("*H"+String(humidity)+"*");
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);

      int sensorWater =analogRead(pinWaterSensor);
      Serial.println("SensorWater: ");
      Serial.println("*W"+String(sensorWater)+"*");
      BT1.println("*W"+String(sensorWater)+"*");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);

      int sensorMoisture =analogRead(pinMoisture);
      sensorMoisture = map(sensorMoisture,1023,0,0,100);
      Serial.println("SensorMoisture: ");
      Serial.println("*M"+String(sensorMoisture)+"*");
      BT1.println("*M"+String(sensorMoisture)+"*");
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);

      // activar riego
      if(sensorMoisture <= 10){ statusEV = 1; }
      else { statusEV = 0; }
      
      Serial.println("statusEV: " + String(statusEV));
      if(statusEV == 1){
        Serial.println("EV Encendida: ");
        digitalWrite(pinEV, LOW);
      } else {
        Serial.println("EV por defecto: ");
        digitalWrite(pinEV, HIGH);
      }
  }
}