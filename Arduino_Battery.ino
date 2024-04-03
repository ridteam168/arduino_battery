#define UP 0
#define DOWN 1

// General
int i,j;
int ledState1 = 0;
int interval = 500;
bool lampState = true;
bool stateCondition = true;
int ledState2 = 0, ledState3 = 0;
int ledState4 = 0, ledState  = 0;
unsigned long millisKondisi = 0;
unsigned long nolPersenMillis = 0;
unsigned long satuPersenMillis = 0;
unsigned long seratusPersenMillis = 0;
unsigned long millisSeratusBerjalan;
unsigned long millisBerjalan;

unsigned long seratusPersenMillis1 = 0;
unsigned long millisSeratusBerjalan1;
unsigned long millisBaterai = 0;
unsigned long millisBerjalan1;

// Pin On | Off | Charging State
int batteryPin = A1, batteryRead = 0;
int powerPin = 13, powerRead = 0;

// Pin Level Baterai
int nolPersen       = 7;
int sepuluhPersen   = 12;
int duaLimaPersen   = 2;
int limaPuluhPersen = 3;
int tujuhLimaPersen = 4;
int seratusPersen   = 11;

int statePertama = A2; // 0%  - 29%
int stateKedua   = A3; // 30% - 59%
int stateKetiga  = A4; // 60% - 100%

// Pin PWM Kamera
int pinPWM = 9;

// Pin Data Baterai
int sensorValue;
int teganganSementara;
int dataTeganganAsli;
int sensorPin = A0;
int pinCharging = 8;
int stateCharging = 1;
unsigned long millisPenuh = 0;
unsigned long previousMillis = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillisData = 0;

// Pin Ambeient
const byte redAmbient = 6;
const byte greenAmbient = 5;
const byte whiteAmbient = 10;

// PWM Ambient LED
int fadeValue = 0;
int fadeInterval = 50;
const int minPWM = 0;
const int maxPWM = 255;
byte fadeIncrement = 5;
byte fadeDirection = UP;
unsigned long previousFadeMillis;

// Deklarasi Parsing Serial
int in[] = {0,0};
int count ,flagSerial;
char last,incomingByte,buff[70];
char indeks1[20],indeks2[20],indeks3[20],indeks4[20],indeks5[20];

// Voltage Filter
long total = 0, total1 = 0, total2 = 0, total3 = 0;
int readIndex = 0, readIndex1 = 0, readIndex2 = 0, readIndex3 = 0;
const int numReadings = 25, numReadings1 = 25, numReadings2 = 25, numReadings3 = 25;
int readings[numReadings], readings1[numReadings1], readings2[numReadings2], readings3[numReadings3];

