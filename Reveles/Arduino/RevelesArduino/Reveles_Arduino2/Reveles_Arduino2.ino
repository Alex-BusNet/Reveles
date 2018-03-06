 /*
 * Reveles DC Motor Control Program
 * Motor Control written by Frank Hittel
 * I2C communication written by Alisha Johnson
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include <Servo.h>

#define SLAVE_ADDRESS   0x04 // I2C address of the Arduino
//----------------------------------
//        PIN DEFINES
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

//----------------------------------
//        Motor Drive states
#define FORWARD_STATE   1
#define STOP_STATE      0
#define REVERSE_STATE  -1

//----------------------------------
//        Servo Drive states
#define NEUTRAL_STATE   0
#define LEFT_STATE     -1
#define RIGHT_STATE     1

//----------------------------------
//        I2C Comm commands
#define COM_CHECK   0xAA
#define START_CMD   0x00
#define END_CMD     0xA3
#define M_FWD       0x11
#define CMD_G       0x20
#define CMD_M       0x10
#define CMD_S       0x30
#define CMD_T	    0x40
#define M_REV       0x12
#define M_STOP      0x13
#define LATITUDE    0x21
#define LONGITUDE   0x22
#define TURN_LEFT   0x31
#define TURN_RIGHT  0x32
#define RET_NEUTRAL 0x33
#define CMD_FLUSH   0x44

//----------------------------------
//        I2C Comm states
#define WAITING_FOR_START      0
#define WAITING_FOR_US_DATA    1
#define WAITING_FOR_MOTOR_DIR  2
#define WAITING_FOR_TOF_DATA   3
#define WAITING_FOR_END        4
#define WAITING_FOR_MGS        5 // Waiting for (M)otor, (G)PS, or (S)ervo.
#define WAITING_FOR_HELLO      6
#define WAITING_FOR_SERVO_DIR  7
#define WAITING_FOR_SERVO_VAL  8

#define READY_FOR_HELLO        9
#define READY_FOR_GPS_RESPONSE 10
#define NO_DATA_READY          11

//----------------------------------
//        Other defines
#define INT_MAX 32767

//----------------------------------
//        Variables 
long duration = 0L;  // Time for response to arrive from (temp)
int distance = 0;    // Calculated value from US (temp)
int inch = 0;        // Ultrasonic value received from RPi
int pwmOUT = 0;      // Motor Drive speed signal
int turnValue = 0;   // Turn value received from RPi (may be unneeded)
int sPosition = 90; // Current position of Servos (in degrees)..
int tofDistance = INT_MAX;
int rD = 0;
int destination = 0;
int latitude = 0; // int type is temporary
int longitude = 0; // int type is temporary

int mState = STOP_STATE;
int sState = NEUTRAL_STATE;
bool fwd = true;                // Direction switch (temp)
bool on = false;                // Are the motors on.
bool updateMotorState = true;   // Was a command given to change motor direction.
bool updateServoState = false;  // Was a command given to change the servo position.
bool initialized = false;       // Sanity check

Adafruit_GPS gps(&Serial);
HardwareSerial hs = Serial;
Servo frontServo;
Servo rearServo;

//----------------------------------
//          I2C variables
int commState = WAITING_FOR_HELLO;
int responseState = NO_DATA_READY;
bool respond = false;       // Send GPS info back to RPi
String commStateStrings[] = {"WAITING_FOR_START", "WAITING_FOR_US_DATA",                    //
                             "WAITING_FOR_MOTOR_DIR", "WAITING_FOR_TOF_DATA",               // This is for debugging
                             "WAITING_FOR_END\t", "WAITING_FOR_MGS\t", "WAITING_FOR_HELLO",  // purposes.
                             "WAITING_FOR_SERVO_DIR", "WAITING_FOR_SERVO_VAL"};             //

//================================================================================
//                      I2C Command Structure (RPi -> Arduino):
// <START> -> <Function>|-> <US reading> -> <Direction> -> <ToF reading> -> <END>
//                      |-> <Servo Direction> -> <Servo value> -> <END>
//                      |-> <END> (GPS Command)
//--------------------------------------------------------------------------------
//          [ ALL COMMANDS ARE SENT IN HEXADECIMAL, VALUES ARE IN DECIMAL ]
//
// Possible values
//    Function:
//      CMD_M - RPi is sending drive instructions.
//      CMD_G - RPi is requesting GPS coordinates.
//      CMD_S - RPi is sending turning instructions.
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
//    Servo Direction: (Servo command only)
//      TURN_RIGHT - Indicates Reveles should turn to the right. (Relative to self).
//      TURN_LEFT  - Indicates Reveles should turn to the left. (Relative to self).
//    Servo Value: (Servo command only)
//      Degrees servo should turn.
//      Value is always positive.
//      Value is not mapped to PWM output.
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
        if(cmd == COM_CHECK) { commState = WAITING_FOR_START; responseState = READY_FOR_HELLO; }
    }
    else if (commState == WAITING_FOR_START)
    {
        if(cmd == START_CMD) { commState = WAITING_FOR_MGS; }
    }
    else if (commState == WAITING_FOR_MGS)
    {
        if(cmd == CMD_M) { commState = WAITING_FOR_US_DATA; }
        else if(cmd == CMD_G) { respond = true; commState = WAITING_FOR_END; responseState = READY_FOR_GPS_RESPONSE; }
        else if(cmd == CMD_S) { commState = WAITING_FOR_SERVO_DIR; }
    }
    else if(commState == WAITING_FOR_US_DATA)
    {
        inch = data;
        commState = WAITING_FOR_MOTOR_DIR;
    }
    else if (commState == WAITING_FOR_MOTOR_DIR)
    {
        if(cmd == M_STOP)      { mState = STOP_STATE; updateMotorState = true; }
        else if(cmd == M_FWD)   { mState = FORWARD_STATE; updateMotorState = true; }
        else if(cmd == M_REV)   { mState = REVERSE_STATE; updateMotorState = true; }
      
        commState = WAITING_FOR_TOF_DATA;
    }
    else if (commState == WAITING_FOR_TOF_DATA)
    {
        tofDistance = data;
        commState = WAITING_FOR_END;
    }
    else if (commState == WAITING_FOR_SERVO_DIR)
    {
        if(cmd == TURN_LEFT)        { sState = LEFT_STATE; updateServoState = true; }
        else if(cmd == TURN_RIGHT)  { sState = RIGHT_STATE; updateServoState = true; }
        else if(cmd == RET_NEUTRAL) { sState = NEUTRAL_STATE; updateServoState = true; }

        commState = WAITING_FOR_SERVO_VAL;
    }
    else if (commState == WAITING_FOR_SERVO_VAL)
    {
        // This command may be unneeded. -Alex
        turnValue = cmd;
        commState = WAITING_FOR_END;
    }
    else if (commState == WAITING_FOR_END)
    {
        if(cmd == END_CMD) { commState = WAITING_FOR_START; }
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
    if(responseState = READY_FOR_HELLO)
    {
        Wire.write(COM_CHECK);
        responseState = NO_DATA_READY;
    }
    else if(responseState = READY_FOR_GPS_RESPONSE)
    {
        responseState = NO_DATA_READY;
      
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
}

void readGPS()
{
    char c = gps.read();

    if (c) { Serial.write(c); }

    if(gps.newNMEAreceived())
    {
        Serial.println("New NMEA sententence");
        if(!gps.parse(gps.lastNMEA()))
        {
            Serial.println("Failed to parse!");
            return;
        }
    }
    
    if(gps.fix)
    {
        latitude = gps.latitude;
        longitude = gps.longitude;
        
        Serial.print("Latitude: ");
        Serial.print(gps.latitude, 4);
        Serial.println(gps.lat);
        Serial.print("Longitude: ");
        Serial.print(gps.longitude, 4);
        Serial.println(gps.lon);
    }
    else
        Serial.println("No GPS Fix!");

    
    digitalWrite(GPS_READY_PIN, LOW);
    delay(5);
    digitalWrite(GPS_READY_PIN, HIGH);
    delay(5);
    digitalWrite(GPS_READY_PIN, LOW);
    respond = false;
}

/*This function is designed for use in the initial boot-up of the robot. The motors will be turned on if not already and begin moving forward,
FUTURE ITERATIONS SHOULD INVOLVE A DECISION TO CHANGE ITS STARTING DIRECTION!*/
void start()
{
    if(!on)
    {
        motorON();
    }

    if(updateMotorState)
    {
        updateMotorState = false;
        if(mState == FORWARD_STATE) { forward(); }
        else if(mState == REVERSE_STATE) { backward(); }
        else { motorSTOP(); }
    }

    if(updateServoState)
    {
        updateServoState = false;
        if(sState == LEFT_STATE) { turnLeft(); }
        else if(sState == RIGHT_STATE) { turnRight(); }
        else { returnToNeutral(); }
    }
    
    if(mState == FORWARD_STATE)
    {
        driveFORWARD(); 
    }
    else if(mState == REVERSE_STATE)
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
//    readGPS(); // Temporary; used for testing.
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
        mState = STOP_STATE;
//        motorSTOP();
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
        mState = STOP_STATE;
//        motorSTOP();
    }
    else if (pwmOUT > 255)
    {
        pwmOUT = 255;
    }
    
    analogWrite(enableA,pwmOUT);
}

