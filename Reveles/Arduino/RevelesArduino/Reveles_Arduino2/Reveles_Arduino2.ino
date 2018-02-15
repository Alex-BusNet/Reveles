 /*
 * Reveles DC Motor Control Program
 * Motor Control written by Frank Hittel
 * I2C communication written by Alisha Johnson
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include <Servo.h>

#define SLAVE_ADDRESS   0x04
#define enableA         10
#define input1          11
#define input2          12
#define frontTrigPin    2 // trigPIN1
#define frontEchoPin    3 // echoPIN1
#define rearTrigPin     6 // trigPIN2
#define rearEchoPin     7 // echoPIN2
#define GPS_RX_PIN      1 // HardwareSerial TX
#define GPS_TX_PIN      0 // HardwareSerial RX
#define LED             13
// SCL A5
// SDA A4
#define GPS_READY_PIN   8
#define FRONT_SERVO_SIG 4
#define REAR_SERVO_SIG  5

// Motor Drive states
#define FORWARD_STATE   1
#define STOP_STATE      0
#define REVERSE_STATE  -1

// I2C Comm commands
#define COM_CHECK  0xAA
#define START_CMD  0x00
#define END_CMD    0xA3
#define M_FWD      0x11
#define CMD_G      0x20
#define CMD_M      0x10
#define M_REV      0x12
#define M_STOP     0x13
#define LATITUDE   0x21
#define LONGITUDE  0x22
#define CMD_FLUSH  0x4C

// I2C Comm states
#define WAITING_FOR_HELLO     6
#define WAITING_FOR_START     0
#define WAITING_FOR_US_DATA   1
#define WAITING_FOR_MOTOR_DIR 2
#define WAITING_FOR_TOF_DATA  3
#define WAITING_FOR_END       4
#define WAITING_FOR_MG        5 // Waiting for (M)otor or (G)PS.

// Other defines
#define INT_MAX 32767

//-----------------
// Variables 
long duration = 0L; // Time for response to arrive from (temp)
int distance = 0;   // Calculated value from US (temp)
int inch = 0;       // Calculated value from US (temp)
int pwmOUT = 0;     // Motor Drive speed signal
int tofDistance = INT_MAX;
int rD = 0;
int destination = 0;
int latitude = 0; // int type is temporary
int longitude = 0; // int type is temporary

Adafruit_GPS gps(&Serial);
HardwareSerial hs = Serial;
Servo frontServo;
Servo rearServo;

// I2C variables
//byte cmd = CMD_FLUSH; // Info recieved from RPi
int commState = WAITING_FOR_HELLO;
bool respond = false; // Send GPS info back to RPi
String commStateStrings[] = {"WAITING_FOR_START", "WAITING_FOR_US_DATA", 
                             "WAITING_FOR_MOTOR_DIR", "WAITING_FOR_TOF_DATA",
                             "WAITING_FOR_END\t", "WAITING_FOR_MG\t", "WAITING_FOR_HELLO" };

int state = STOP_STATE;
bool fwd = true;          // Direction switch (temp)
bool on = false;          // Are the motors on.
bool initialized = false; // Sanity check
//-----------------

//================================================================================
//                      I2C Command Structure (RPi -> Arduino):
// <START> -> <Function> -> <US reading> -> <Direction> -> <ToF reading> -> <END>
//--------------------------------------------------------------------------------
//            [ ALL COMMANDS ARE RECIEVED IN BINARY, VALUES ARE IN DECIMAL ]
//
// Possible values
//    Function:
//      CMD_M - RPi is sending drive instructions.
//      CMD_G - RPi is requesting GPS coordinates.
//    US Value: Floating Point value for PWM signal.
//      (Motor command only)
//      Value is in inches.
//      Value is not adjusted to fit between 255 and 0.
//    Direction: (Motor command only)
//      M_FWD  - Motors should drive forwards.
//      M_STOP - Motors should stop driving.
//      M_REV  - Motors should drive in reverse.
//    ToF value: Floating point value from Time of Flight sensors.
//      (Motor command only)
//      Value is in inches.
//================================================================================

void recieveData(int byteCount)
{
    uint8_t cmd = CMD_FLUSH;
    int data = 0;
    Serial.print("{" + String(byteCount) + "} Comm state: " + commStateStrings[commState]);
  
    while(0 < Wire.available())
    {
        if((commState == WAITING_FOR_US_DATA) || (commState == WAITING_FOR_TOF_DATA))
            data = Wire.read();
        else
            cmd = Wire.read();
    }
   
    if((commState != WAITING_FOR_US_DATA) && (commState != WAITING_FOR_TOF_DATA))
        Serial.print("\tCommand: 0x" + String(cmd, HEX));
    else
        Serial.print("\t   Data: " + String(data));

    if(commState == WAITING_FOR_HELLO)
    {
        if((cmd & COM_CHECK) == COM_CHECK) { commState = WAITING_FOR_START; Wire.write(COM_CHECK); }
    }
    else if (commState == WAITING_FOR_START)
    {
        if((cmd & START_CMD) == START_CMD) { commState = WAITING_FOR_MG; }
    }
    else if (commState == WAITING_FOR_MG)
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
        if((cmd & M_STOP) == M_STOP)      { state = STOP_STATE; motorSTOP(); }
        else if((cmd & M_FWD) == M_FWD)   { state = FORWARD_STATE; forward();}
        else if((cmd & M_REV) == M_REV)   { state = REVERSE_STATE; backward(); }
      
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
        if((cmd & END_CMD) == END_CMD) { commState = WAITING_FOR_START; }
    } 

    Serial.println("\tNew Comm state: " + commStateStrings[commState]);
}

//=======================================================================================
//                        I2C Command Structure (Arduino -> RPi):
// <START> -> <LATITUDE> -> <latitude value> -> <LONGITUDE> -> <longitude value> -> <END>
//---------------------------------------------------------------------------------------
//                   [ COMMANDS ARE IN BINARY, VALUES ARE IN DECIMAL ]
//=======================================================================================

void sendData()
{
    readGPS();
      
    Wire.write(START_CMD);
    delay(85);
    Wire.write(LATITUDE);
    delay(85);
    Wire.write(latitude);
    delay(85);
    Wire.write(LONGITUDE);
    delay(85);
    Wire.write(longitude);
    delay(85);
    Wire.write(END_CMD);
}

void readGPS()
{
    gps.read();

    if(gps.fix)
    {
      latitude = gps.latitude;
      longitude = gps.longitude;
    }
    digitalWrite(GPS_READY_PIN, LOW);
    delay(5);
    digitalWrite(GPS_READY_PIN, HIGH);
    delay(5);
    digitalWrite(GPS_READY_PIN, LOW);
}

/*This function is designed for use in the initial boot-up of the robot. The motors will be turned on if not already and begin moving forward,
FUTURE ITERATIONS SHOULD INVOLVE A DECISION TO CHANGE ITS STARTING DIRECTION!*/
void start()
{
  if(!on)
  {
      motorON();
      if(state == FORWARD_STATE) { forward(); }
      else if(state == REVERSE_STATE) { backward(); }
      else { motorSTOP(); }
  }
  
  if(state == FORWARD_STATE)
  {
    driveFORWARD(); 
  }
  else if(state == REVERSE_STATE)
  {
    driveBACKWARD();
  }
}

