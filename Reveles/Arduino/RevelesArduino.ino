/*
 * DC Motor Control Program
 * Motor Control written by Frank Hittel
 * I2C communication written by Alisha Johnson
*/

#include <Wire.h>
#include <SPI.h>
#include <Servo.h>

Servo turn1;
Servo turn2;
#define SLAVE_ADDRESS 0x04
#define enableA 10
#define input1 11
#define input2 12
#define frontTrigPin 2 // trigPIN1
#define frontEchoPin 3 // echoPIN1
#define rearTrigPin 6  // trigPIN2
#define rearEchoPin 7  // echoPIN2
#define LED 13
// I2C drive commands
#define FORWARD_STATE 1
#define STOP_STATE 0
#define REVERSE_STATE -1

// Other defines
#define INT_MAX 2147483647

//-----------------
// Variables 
long duration = 0L; // Time for response to arrive from (temp)
int distance = 0;   // Calculated value from US (temp)
int inch = 0;       // Calculated value from US (temp)
int pwmOUT = 0;     // Motor Drive speed signal
int tofDistance = INT_MAX;
int rD = 0;
int destination = 0;

// I2C variables
String command = "";      // Info recieved from RPi
String cmdBreakdown[4];
String gpsResponse = "0.00000,0.00000";  // Should contain two floats, comma separated, no spaces
int number = 0;
int state = STOP_STATE;

bool fwd = true;          // Direction switch (temp)
bool on = false;          // Are the motors on.
bool initialized = false; // Sanity check
//-----------------

//=================================================================
//             I2C Command Structure (RPi -> Arduino): 
//         (Function):(US value):(Direction):(ToF value):
// ----------------------------------------------------------------
// Possible values
//    Function:
//      (M)otor - RPi is sending drive instructions.
//      (G)PS - RPi is requesting GPS coordinates.
//    US Value: Floating Point value for PWM signal.
//      (Motor command only) 
//      Value is in inches.
//      Value is not adjusted to fit between 255 and 0.
//    Direction: (Motor command only)
//      (F)orward.
//      (S)top.
//      (R)everse.
//    ToF value: Floating point value from Time of Flight sensors. 
//      (Motor command only)
//      E-Stop may still need to be determined.
//=================================================================

void recieveData(int byteCount)
{
    while(0 < Wire.available())
    {
        // Concatenate each byte to the string.
        command += Wire.read(); 
        //number = Wire.read();

    }
    
    Serial.print("data recieved");
    Serial.print(command);

    int lastIdx = 0; // tracks where we found the previous instance of ':' in the command string.
    for (int i = 0; i < 4; i++)
    {
      cmdBreakdown[i] = command.substring(lastIdx, command.indexOf(':', lastIdx));
      lastIdx = command.indexOf(':', lastIdx);

      // Since the GPS command won't have anything beyond the (Function) parameter, 
      // we need to break out of the loop so we don't read NULL characters from
      // the command string.
      if(lastIdx == command.lastIndexOf(':')) { break; }
    }
    
    // Check to make sure the first value is a letter.
    // toInt() returns a zero (0) on an invalid conversion
    if(cmdBreakdown[0].toInt() == 0)
    {
          if(cmdBreakdown[0].equals("M"))
          {
            //--------------------
            // Update motor drive
            //--------------------
            
            // Get Ultrasonic distance
            inch = cmdBreakdown[1].toInt();

            // Get the direction to drive
            if(cmdBreakdown[2].equals("F"))
              state = FORWARD_STATE;
            else if(cmdBreakdown[2].equals("S"))
              state = STOP_STATE;
            else if(cmdBreakdown[2].equals("R"))
              state = REVERSE_STATE;

            // Get the ToF distance
            tofDistance = cmdBreakdown[3].toInt();
          }
          else if (cmdBreakdown[0].equals("G"))
          {
            // Get GPS Coordinates
            // Set a flag indicating the RPi is waiting for GPS data?
          }
    }
    
//    Serial.println(number);
//    if(number == 1)
//    {
//        if(state == 0)
//        {
//            digitalWrite(13, HIGH);
//            state = 1;
//        }
//        else
//        {
//            digitalWrite(13, LOW);
//            state = 0;
//        }
//    }
}

void sendData()
{
    Wire.write(number);
}

/*This function is designed for use in the initial boot-up of the robot. The motors will be turned on if not already and begin moving forward,
FUTURE ITERATIONS SHOULD INVOLVE A DECISION TO CHANGE ITS STARTING DIRECTION!*/
void start()
{
  if(!on)
  {
    motorON();
  }
  
  if(state == FORWARD_STATE)
  {
    forward(); 
  }
  else if(state == REVERSE_STATE)
  {
    backward();
  }
}