/*
 * Servos should turn such that the wheels
 * are pointing in different directions.
 */
void turnLeft()
{
    /* 
     *  This code is borrowed from earlier versions
     * and may not work properly
     */
     
     frontServo.write(sPosition);
     rearServo.write(sPosition);

     for(; sPosition <= 180; sPosition++)
     {
        frontServo.write(sPosition);
        rearServo.write(sPosition);
     }

     for(; sPosition >= 90; sPosition--)
     {
        frontServo.write(sPosition);
        rearServo.write(sPosition);
     }
}

void turnRight()
{
    /* 
     *  This code is borrowed from earlier versions
     * and may not work properly
     */
         
     frontServo.write(sPosition);
     rearServo.write(sPosition);

     for(; sPosition >= 0; sPosition--)
     {
        frontServo.write(sPosition);
        rearServo.write(sPosition);
     }

     // Wait 1 second before returning servos to center.
     delay(1000);

     for(; sPosition <= 90; sPosition++)
     {
        frontServo.write(sPosition);
        rearServo.write(sPosition);
     }
}

void returnToNeutral()
{
    if(sPosition < 90)
    {
        for(; sPosition <= 90; sPosition++)
        {
            frontServo.write(sPosition);
            rearServo.write(sPosition);
        }
    }
    else if(sPosition > 90)
    {
         for(; sPosition >= 90; sPosition--)
         {
            frontServo.write(sPosition);
            rearServo.write(sPosition);
         }
    }
}

void setup() {
    // Serial Settings for reading GPS
//    Serial.begin(115200);
    Serial.begin(9600);
//    delay(5000);

    // Start reading GPS with passed baud rate
    gps.begin(9600); 
    // Turn on RMC (recommended minimum) and GGA (fix data) including altitude
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // Set update rate to 1Hz
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    // Request updates on antenna status.
    gps.sendCommand(PGCMD_ANTENNA);
    delay(1000);
    Serial.println(PMTK_Q_RELEASE);

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
    mState = STOP_STATE;
    sState = NEUTRAL_STATE;
    commState = WAITING_FOR_HELLO;
    inch = 156;
    frontServo.write(sPosition);
    rearServo.write(sPosition);
    initialized = true;
}

void loop()
{ 
    if(initialized == true)
    {
        start();
    
        if(respond)
            readGPS();
    }
}

