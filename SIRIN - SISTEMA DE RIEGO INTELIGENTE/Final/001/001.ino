#include <SoftwareSerial.h>      // Libreria para comunicacion serial
#include <SimpleDHT.h>

const int pBB1  = 9;  // Bombeo 1
const int pLL1  = 8;  // Luces
const int pBZ1  = 7;  // PIN 7 como BUZZER1
const int pBTX = 6;   // PIN 6 como TX
const int pBRX = 5;   // PIN 5 como RX
const int pBAT = 4;   // PIN 4 para forzar modo AT de configuración
const int pBVC = 3;   // PIN 3 como alimentacion 3.3V para modulo BT
const int pDTH = 2;   // PIN 2 para DTH
const int pM   = A0;  // 
const int pW   = A1;  // 
float T = 0;
float H = 0;
float M = 0;
float W = 0;

SimpleDHT22 dht22(pDTH);
SoftwareSerial BT(pBRX, pBTX);  // Crear BT1 - pin 10 como RX, pin 11 como TX
int err = SimpleDHTErrSuccess;

int minW = 25;
int minH = 20;
int maxH = 45;

void setup() {
  Serial.begin(9600);  // comunicacion de monitor serial a 9600 bps
  while (!Serial) ;    // espera por el serial
  delay(200);
  
  pinMode(LED_BUILTIN, OUTPUT);     //  PIN 13 - Salida led principal
  pinMode(pBB1, OUTPUT);            //  PIN 09 - Salida relé Bombeo 1
  pinMode(pLL1, OUTPUT);            //  PIN 08 - Salida relé Luces
  pinMode(pBZ1, OUTPUT);            //  PIN 07 - Salida Buzzer
  pinMode(pBAT, OUTPUT);            // En HIGH/LOW forzaremos el modo AT // Descomentar para activar la configuracion AT
  pinMode(pBVC, OUTPUT);            // BT se alimenta de aqui
  
  // digitalWrite(pBAT, HIGH);      //  Forzar AT para configuracion BT
  delay(500);                       // Espera antes de encender el modulo
  digitalWrite(pBVC, HIGH);         // Enciende el modulo BT
  BT.begin(38400);                  // comunicacion serie entre Arduino y el modulo a 38400 bps

  digitalWrite(pBB1, HIGH);      // DEFECTO
  delay(250);
  digitalWrite(pLL1, HIGH);      // DEFECTO
  delay(250);
  
  beep(1000, 50);               // Beep de arranque
}

void loop() {
  if(digitalRead(pBAT) == HIGH){
    Serial.println("Modo configuracion de AT Bluetooth:");
    if (BT.available()){ Serial.write(BT.read()); }
    if (Serial.available()){ BT.write(Serial.read()); }
  } else {
    BT.listen();
    if (BT.available()) { recibeInfo(BT.readString()); } // si hay informacion disponible desde modulo
    if (Serial.available()) { recibeInfo(Serial.readString()); } // si hay informacion disponible desde el monitor serial
  }
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Serial.println("-*-*-*-*-*-*-*-* Ciclo Nuevo -*-*-*-*-*-*-*-*");
  loadValues();
  showValues();
  checkLevelWater();
  checkIrrigation();
  Serial.println("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");
  delay(1000); // 1 segundos para volver a empezar
}

void beep(int frequency, int duration){
  tone(pBZ1, frequency, duration); //
  delay(duration);                 // Espera
  noTone(pBZ1);                    // Stop sound...
  delay(duration);                 // ...for 1sec
}

void checkLevelWater(){
  if(W <= minW){
    showText("Tanque KO.");
    beep(1000, 100);               // Beep de alerta de agua
    delay(150);
    beep(1000, 100);               // Beep de alerta de agua
    delay(150);
    beep(1000, 100);               // Beep de alerta de agua
  } else {
    showText("Tanque OK.");
  }
}

void checkIrrigation(){
  if(M <= minH){
    showText("*Regando.");
     digitalWrite(pBB1, LOW);
  } else {
    showText("*Agua OK.");
    digitalWrite(pBB1, HIGH);
  }
}

void recibeInfo(String infoRecibe){
  if(infoRecibe == "none"){} else {
      showText("Comando recibido: " + String(infoRecibe));
      if(infoRecibe.indexOf("dth_basic") >= 0){
        showText("*T" + String(T) + "*");
        showText("*H" + String(H) + "*");
      } else if(infoRecibe.indexOf("temperatura") >= 0){
        showText("*T" + String(T) + "*");
      } else if(infoRecibe.indexOf("humedad") >= 0){
        showText("*H" + String(H) + "*");
      } else if(infoRecibe.indexOf("tierra") >= 0){
        showText("*M" + String(M) + "*");
      } else if(infoRecibe.indexOf("agua") >= 0){
        showText("*W" + String(W) + "*");
      } 
      
      else if(infoRecibe.indexOf("l_on") >= 0){
        showText("*luces encendidas*");
        digitalWrite(pLL1, LOW);
      } else if(infoRecibe.indexOf("l_off") >= 0){
        showText("*luces apagadas*");
        digitalWrite(pLL1, HIGH);
      } 
      
      else if(infoRecibe.indexOf("b_on") >= 0){
        showText("*Bomba ON*");
        digitalWrite(pBB1, LOW);
      } else if(infoRecibe.indexOf("b_off") >= 0){
        showText("*Bomba OFF*");
        digitalWrite(pBB1, HIGH);
      }
  }
}

void loadValues(){
  // delay(1000); // 2 segundos para tomar la H&T Ambiente
  if ((err = dht22.read2(&T, &H, NULL)) != SimpleDHTErrSuccess) {
    showText("Error al leer DHT22, err=");
    showText(String(err));
    T = 0;
    H = 0;
    delay(3000);
  }
  delay(1000); // 1 segundos para tomar nivel de agua
  W = map(analogRead(pW),0,500,0,100);
  delay(1000); // 1 segundos para tomar la H Tierra
  M = map(analogRead(pM), 1023, 0, 0, 100);
}

void showValues(){
  showText("*T" + String(T) + "*");
  showText("*H" + String(H) + "*");
  showText("*M" + String(M) + "*");
  showText("*W" + String(W) + "*");
}

void showText(String text){
    Serial.println(text);
    if (BT.available()){  BT.println(text);  }
}