/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors forward and begins incrementing*/
void forward()
{
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
  driveFORWARD();
}

/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors backward and begins incrementing*/
void backward()
{
   digitalWrite(input1, LOW);
   digitalWrite(input2, HIGH);
   driveBACKWARD();
}

/*This function is designed to stop movement while still ready to move. The input pins will be turned high to prevent current flow,
resulting in the robot's inability to move but can begin once a proper current path is made. */
void motorSTOP()
{
  digitalWrite(input1, HIGH);
  digitalWrite(input2, HIGH);
}

/*This function is designed for use in the initial startup of the robot. When the robot is booted up, the enable pin on the motor
drivers will be turned high so that the robot may begin moving.*/
void motorON()
{
  digitalWrite(enableA, HIGH);
  on = true;
  delay(20);
}

/*This function is designed as the Emergency Stop for the robot. When the time-of-flight sensor is flagged, the robot will shut off.*/
void motorOFF()
{
  digitalWrite(enableA, LOW);
  on = false;
}

/*This function is designed to accelerate and deccelerate the motors in increments\
while moving forward. There are two individual incremental settings based on the signals
received by the PIR sensor and the time-of-flight sensors. If neither sensors are flagged 
(e.g. only sees a wall), then the function will go through the slower progression. If either
sensor is flagged, the function will go into the the steeper section. If the time-of-flight sensor
is flagged, then the system will go into an emergency stop function. */
void driveFORWARD()
{
  //----------------------------
  // US temp code
  digitalWrite(LED, HIGH);
  digitalWrite(frontTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(frontTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(frontTrigPin, LOW);
  duration = pulseIn(frontEchoPin, HIGH);
  distance = duration*0.034/2;//calculate distance in cm
  inch = distance/2.5; //calculate in inches 
  //----------------------------
  // Final code starts here
  pwmOUT = (inch * 2)-1;
  if(pwmOUT < 48)
  {
    state = REVERSE_STATE;
    motorSTOP();
  }
  else if (pwmOUT > 255)
  {
    pwmOUT = 255;
  }
   
  analogWrite(enableA,pwmOUT);
//  turn1.write (90);
//  turn2.write (90);
//  for(int pos = 90; pos = 180; pos++)
//  {
//    turn1.write(pos);
//    turn2.write(pos);
//   }
//  for(int pos = 180; pos = 90; pos++)
//  {
//    turn1.write(pos);
//    turn2.write(pos);
//   }
}

/*This function is designed to accelerate and deccelerate the motors in increments\
while moving backward. There are two individual incremental settings based on the signals
received by the PIR sensor and the time-of-flight sensors. If neither sensors are flagged 
(e.g. only sees a wall), then the function will go through the slower progression. If either
sensor is flagged, the function will go into the the steeper section. If the time-of-flight sensor
is flagged, then the system will go into an emergency stop function. */
void driveBACKWARD()
{
  //----------------------------
  // US temp code
  digitalWrite(LED, LOW);
  digitalWrite(rearTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(rearTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(rearTrigPin, LOW);
  duration = pulseIn(rearEchoPin, HIGH);
  distance = duration*0.034/2;//calculate distance in cm
  inch = distance/2.5; //calculate in inches
  //----------------------------
  // Final code starts here
  pwmOUT = (inch * 2)-1;
  
  if(pwmOUT < 48)
  {
    state = FORWARD_STATE;
    motorSTOP();
  }
  else if (pwmOUT > 255)
  {
    pwmOUT = 255;
  }
  
 analogWrite(enableA,pwmOUT);
// turn1.write (90);
// turn2.write (90);
// for(int pos = 90; pos = 0; pos--)
// {
//    turn1.write(pos);
//    turn2.write(pos);
//  }
//
// for(int pos = 0; pos = 90; pos++)
// {
//    turn1.write(pos);
//    turn2.write(pos);
//  }
}

void setup() {
  //set up pins and digital inputs
  Serial.begin(9600);
  turn1.attach(4);
  turn2.attach(5);
  pinMode(enableA, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(frontTrigPin, OUTPUT);
  pinMode(rearTrigPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(rearEchoPin, INPUT);
  
  // Used for initial motor startup
  digitalWrite(enableA, LOW);
  digitalWrite(input1, LOW);
  digitalWrite(input2, LOW);

  //initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  //define callbacks for i2c communication
  Wire.onReceive(recieveData);
  Wire.onRequest(sendData);

  // Setup motor drive flags.
  on = false;
  state = FORWARD_STATE;
  initialized = true;
}

void loop()
{ 
  if(initialized == true)
  {
    start();
  }
}
