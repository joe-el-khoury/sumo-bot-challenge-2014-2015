//Import Ultrasonic Sensor and Servo Libraries
#include <NewPing.h>
#include <Servo.h>

//Motor Speed Control Pins
const int l_speed_pin = 6;
const int r_speed_pin = 11;

//Motor Direction Pins
const int l_dir_pin1 = 7;
const int l_dir_pin2 = 8;
const int r_dir_pin1 = 12;
const int r_dir_pin2 = 13;

//Line Sensor Inputs
const int l_line_pin = A0;
const int r_line_pin = A1;
//Input value beyond which indicates a line is detected
const int line_thresh = 720;   

//Distance Sensor Pins
const int trig_pin = 2;
const int echo_pin = 3;
//Limit max distance reading (cm)
const int dist_limit = 100;
//Distance within which opponent will be attacked
const int attack_dist = 75;

//Distance Sensor Initializations
NewPing ultra(trig_pin, echo_pin, dist_limit);

//Variable to keep track of and change robot states
//Initially set to hunt state
//0: Survive ; 1: Hunt ; 2: Attack
const int Survive = 0;
const int Hunt = 1;
const int Attack = 2;
int state = Hunt;

//Servo object and pin
const int servo_pin = 9;
Servo ramp;


void setup(){
  for(int i = 6; i <= 8; i++){
    pinMode(i, OUTPUT);
  }
  for(int i = 11; i <= 13; i++){
    pinMode(i, OUTPUT);
  } 

  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  //Attach servo object "ramp" to servo_pin and lower ramp
  //to prepare for combat
  ramp.attach(servo_pin);
  ramp.write(10);
}


/*Driving Functions*/
//Forward function
void forward(){
  //Set direction
  digitalWrite(l_dir_pin1, HIGH);
  digitalWrite(r_dir_pin1, HIGH);
  digitalWrite(l_dir_pin2, LOW);
  digitalWrite(r_dir_pin2, LOW);

  //Set full speed
  analogWrite(l_speed_pin, 255);
  analogWrite(r_speed_pin, 255); 
}

//Left turn function
void turn_left(){
  //Set direction
  digitalWrite(l_dir_pin1, LOW);
  digitalWrite(r_dir_pin1, HIGH);
  digitalWrite(l_dir_pin2, HIGH);
  digitalWrite(r_dir_pin2, LOW);

  //Set full speed
  analogWrite(l_speed_pin, 255);
  analogWrite(r_speed_pin, 255);
}

//Right turn function
void turn_right(){
  //Set direction
  digitalWrite(l_dir_pin1, HIGH);
  digitalWrite(r_dir_pin1, LOW);
  digitalWrite(l_dir_pin2, LOW);
  digitalWrite(r_dir_pin2, HIGH);

  //Set full speed
  analogWrite(l_speed_pin, 255);
  analogWrite(r_speed_pin, 255);
}

//Brake function
void brake(){
  //Disable direction
  digitalWrite(l_dir_pin1, LOW);
  digitalWrite(r_dir_pin1, LOW);
  digitalWrite(l_dir_pin2, LOW);
  digitalWrite(r_dir_pin2, LOW);

  //Set no speed
  analogWrite(l_speed_pin, 0);
  analogWrite(r_speed_pin, 0);
}


/*Robot State Functions*/
//Survive State
void survive(){
  int l_line = analogRead(l_line_pin);
  int r_line = analogRead(r_line_pin);

  //If line detected on left side, turn right and drive forward a bit
  if(l_line < line_thresh){
    brake();
    turn_right();
    delay(500);
    forward();
    delay(200);
    return;
  }

  //If line detected on right side, turn left and drive forward a bit
  if(r_line < line_thresh){
    brake();
    turn_left();
    delay(500);
    forward();
    delay(200);
    return;
  }

  //If no line detected, switch to hunt state
  else
    state = Hunt;
    //{forward();
    //delay(200);}
}

//Hunt state
void hunt(){
  //If a line is detected on either side, switch to survive state
  int l_line = analogRead(l_line_pin);
  int r_line = analogRead(r_line_pin);

  if(l_line < line_thresh || r_line < line_thresh){
    state = Survive;
    return;
  }

  //If opponent is detected, switch to attack state
  int dist = ultra.ping_cm();

  if(dist < attack_dist){
    state = Attack;
    return;
  }

  //Otherwise, rotate slightly and continue hunting 
  else{
    turn_right();
    delay(200);
    brake();
  }
}

//Attack State
void attack(){
  //If a line is detected on either side, switch to survive state
  int l_line = analogRead(l_line_pin);
  int r_line = analogRead(r_line_pin);

  if(l_line < line_thresh || r_line < line_thresh){
    state = Survive;
    return;
  }

  int dist = ultra.ping_cm();

  //If opponent is not detected, switch to hunt state
  if(dist > attack_dist){
    state = Hunt;
    return;
  }

  //Otherwise, if opponent is detected, drive forward to attack
  else{
    forward();
    delay(500);
    brake();
  }
}


void loop(){
  //Use the switch function and "state" variable
  //to toggle between robot states
  switch(state){
  case Survive:
    survive();
    break;

  case Hunt:
    hunt();
    break;

  case Attack:
    attack();
    break;
  }
  //survive();
}


