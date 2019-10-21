#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

int pinSYNC = 2;          // PIN 2 como LED para el estado de la sincronizacion
int pinERROR = 3;         // PIN 2 como LED para errores

int T = 0;
int H = 0;
int minH = 60;
int maxH = 65;

int Anho = 0;
int Mes = 0;
int Dia = 0;
int Hora = 0;
int Minuto = 0;
int Segundo = 0;

int segActual = 0;
int totalLoop = 0;
int totalBackLoop = 0;
String horaActual = String(Hora) + ":" + String(Minuto) + ":" + String(Segundo);
String fechaActual = String(Anho) + "/" + String(Mes) + "/" + String(Dia);
tmElements_t tm;
int nextSeg = -1;
int countE = 0;

int valorHumedad = 1023;
String FC_humidity = "";
int mode = 0; // 0=manual | 1=slave

#define pinH_FC A0         // PIN como FC-28
#define pinH_FC2 A2         // PIN como FC-28

void setup() {
  pinMode(pinSYNC, OUTPUT); // PIN COMO LED SYNC
  pinMode(pinERROR, OUTPUT); // PIN LED ERROR
  pinMode(pinH_FC, INPUT);        // Al poner en INPUT activamos el FC-28
  pinMode(pinH_FC2, INPUT);        // Al poner en INPUT activamos el FC-28
  
  Wire.begin(8); // Unimos este dispositivo al bus I2C con dirección 8
  Wire.onReceive(receiveEvent); // Registramos el evento al recibir datos
  Serial.begin(9600); // Iniciamos el monitor serie para monitorear la comunicación
  digitalWrite(pinSYNC , LOW);
  digitalWrite(pinERROR , HIGH);
}
 
void loop() {
  digitalWrite(pinSYNC , HIGH);
  if(nextSeg == Segundo){
    countE = 0;
    digitalWrite(pinERROR , LOW);
    mode = 1;
  } else {
    countE = countE + 1;
    if(countE >= 2){
      Serial.println(" | Error de sincronizado");
      digitalWrite(pinERROR , HIGH);
      repeatLED(10, pinSYNC);
    }
    mode = 0;
  }

  Serial.print(" | pinH_FC: " + String(analogRead(pinH_FC))); 
  Serial.print(" | pinH_FC2: " + String(analogRead(pinH_FC2)));

  valorHumedad = map(analogRead(pinH_FC2), 0, 1023, 100, 0);
  if(valorHumedad <= 3){
    Serial.print(" | Error FC ");
    repeatLED(10, pinERROR);
  }
  FC_humidity = ("*S"+String(valorHumedad)+"*");
  
  Serial.print(" | Actual: " + String(valorHumedad));
  Serial.print(" | %: " + String(FC_humidity));
  
  if (mode == 0){
    Serial.print(" | Modo Automatico");
    
    String horaActual = String(Hora) + ":" + String(Minuto) + ":" + String(Segundo);
    String fechaActual = String(Anho) + "/" + String(Mes) + "/" + String(Dia);
    Serial.print(fechaActual + " " + horaActual + " - Temperatura: " + String(T) + " | Humedad: " + String(H) + " - Humedad MIN/MAX: " + String(minH) + "/" + String(maxH));

  } else if (mode == 1){
    Serial.print(" | Modo Manual");
  
    if(valorHumedad < minH){
       Serial.print("| Se necesita riego.");
    }
  }


  Serial.println();
  
  delay(1000);
  nextSeg = nextSeg - 1;
}

void repeatLED(int repeat, int pin){
  for (int i = 0; i <= repeat; i++){
    digitalWrite(pin, !digitalRead(pin));
    delay(100);
  }
}

void receiveEvent(int howMany) {
  int rDevice = 0;
  int rData = "";
  
  // Leemos el primero que será
  if (Wire.available() == 2){
    rDevice = Wire.read();
  }
  // Si hay un byte disponible
  if (Wire.available() == 1){
    rData = Wire.read();
  }

  if(rDevice == 1) { Anho = (rData); } 
  else if(rDevice == 2) { Mes = (rData); } 
  else if(rDevice == 3) { Dia = (rData); } 
  else if(rDevice == 4) { Hora = (rData); } 
  else if(rDevice == 5) { Minuto = (rData); } 
  else if(rDevice == 6) { Segundo = (rData); nextSeg = (Segundo <= 58) ? Segundo + 1 : 0; } 
  else if (rDevice == 7) { T = rData; } 
  else if (rDevice == 8) { H = rData; } 
  else if(rDevice == 9) { minH = rData; } 
  else if(rDevice == 0) { maxH = rData; } 
  else {
    Serial.print("Dispositivo: ");
    Serial.println(rDevice);
    Serial.print("Data: ");
    Serial.println(rData);
  }
  
}