void setup() {
  Serial.begin(9600);
  pinMode(pinPWM, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(redAmbient, OUTPUT);
  pinMode(whiteAmbient, OUTPUT);
  pinMode(greenAmbient, OUTPUT);
  pinMode(nolPersen, OUTPUT);
  pinMode(sepuluhPersen, OUTPUT);
  pinMode(duaLimaPersen, OUTPUT);
  pinMode(limaPuluhPersen, OUTPUT);
  pinMode(tujuhLimaPersen, OUTPUT);
  pinMode(seratusPersen, OUTPUT);
  pinMode(statePertama, OUTPUT);
  pinMode(stateKedua, OUTPUT);
  pinMode(stateKetiga, OUTPUT);
  pinMode(powerPin, INPUT_PULLUP);
  pinMode(pinCharging, INPUT_PULLUP);
  digitalWrite(powerPin, LOW);
  digitalWrite(batteryPin, LOW);
  analogWrite(greenAmbient, fadeValue); 
  analogWrite(redAmbient, fadeValue);
  analogWrite(whiteAmbient, fadeValue);
}

void loop() {
  // Pembacaan Millis General
  millisSeratusBerjalan = millis();

  // Fungsi General
  readCurrentState();
  bacaDataBaterai();
  
  // Jika Power Nyala dan Sedang Charge
  if(powerRead == 1 && batteryRead == 1){
    kondisiCharge();
    ambientNyala();
    analogWrite(pinPWM, in[1]);
  }
  
  // Jika Power Nyala dan Tidak Sedang Charge
  else if(powerRead == 1 && batteryRead == 0){
    onlyPower();
    ambientNyala();
    analogWrite(pinPWM, in[1]);
  }
  
  // Jika Power Mati dan Tidak Sedang Charge
  else if(powerRead == 0 && batteryRead == 0){
    batteryMati();
    ambientMati();
    analogWrite(pinPWM, 0);
  }
  
  // Jika Power Mati dan Sedang di Charge
  else if(powerRead == 0 && batteryRead == 1){
    kondisiCharge();
    ambientMati();
    analogWrite(pinPWM, 0);
  }
  serialKomunikasi();
}

void readCurrentState(){
  powerRead   = digitalRead(powerPin);
  batteryRead = digitalRead(batteryPin); 
}

void bacaDataBaterai(){
  stateCharging = digitalRead(pinCharging);
  unsigned long currentMillis1 = millis();
  unsigned long currentMillis5 = millis();
  if(stateCharging == 1){
    digitalWrite(statePertama, LOW);
    digitalWrite(stateKedua, LOW);
    digitalWrite(stateKetiga, LOW);
    delay(1000); // Awal 1 Detik
    if(stateCondition){
      dataTeganganAsli = smooth();
      if(dataTeganganAsli >= 977 && dataTeganganAsli <= 992){
        sensorValue = 100;
      }
      else if(dataTeganganAsli >= 975 && dataTeganganAsli <= 976){
        sensorValue = 99;
      }
      else{
        sensorValue = analogRead(sensorPin);
        sensorValue = map(sensorValue, 868, 974, 0, 98);
        if(sensorValue <= 0){sensorValue = 0;}
        else if(sensorValue >= 100){sensorValue = 100;}
      }
      stateCondition = false;
    }
    else{
      if((currentMillis5 - millisBaterai) >= 10000){
        dataTeganganAsli = smooth();
        if(dataTeganganAsli >= 977 && dataTeganganAsli <= 992){
          sensorValue = 100;
        }
        else if(dataTeganganAsli >= 975 && dataTeganganAsli <= 976){
          sensorValue = 99;
        }
        else{
          sensorValue = analogRead(sensorPin);
          sensorValue = map(sensorValue, 868, 974, 0, 98);
          if(sensorValue <= 0){sensorValue = 0;}
          else if(sensorValue >= 100){sensorValue = 100;}
        }
        millisBaterai = currentMillis5;
      }
    }
    if((currentMillis1 - previousMillis) >= 550){
      dataTeganganAsli = analogRead(sensorPin);
      stateCharging = digitalRead(pinCharging);
      Serial.print(sensorValue); Serial.print(",");
      Serial.println(stateCharging);
      previousMillis = currentMillis1;
    }
  }
  else if(stateCharging == 0){
    stateCondition = true;
    unsigned long currentMillis3 = millis();
    if (lampState){
      if (currentMillis3 - previousMillis3 >= 1800){
        lampState = false;
        dataTeganganAsli = analogRead(sensorPin);
        if(dataTeganganAsli >= 977 && dataTeganganAsli <= 991){
          sensorValue = 99;
        }
        else if(dataTeganganAsli >= 992){
          sensorValue = 100;
        }
        else{
          sensorValue = analogRead(sensorPin);
          sensorValue = map(sensorValue, 868, 976, 0, 98);
          sensorValue = sensorValue - 15;
          if(sensorValue < teganganSementara){
            sensorValue = teganganSementara;
          }
          else{
            sensorValue = sensorValue;
          }
        }
        
        if(sensorValue <= 0){sensorValue = 0;}
        else if(sensorValue >= 100){sensorValue = 100;}
        
        millisBerjalan1 = millis();
        if(sensorValue >= 0 && sensorValue <= 55){
          if(sensorValue >= 35 && sensorValue <= 55){
            digitalWrite(statePertama, HIGH);
            digitalWrite(stateKedua, HIGH);
            digitalWrite(stateKetiga, LOW);
          }
          else{
            digitalWrite(statePertama, HIGH);
            digitalWrite(stateKedua, LOW);
            digitalWrite(stateKetiga, LOW);
          }
          seratusPersenMillis1 = millisBerjalan1;
        }
        else if(sensorValue >= 56 && sensorValue <= 75){
          if(sensorValue >= 65 && sensorValue <= 75){
            digitalWrite(statePertama, LOW);
            digitalWrite(stateKedua, HIGH);
            digitalWrite(stateKetiga, HIGH);
          }
          else{
            digitalWrite(statePertama, LOW);
            digitalWrite(stateKedua, HIGH);
            digitalWrite(stateKetiga, LOW);
          }
          seratusPersenMillis1 = millisBerjalan1;
        }
        else if(sensorValue >= 76 && sensorValue <= 99){
          digitalWrite(statePertama, LOW);
          digitalWrite(stateKedua, LOW);
          digitalWrite(stateKetiga, HIGH);
          seratusPersenMillis1 = millisBerjalan1;
        }
        else if(sensorValue == 100){
          if(dataTeganganAsli >= 1000){
            digitalWrite(statePertama, LOW);
            digitalWrite(stateKedua, LOW);
            digitalWrite(stateKetiga, LOW);
          }
          else{
            digitalWrite(statePertama, LOW);
            digitalWrite(stateKedua, LOW);
            digitalWrite(stateKetiga, HIGH);
          }
        }
      }
    } else {
      if (currentMillis3 - previousMillis3 >= 10000) { // Awal = 30 detik
        digitalWrite(statePertama, LOW);
        digitalWrite(stateKedua, LOW);
        digitalWrite(stateKetiga, LOW);
        lampState = true;
        teganganSementara = sensorValue;
        previousMillis3 = currentMillis3;
      }
    }

    unsigned long currentMillisData = millis();
    if (currentMillisData - previousMillisData >= 550) {
      stateCharging = digitalRead(pinCharging);
      Serial.print(sensorValue); Serial.print(",");
      Serial.println(stateCharging);
      previousMillisData = currentMillisData;
    }
  }
  else{
    digitalWrite(statePertama, LOW);
    digitalWrite(stateKedua, LOW);
    digitalWrite(stateKetiga, LOW);
  }
}

void kondisiCharge(){
  if(sensorValue >= 0 && sensorValue <= 25){lampuSatu();}
  else if(sensorValue >= 26 && sensorValue <= 50){lampuDua();}
  else if(sensorValue >= 51 && sensorValue <= 75){lampuTiga();}
  else if(sensorValue >= 76 && sensorValue <= 99){lampuEmpat();}
  else if(sensorValue == 100){lampuFull();}
}

void onlyPower(){
  millisBerjalan = millis();
  if(sensorValue == 0){
    digitalWrite(duaLimaPersen, LOW);
    digitalWrite(limaPuluhPersen, LOW);
    digitalWrite(tujuhLimaPersen, LOW);
    digitalWrite(seratusPersen, LOW);
    if((millisBerjalan - millisKondisi) >= 30000){
      digitalWrite(nolPersen, HIGH);
      digitalWrite(sepuluhPersen, LOW);
    }
    else{
      unsigned long millisNolPersen = millis();
      if(millisNolPersen - nolPersenMillis >= 200){
        nolPersenMillis = millisNolPersen;
        if((millisBerjalan - millisKondisi) <= 60000){
          if(ledState == LOW){ledState = HIGH;} 
          else{ledState = LOW;} 
          }else{ledState = LOW;}
          digitalWrite(sepuluhPersen, ledState);
        }
      }
    }
    else if(sensorValue >= 1 && sensorValue <= 25){
      if(sensorValue >= 1 && sensorValue <= 10){
        digitalWrite(nolPersen, LOW);
        digitalWrite(duaLimaPersen, LOW);
        digitalWrite(limaPuluhPersen, LOW);
        digitalWrite(tujuhLimaPersen, LOW);
        digitalWrite(seratusPersen, LOW);
        unsigned long millisSatuPersen = millis();
        if(millisSatuPersen - satuPersenMillis >= interval) {
          satuPersenMillis = millisSatuPersen;
          if (ledState == LOW){ledState = HIGH;} 
          else{ledState = LOW;}
          digitalWrite(sepuluhPersen, ledState);
        }
      }
      else{
        digitalWrite(nolPersen, LOW);
        digitalWrite(sepuluhPersen, LOW);
        digitalWrite(duaLimaPersen, HIGH);
        digitalWrite(limaPuluhPersen, LOW);
        digitalWrite(tujuhLimaPersen, LOW);
        digitalWrite(seratusPersen, LOW);
      }
      millisKondisi = millisBerjalan;
    }
    else if(sensorValue >= 26 && sensorValue <= 50){
      digitalWrite(nolPersen, LOW);
      digitalWrite(duaLimaPersen, HIGH);
      digitalWrite(limaPuluhPersen, HIGH);
      digitalWrite(tujuhLimaPersen, LOW);
      digitalWrite(seratusPersen, LOW);
      millisKondisi = millisBerjalan;
    }
    else if(sensorValue >= 51 && sensorValue <= 75){
      digitalWrite(nolPersen, LOW);
      digitalWrite(duaLimaPersen, HIGH);
      digitalWrite(limaPuluhPersen, HIGH);
      digitalWrite(tujuhLimaPersen, HIGH);
      digitalWrite(seratusPersen, LOW);
      millisKondisi = millisBerjalan;
    }
    else if(sensorValue >= 76 && sensorValue <= 99){
      digitalWrite(nolPersen, LOW);
      digitalWrite(duaLimaPersen, HIGH);
      digitalWrite(limaPuluhPersen, HIGH);
      digitalWrite(tujuhLimaPersen, HIGH);
      digitalWrite(seratusPersen, HIGH);
      millisKondisi = millisBerjalan;
    }
    else if(sensorValue == 100){
      digitalWrite(nolPersen, LOW);
      digitalWrite(sepuluhPersen, LOW);
      digitalWrite(duaLimaPersen, HIGH);
      digitalWrite(limaPuluhPersen, HIGH);
      digitalWrite(tujuhLimaPersen, HIGH);
      digitalWrite(seratusPersen, HIGH);
   }
}

void lampuSatu(){
  unsigned long millisBerjalan = millis();
  if((millisBerjalan - millisKondisi) >= 300){
    if(ledState1 == LOW){ledState1 = HIGH;}
    else{ledState1 = LOW;}
    millisKondisi = millisBerjalan;
  }
  digitalWrite(nolPersen, LOW);
  digitalWrite(duaLimaPersen, ledState1);
  digitalWrite(sepuluhPersen, LOW);
  digitalWrite(limaPuluhPersen, LOW);
  digitalWrite(tujuhLimaPersen, LOW);
  digitalWrite(seratusPersen, LOW);
  seratusPersenMillis = millisSeratusBerjalan;
}

void lampuDua(){
  unsigned long millisBerjalan = millis();
  if((millisBerjalan - millisKondisi) >= 300){
    if(ledState1 == LOW){ledState1 = HIGH;}
    else{ledState1 = LOW;}
    
    if((millisBerjalan - millisKondisi) >= 600){
      if(ledState2 == LOW){ledState2 = HIGH;}
      else{ledState2 = LOW;}
      if((millisBerjalan - millisKondisi) >= 900){
        ledState1 = LOW;
        ledState2 = LOW;
        millisKondisi = millisBerjalan;
      } 
    }
  }
  digitalWrite(nolPersen, LOW);
  digitalWrite(sepuluhPersen, LOW);
  digitalWrite(duaLimaPersen, ledState1);
  digitalWrite(limaPuluhPersen, ledState2);
  digitalWrite(tujuhLimaPersen, LOW);
  digitalWrite(seratusPersen, LOW);
  seratusPersenMillis = millisSeratusBerjalan;
}

void lampuTiga(){
  unsigned long millisBerjalan = millis();
  if((millisBerjalan - millisKondisi) >= 300){
    if(ledState1 == LOW){ledState1 = HIGH;}
    else{ledState1 = LOW;}
    if((millisBerjalan - millisKondisi) >= 600){
      if(ledState2 == LOW){ledState2 = HIGH;}
      else{ledState2 = LOW;}
      if((millisBerjalan - millisKondisi) >= 900){
        if(ledState3 == LOW){ledState3 = HIGH;}
        else{ledState3 = LOW;}
        if((millisBerjalan - millisKondisi) >= 1200){
          ledState1 = LOW;
          ledState2 = LOW;
          ledState3 = LOW;
          millisKondisi = millisBerjalan;
        }
      }
    }
  }
  digitalWrite(nolPersen, LOW);
  digitalWrite(sepuluhPersen, LOW);
  digitalWrite(duaLimaPersen, ledState1);
  digitalWrite(limaPuluhPersen, ledState2);
  digitalWrite(tujuhLimaPersen, ledState3);
  digitalWrite(seratusPersen, LOW);
  seratusPersenMillis = millisSeratusBerjalan;
}

void lampuEmpat(){
  unsigned long millisBerjalan = millis();
  if((millisBerjalan - millisKondisi) >= 300){
    if(ledState1 == LOW){ledState1 = HIGH;}
    else{ledState1 = LOW;}
    if((millisBerjalan - millisKondisi) >= 600){
      if(ledState2 == LOW){ledState2 = HIGH;}
      else{ledState2 = LOW;}
      if((millisBerjalan - millisKondisi) >= 900){
        if(ledState3 == LOW){ledState3 = HIGH;}
        else{ledState3 = LOW;}
        if((millisBerjalan - millisKondisi) >= 1200){
          if(ledState4 == LOW){ledState4 = HIGH;}
          else{ledState4 = LOW;}
          if((millisBerjalan - millisKondisi) >= 1500){
            ledState1 = LOW;
            ledState2 = LOW;
            ledState3 = LOW;
            ledState4 = LOW;
            millisKondisi = millisBerjalan;
          } 
        }
      }
    }
  }
  digitalWrite(nolPersen, LOW);
  digitalWrite(sepuluhPersen, LOW);
  digitalWrite(duaLimaPersen, ledState1);
  digitalWrite(limaPuluhPersen, ledState2);
  digitalWrite(tujuhLimaPersen, ledState3);
  digitalWrite(seratusPersen, ledState4);
  seratusPersenMillis = millisSeratusBerjalan;
}

void lampuFull(){
  millisBerjalan = millis();
  if((millisSeratusBerjalan - seratusPersenMillis) >= 60000){
    ledState1 = HIGH; ledState2 = HIGH;
    ledState3 = HIGH; ledState4 = HIGH;
    digitalWrite(statePertama, LOW);
    digitalWrite(stateKedua, LOW);
    digitalWrite(stateKetiga, LOW);
  }
  else{
    if((millisBerjalan - millisKondisi) >= 300){
      if(ledState1 == LOW){ledState1 = HIGH;}
      else{ledState1 = LOW;}
      if((millisBerjalan - millisKondisi) >= 600){
        if(ledState2 == LOW){ledState2 = HIGH;}
        else{ledState2 = LOW;}
        if((millisBerjalan - millisKondisi) >= 900){
          if(ledState3 == LOW){ledState3 = HIGH;}
          else{ledState3 = LOW;}
          if((millisBerjalan - millisKondisi) >= 1200){
            if(ledState4 == LOW){ledState4 = HIGH;}
            else{ledState4 = LOW;}
            if((millisBerjalan - millisKondisi) >= 1500){
              ledState1 = LOW;
              ledState2 = LOW;
              ledState3 = LOW;
              ledState4 = LOW;
              millisKondisi = millisBerjalan;
            } 
          }
        }
      }
    }
  }
  digitalWrite(nolPersen, LOW);
  digitalWrite(sepuluhPersen, LOW);
  digitalWrite(duaLimaPersen, ledState1);
  digitalWrite(limaPuluhPersen, ledState2);
  digitalWrite(tujuhLimaPersen, ledState3);
  digitalWrite(seratusPersen, ledState4);
}

void batteryMati(){
  digitalWrite(nolPersen, LOW);
  digitalWrite(sepuluhPersen, LOW);
  digitalWrite(duaLimaPersen, LOW);
  digitalWrite(limaPuluhPersen, LOW);
  digitalWrite(tujuhLimaPersen, LOW);
  digitalWrite(seratusPersen, LOW);
}

void ambientNyala(){
  unsigned long currentMillis = millis();
  if(in[0] == 1){
    analogWrite(redAmbient, 0);
    analogWrite(whiteAmbient, 255);
    digitalWrite(greenAmbient, HIGH);
  }else{
    magicWhiteFade(currentMillis);
    digitalWrite(greenAmbient, LOW);
  }
}

void ambientMati(){
  analogWrite(pinPWM, 0);
  analogWrite(redAmbient, 0);
  analogWrite(whiteAmbient, 0);
  digitalWrite(greenAmbient, LOW);
}

void magicWhiteFade(unsigned long thisMillis){
  if (thisMillis - previousFadeMillis >= fadeInterval){
    if(fadeDirection == UP){ fadeValue = fadeValue + fadeIncrement;
      if(fadeValue >= maxPWM){fadeValue = maxPWM; fadeDirection = DOWN;}
    }
    else{fadeValue = fadeValue - fadeIncrement;
      if(fadeValue <= minPWM){fadeValue = minPWM; fadeDirection = UP;}
    }
    analogWrite(redAmbient, fadeValue); analogWrite(whiteAmbient, fadeValue);
    previousFadeMillis = thisMillis;
  }
}

void serialKomunikasi(){
  if(Serial.available()>0){
    incomingByte = Serial.read();
    if(incomingByte=='#'){count=0;flagSerial=1;}
    else{
      count++;
      buff[count] = incomingByte;last=1;
      if(incomingByte=='!' && flagSerial == 1){
        flagSerial = 0;
        for (i=1; i<=2; i++){
          for (j=0; j<10; j++){
            if (buff[j+last]==',' || buff[j+last]=='!'||buff[j+last]=='\0'){
              last = last + j + 1;
              break;
            }
            switch (i){
              case 1: indeks1[j] = buff[j+last];   case 2: indeks2[j] = buff[j+last];
            }
          }
          switch (i){
            case 1: indeks1[j] = '\0';   case 2: indeks2[j] = '\0';
          }
        }
      }
      in[0]  = atoi(indeks1);  in[1]  = atoi(indeks2);
    }
  }
}

long smooth(){
  long average;
  total = total - readings[readIndex]; readings[readIndex] = analogRead(sensorPin);
  total = total + readings[readIndex]; readIndex = readIndex + 1;
  if(readIndex >= numReadings){readIndex = 0;}
  average = total / numReadings; return average;
}
