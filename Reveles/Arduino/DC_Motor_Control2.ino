/*DC Motor Tech Demo Program*/

#include <Wire.h>
#include <SPI.h>

#define SLAVE_ADDRESS 0x04
#define enableA 10
#define input1 11
#define input2 12
#define trigPIN1 1
#define echoPIN1 2
#define trigPIN2 6
#define echoPIN2 7
#define LED 13

long duration;
int distance;
int inch;
int pwmOUT;
int rD = 0;
int destination;
int number = 0;
int state = 0;
bool fwd;

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

/*This function is designed for use in the initial boot-up of the robot. The motors will be turned on if not already and begin moving forward,
FUTURE ITERATIONS SHOULD INVOLVE A DECISION TO CHANGE ITS STARTING DIRECTION!*/
void start()
{
  if(fwd)
  {
    motorON();
  }
  forward();
}
/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors forward and begins incrementing*/
void forward()
{
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
  delayMicroseconds(1000);
  driveFORWARD();
  }
/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors backward and begins incrementing*/
void backward()
{
   digitalWrite(input1, LOW);
   digitalWrite(input2, HIGH);
   delay(20);
   driveBACKWARD();
  }
/*This function is designed to stop movement while still ready to move. The input pins will be turned high to prevent current flow,
resulting in the robot's inability to move but can begin once a proper current path is made. */
void motorSTOP()
{
  digitalWrite(input1, HIGH);
  digitalWrite(input2, HIGH);
  delay(20);
  }
/*This function is designed for use in the initial startup of the robot. When the robot is booted up, the enable pin on the motor
drivers will be turned high so that the robot may begin moving.*/
void motorON()
{
  digitalWrite(enableA, HIGH);
  fwd = true;
  delay(20);
  }
/*This function is designed as the Emergency Stop for the robot. When the time-of-flight sensor is flagged, the robot will shut off.*/
void motorOFF()
{
  digitalWrite(enableA, LOW);
  fwd = false;
  }
/*This function is designed to accelerate and deccelerate the motors in increments\
while moving forward. There are two individual incremental settings based on the signals
received by the PIR sensor and the time-of-flight sensors. If neither sensors are flagged 
(e.g. only sees a wall), then the function will go through the slower progression. If either
sensor is flagged, the function will go into the the steeper section. If the time-of-flight sensor
is flagged, then the system will go into an emergency stop function. */
void driveFORWARD()
{
  digitalWrite(LED, HIGH);
  digitalWrite(trigPIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPIN1, LOW);
  duration = pulseIn(echoPIN1, HIGH);
  distance = duration*0.034/2;//calculate distance in cm
  inch = distance/2.5; //calculate in inches 
  pwmOUT = (inch * 2)-1;
  if(pwmOUT < 48)
  {
    motorSTOP();
    backward();
   }
  else if (pwmOUT > 255)
  {
    pwmOUT = 255;
   }
  analogWrite(enableA,pwmOUT);
 }
/*This function is designed to accelerate and deccelerate the motors in increments\
while moving backward. There are two individual incremental settings based on the signals
received by the PIR sensor and the time-of-flight sensors. If neither sensors are flagged 
(e.g. only sees a wall), then the function will go through the slower progression. If either
sensor is flagged, the function will go into the the steeper section. If the time-of-flight sensor
is flagged, then the system will go into an emergency stop function. */
void driveBACKWARD()
{
  digitalWrite(LED, LOW);
  digitalWrite(trigPIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPIN2, LOW);
  duration = pulseIn(echoPIN2, HIGH);
  distance = duration*0.034/2;//calculate distance in cm
  inch = distance/2.5; //calculate in inches
  pwmOUT = (inch * 2)-1;
  if(pwmOUT < 48)
  {
    motorSTOP();
    backward();
   }
  else if (pwmOUT > 255)
  {
    pwmOUT = 255;
   }
  analogWrite(enableA,pwmOUT);
  }

void setup() {
  //set up pins and digital inputs
  Serial.begin(9600);
  pinMode(enableA, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(trigPIN1, OUTPUT);
  pinMode(trigPIN2, OUTPUT);
  pinMode(echoPIN1, INPUT);
  pinMode(echoPIN2, INPUT);
  
  // Used for initial motor startup
  digitalWrite(enableA, LOW);
  digitalWrite(input1, LOW);
  digitalWrite(input2, LOW);

  //initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  //define callbacks for i2c communication
  Wire.onReceive(recieveData);
  Wire.onRequest(sendData);
  fwd = true;
}

void loop()
{ 
    start();
}