/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors forward and begins incrementing*/
void forward()
{
//    Serial.println("Driving Forward");
    digitalWrite(input1, HIGH);
    digitalWrite(input2, LOW);
//    driveFORWARD();
}

/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors backward and begins incrementing*/
void backward()
{
//    Serial.println("Driving Backwards");
    digitalWrite(input1, LOW);
    digitalWrite(input2, HIGH);
//    driveBACKWARD();
}

/*This function is designed to stop movement while still ready to move. The input pins will be turned high to prevent current flow,
resulting in the robot's inability to move but can begin once a proper current path is made. */
void motorSTOP()
{
//    Serial.println("Stopping");
    digitalWrite(input1, HIGH);
    digitalWrite(input2, HIGH);
    readGPS();
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
//  digitalWrite(LED, HIGH);
//  digitalWrite(frontTrigPin, LOW);
//  delayMicroseconds(2);
//  digitalWrite(frontTrigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(frontTrigPin, LOW);
//  duration = pulseIn(frontEchoPin, HIGH);
//  distance = duration*0.034/2;//calculate distance in cm
//  inch = distance/2.5; //calculate in inches 
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
//  digitalWrite(LED, LOW);
//  digitalWrite(rearTrigPin, LOW);
//  delayMicroseconds(2);
//  digitalWrite(rearTrigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(rearTrigPin, LOW);
//  duration = pulseIn(rearEchoPin, HIGH);
//  distance = duration*0.034/2;//calculate distance in cm
//  inch = distance/2.5; //calculate in inches
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
  // Serial Settings for reading GPS
//  Serial.begin(115200);
  Serial.begin(9600);
//  delay(5000);

  // Start reading GPS with passed baud rate
  gps.begin(9600); 
  // Turn on RMC (recommended minimum) and GGA (fix data) including altitude
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set update rate to 1Hz
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  // Request updates on antenna status.
//  gps.sendCommand(PGCMD_ANTENNA);

  // Motor Control I/O
  pinMode(enableA, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(frontTrigPin, OUTPUT);
  pinMode(rearTrigPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(rearEchoPin, INPUT);
  pinMode(GPS_READY_PIN, OUTPUT);

  // Servo I/O
  frontServo.attach(FRONT_SERVO_SIG);
  rearServo.attach(REAR_SERVO_SIG);
  
  // Used for initial motor startup
  digitalWrite(enableA, LOW);
  digitalWrite(input1, LOW);
  digitalWrite(input2, LOW);

  //initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  //define callbacks for i2c communication
  Wire.onReceive(recieveData);
  Wire.onRequest(sendData);

  // Make sure the interrupt pin is low.
  digitalWrite(GPS_READY_PIN, LOW);

  // Setup motor drive flags.
  on = false;
  state = STOP_STATE;
  initialized = true;
  commState = WAITING_FOR_HELLO;
  inch = 156;
}

void loop()
{ 
  if(initialized == true)
  {
    start();

    if(respond)
      sendData();
  }
}

