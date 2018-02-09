/*
 * DC Motor Control Program
 * Motor Control written by Frank Hittel
 * I2C communication written by Alisha Johnson
*/

#include <Wire.h>
#include <SPI.h>

#define SLAVE_ADDRESS 0x04
#define enableA 10
#define input1 11
#define input2 12
#define frontTrigPin 2 // trigPIN1
#define frontEchoPin 3 // echoPIN1
#define rearTrigPin 6  // trigPIN2
#define rearEchoPin 7  // echoPIN2
#define LED 13

// I2C drive states
#define FOWARD_STATE 1
#define STOP_STATE  0
#define REVERSE_STATE  -1

// I2C Comm commands
#define START      0x5354515354,
#define END        0x454E44,
#define M_FWD      0x46,
#define CMD_G      0x47,
#define CMD_M      0x4D,
#define M_REV      0x52,
#define M_STOP     0x53,
#define LATITUDE   0x4C41544954554445,
#define LONGITUDE  0x4C4f4E474954554445

// I2C Comm states
#define WAITING_FOR_START     0
#define WAITING_FOR_US_DATA   1
#define WAITING_FOR_MOTOR_DIR 2
#define WAITING_FOR_TOF_DATA  3
#define WAITING_FOR_END       4
#define WAITING_FOR_MG	      5 // Waiting for (M)otor or (G)PS.

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
String command = "";  // Info recieved from RPi
int commState = 0;
bool respond = false; // Send GPS info back to RPi

int state = STOP_STATE;
bool fwd = true;          // Direction switch (temp)
bool on = false;          // Are the motors on.
bool initialized = false; // Sanity check
//-----------------

//=============================================================================
//                   I2C Command Structure (RPi -> Arduino):
// <START> -> <M | G> -> <US reading> -> <F | S | R> -> <ToF reading> -> <END>
//-----------------------------------------------------------------------------
//	 [ ALL COMMANDS AND VALUES ARE RECIEVED IN HEXADECIMAL FORMAT ]
//
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
//=============================================================================

void recieveData(int byteCount)
{
    while(0 < Wire.available())
    {
        // Concatenate each byte to the string.
        command += Wire.read();

    }
    
    Serial.print("data recieved");
    Serial.print(command);

    int cmd = command.toInt();

    if (commState == WAITING_FOR_START)
    {
        if((cmd & START) == START) { commState = WAITING_FOR_MG; }
    }
    else if (commState == WAITING_FOR MG)
    {
	if((cmd & CMD_M) == CMD_M) { commState = WAITING_FOR_US_DATA; }
	else if((cmd & CMD_G) == CMD_G) { respond = true; commState = WAITING_FOR_END; }
    }
    else if(commState == WAITING_FOR_US_DATA)
    {
	// Are we going to need additional conversion code here? -Alex
	inch = cmd;
	commState = WAITING_FOR_MOTOR_DIR;
    }
    else if (commState == WAITING_FOR_MOTOR_DIR)
    {
	if((cmd & M_FWD) == M_FWD)        { state = FORWARD_STATE; }
	else if((cmd & M_STOP) == M_STOP) { state = STOP_STATE; }
	else if((cmd & M_REV) == M_REV)   { state = REVERSE_STATE; }

	commState = WAITING_FOR_TOF_DATA;
    }
    else if (commState == WAITING_FOR_TOF_DATA)
    {
	// Are we going to need additional conversion code here? -Alex
	tofDistance = cmd;
	commState = WAITING_FOR_END;
    }
    else if (commState == WAITING_FOR_END)
    {
	if((cmd & END) == END) { commState = WAITING_FOR_START; }
    }
	
}

//=======================================================================================
//                        I2C Command Structure (Arduino -> RPi):
// <START> -> <LATITUDE> -> <latitude value> -> <LONGITUDE> -> <longitude value> -> <END>
//---------------------------------------------------------------------------------------
//	           [ COMMANDS ARE IN HEXADECIMAL, VALUES ARE IN DECIMAL ]
//=======================================================================================

void sendData()
{
    if(respond)
    {
	Wire.write(START);
	delayMicroseconds(5);
	Wire.write(LATITUDE);
	delayMicroseconds(5);
	Wire.write(0.00000); // Need to get actual value from GPS
	delayMicroseconds(5);
	Wire.write(LONGITUDE);
	delayMicroseconds(5);
	Wire.write(0.00000); // Need to get actual value from GPS
	delayMicroseconds(5);
	Wire.write(END);
    }
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

