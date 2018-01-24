/*DC Motor Tech Demo Program*/

#define enableA 10
#define input1 11
#define input2 12
#define b1 13
#define trigPIN1 1
#define echoPIN1 2
#define tof1 3
#define pir1 4

int rD = 0;
bool pressed = false;

void setup() {
  //set up pins and digital inputs
  Serial.begin(9600);
  pinMode(enableA, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(b1, INPUT);
  pinMode(trigPIN1, OUTPUT);
  pinMode(echoPIN1, INPUT);
  // Used for initial startup
  digitalWrite(enableA, LOW);
  digitalWrite(input1, HIGH);
  digitalWrite(input2, HIGH);
}

void loop()
{ 
  int destination = 0;
  int duration, distance;
  do
  {
    digitalWrite(trigPIN1, HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigPIN1, LOW);
    duration = pulseIn(echoPIN1, HIGH);
    distance = (duration/2)/12; 
    
    if(enableA == LOW)
    {
      motorON();
      }
    
    forward(distance);
  }
 }while(destination == 0)
}
/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors forward and begins incrementing*/
void forward(int x)
{
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
  delay(20);
  incrementsFORWARD(x);
}

/*This function is designed to enable the robot to move forward. The inputs are adjusted to
the orientation that spins the motors backward and begins incrementing*/
void backward(int x)
{
   digitalWrite(input1, LOW);
   digitalWrite(input2, HIGH);
   delay(20);
   incrementsBACKWARD(x);
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
void incrementsFORWARD(int x, int y, int z)
{
  for(int i; z = 1; i++)
  {
  if(z == 1)
  {
    motorOFF();
   }
  else  
  digitalWrite(trigPIN1, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPIN1, LOW);
  int d = pulseIn(echoPIN1, HIGH);
  x = (d/2)/12; 
  //y = code to register a 1 or a 0
  y = 0;
  if (y == 0)
  {
   if (x > 12)
   {
    analogWrite(enableA,255);
    } 
   else if(11 < x < 12)
   {
    analogWrite(enableA, 230);
    } 
   else if(10 < x < 11)
   {
    analogWrite(enableA, 205);
    }
   else if(9 < x < 10)
   {
    analogWrite(enableA, 180);
    }
   else if(8 < x < 9)
   {
    analogWrite(enableA, 155);
    }
   else if(7 < x < 8)
   {
    analogWrite(enableA,130);
    } 
   else if(6 < x < 7)
   {
    analogWrite(enableA, 105);
    } 
   else if(5 < x < 6)
   {
    analogWrite(enableA, 80);
    }
   else if(x > 4 && x < 5)
   {
    analogWrite(enableA, 55);
    }
   else if(x > 3 && x < 4)
   {
    analogWrite(enableA, 30);
    }
   else
   {
    analogWrite(enableA, 0);
    motorSTOP();
    backward(x);
    }
   }
  else
  {
    if (x > 12)
   {
    analogWrite(enableA,255);
    } 
   else if(x > 11 && x < 12)
   {
    analogWrite(enableA, 223);
    } 
   else if(x > 10 && x < 11)
   {
    analogWrite(enableA, 191);
    }
   else if(x > 9 && x < 10)
   {
    analogWrite(enableA, 159);
    }
   else if(x > 8 && x < 9)
   {
    analogWrite(enableA, 127);
    }
   else if(x > 7 && x < 8)
   {
    analogWrite(enableA,95);
    } 
   else if(x > 6 && x < 7)
   {
    analogWrite(enableA, 63);
    }
   else if(x > 5 && x < 6)
   {
    analogWrite(enableA, 31);
    }
   else
   {
    analogWrite(enableA, 0);
    motorSTOP();
    backward(x);
    }
   }
  }
 }
 
/*This function is designed to accelerate and deccelerate the motors in increments\
while moving backward. There are two individual incremental settings based on the signals
received by the PIR sensor and the time-of-flight sensors. If neither sensors are flagged 
(e.g. only sees a wall), then the function will go through the slower progression. If either
sensor is flagged, the function will go into the the steeper section. If the time-of-flight sensor
is flagged, then the system will go into an emergency stop function. */
void incrementsBACKWARD(int x, int y, int z)
{
  
  for(int i; z = 1; i++)
  {
  if(z == 1)
  {
    motorOFF();
   }
  else  
  digitalWrite(trigPIN1, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPIN1, LOW);
  int d = pulseIn(echoPIN1, HIGH);
  x = (d/2)/12; 
  //y = code to register a 1 or a 0
  y = 0;
  if (y == 0)
  {
   if (x > 12)
   {
    analogWrite(enableA,255);
    } 
   else if(11 < x < 12)
   {
    analogWrite(enableA, 230);
    } 
   else if(10 < x < 11)
   {
    analogWrite(enableA, 205);
    }
   else if(9 < x < 10)
   {
    analogWrite(enableA, 180);
    }
   else if(8 < x < 9)
   {
    analogWrite(enableA, 155);
    }
   else if(7 < x < 8)
   {
    analogWrite(enableA,130);
    } 
   else if(6 < x < 7)
   {
    analogWrite(enableA, 105);
    } 
   else if(5 < x < 6)
   {
    analogWrite(enableA, 80);
    }
   else if(x > 4 && x < 5)
   {
    analogWrite(enableA, 55);
    }
   else if(x > 3 && x < 4)
   {
    analogWrite(enableA, 30);
    }
   else
   {
    analogWrite(enableA, 0);
    motorSTOP();
    forward(x);
    }
   }
  else
  {
    if (x > 12)
   {
    analogWrite(enableA,255);
    } 
   else if(x > 11 && x < 12)
   {
    analogWrite(enableA, 223);
    } 
   else if(x > 10 && x < 11)
   {
    analogWrite(enableA, 191);
    }
   else if(x > 9 && x < 10)
   {
    analogWrite(enableA, 159);
    }
   else if(x > 8 && x < 9)
   {
    analogWrite(enableA, 127);
    }
   else if(x > 7 && x < 8)
   {
    analogWrite(enableA,95);
    } 
   else if(x > 6 && x < 7)
   {
    analogWrite(enableA, 63);
    }
   else if(x > 5 && x < 6)
   {
    analogWrite(enableA, 31);
    }
   else
   {
    analogWrite(enableA, 0);
    motorSTOP();
    forward(x);
    }
   }
  }
 }


//  // Read button - Debounce
//  if (digitalRead(b1) == true) 
//  {
//    pressed = !pressed;
//  }
//  while (digitalRead(b1) == true);
//  delay(20);
//  
//  //Motor Direction Control with Button
//  // If button is pressed - change rotation direction
//  if (pressed == true  & rD == 0) 
//  {
//    digitalWrite(input1, HIGH);
//    digitalWrite(input2, LOW);
//    rD = 1;
//    delay(20);
//  }
//  // If button is pressed - change rotation direction
//  if (pressed == false & rD == 1) {
//    digitalWrite(input1, LOW);
//    digitalWrite(input2, HIGH);
//    rD = 0;
//    delay(20);
//  }
