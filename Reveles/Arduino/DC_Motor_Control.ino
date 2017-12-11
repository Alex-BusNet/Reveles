/*DC Motor Tech Demo Program*/

#define enableA 10
#define input1 11
#define input2 12
#define b1 1

int rD = 0;
int pressed = false;

void setup() {
  //set up pins and digital inputs
  pinMode(enableA, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(b1, INPUT);
  
  // Used for initial startup
  digitalWrite(input1, LOW);
  digitalWrite(input2, HIGH);
}

void loop()
  {
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
  if (pressed == false & rD == 1) {
    digitalWrite(input1, LOW);
    digitalWrite(input2, HIGH);
    rD = 0;
    delay(20);
  }
}
