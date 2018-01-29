/*DC Motor Tech Demo Program*/

#define enableA 10
#define input1 11
#define input2 12
#define trigPIN1 1
#define echoPIN1 2
#define tof1 3
#define pir1 4

int rD = 0;
int destination
bool pressed = false;

void setup() {
  //set up pins and digital inputs
  Serial.begin(9600);
  pinMode(enableA, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(b1, INPUT);
  
  // Used for initial motor startup
  digitalWrite(enableA, LOW);
  digitalWrite(input1, LOW);
  digitalWrite(input2, LOW);
}

void loop()
{ 
  int destination == 0
  do
  {
    start();
  }
 }while(destination == 0)
}

/*This function is designed for use in the initial boot-up of the robot. The motors will be turned on if not already and begin moving forward,
FUTURE ITERATIONS SHOULD INVOLVE A DECISION TO CHANGE ITS STARTING DIRECTION!*/
void start()
{
  if(enable == LOW)
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
  delay(20);
  }
/*This function is designed as the Emergency Stop for the robot. When the time-of-flight sensor is flagged, the robot will shut off.*/
void motorOFF()
{
  digitalWrite(enableA, LOW);
  }
/*This function is designed to accelerate and deccelerate the motors in increments\
while moving forward. There are two individual incremental settings based on the signals
received by the PIR sensor and the time-of-flight sensors. If neither sensors are flagged 
(e.g. only sees a wall), then the function will go through the slower progression. If either
sensor is flagged, the function will go into the the steeper section. If the time-of-flight sensor
is flagged, then the system will go into an emergency stop function. */
void driveFORWARD()
{
  int pwmOUT = (distUS1 * 2)-1;
  if(pwmOUT < 48)
  {
    motorOFF();
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
  int pwmOUT = (distUS2 * 2)-1;
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
