/*Integrated Tech Demo Program*/

//#ifndef USE_CLION
#include <SPI.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
#define enableA 10
#define input1 11
#define input2 12
#define b1 1

//i2c pins on A4 and A5
int number = 0;
int state = 0;

long durat;
int dist;
int inch;

int rD = 0;
int pressed = false;

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


void setup()
{
    //DC Motors
    //set up pins and digital inputs
    pinMode(enableA, OUTPUT);
    pinMode(input1, OUTPUT);
    pinMode(input2, OUTPUT);
    pinMode(b1, INPUT);

    // Used for initial startup
    digitalWrite(input1, LOW);
    digitalWrite(input2, HIGH);

    //I2C
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

void loop()
{
    //DC Motor
    //Motor Speed Control with Potentiometer
    int potValue = analogRead(A0); // Read potentiometer value
    int pwmOutput = map(potValue, 0, 1023, 0 , 255); // Map the potentiometer value from 0 to 255
    analogWrite(enableA, pwmOutput); // Send PWM signal to L298N Enable pin

    // Read button - Debounce
    if (digitalRead(b1) == true)
    {
        pressed = !pressed;
    }
    while (digitalRead(b1) == true);
    delay(20);

    //Motor Direction Control with Button
    // If button is pressed - change rotation direction
    if (pressed == true  & rD == 0)
    {
        digitalWrite(input1, HIGH);
        digitalWrite(input2, LOW);
        rD = 1;
        delay(20);
    }
    // If button is pressed - change rotation direction
    if (pressed == false & rD == 1)
    {
        digitalWrite(input1, LOW);
        digitalWrite(input2, HIGH);
        rD = 0;
        delay(20);
    }

    //I2C
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
