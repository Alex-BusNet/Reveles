#include <SPI.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
//i2c pins on A4 and A5
int number = 0;
int state = 0;

long durat;
int dist;
int inch;


void recieveData(int byteCount)
{
  while(Wire.available())
  {
    number = Wire.read();
    Serial.print("data recieved");
    Serial.println(number);

    if(number == 1)
    {
      if(state == 0)
      {
        digitalWrite(13, HIGH);
        state = 1;
      }
      else 
      {
        digitalWrite(13, LOW);
        state = 0;
      }
    }
  }
}

void sendData()
{
  Wire.write(number);
}


void setup() {
  pinMode(9, OUTPUT);
  pinMode(8, INPUT);
  pinMode(13, OUTPUT);//LED pin
  Serial.begin(9600);

  //initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  //define callbacks for i2c communication
  Wire.onReceive(recieveData);
  Wire.onRequest(sendData);

  Serial.println("Ready!");
}

void loop() {
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
  }
  else //Red LED is LOW and distance in feet is shown
  {
    inch = inch/12;
    Serial.print(inch);
    Serial.println(" feet");
  }
}




