#include <SPI.h>

long durat;
int dist;
int inch;
int slavePin = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, INPUT);
  pinMode(slavePin, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
}

void loop() {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(9, LOW);
  delayMicroseconds(2);
  digitalWrite(9, HIGH);
  delayMicroseconds(10);
  digitalWrite(9, LOW);
  durat = pulseIn(8, HIGH);
  dist = durat*0.034/2;//calculate distance in cm
  inch = dist/2.5; //calculate in inches

  if(inch < 12)//will print inches and light up red LED
  {
    Serial.print(inch);
    Serial.println(" inches");
    digitalWrite(2, HIGH);
  }
  else //Red LED is LOW and distance in feet is shown
  {
    inch = inch/12;
    Serial.print(inch);
    Serial.println(" feet");
    digitalWrite(2, LOW);
  }
  
switch (inch) //Shows distance in binary on green LEDS
{
  case 1:
    digitalWrite(6, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, LOW);
    break;
  case 2:
    digitalWrite(6, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(3, LOW);
    break;
  case 3:
    digitalWrite(6, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(3, LOW);
    break;
  case 4:
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    break;
  case 5:
    digitalWrite(6, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    break;
  case 6:
    digitalWrite(6, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    break;
  case 7:
    digitalWrite(6, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    break;
  case 8:
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    break;
  case 9:
    digitalWrite(6, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    break;
  case 10:
    digitalWrite(6, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    break;
  case 11:
    digitalWrite(6, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    break;
  case 12:
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    break;
  case 13:
    digitalWrite(6, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    break;
  case 14:
    digitalWrite(6, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    break;
  case 15:
    digitalWrite(6, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    break;  
  default: //0
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, LOW);
}

  

}
