// thats no moon

#define TRIGGER_PIN A0
#define ECHO_PIN A1

#define CLOSE_PIN 4
#define OPEN_PIN  5

#define FAR_LIMIT 125

#define SPEED 50

#define MAX_DISTANCE      500
#define BAD_READING 500

#define MAX_OPEN_CLOSE_TIME 30

#include <Servo.h>
#include <NewPing.h>
#include <RunningAverage.h>

// Forward declarations
int ping_cm_BugFix();


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
RunningAverage distanceRA(10);
Servo servo;

void motor_forward(){
  servo.attach(10);
  servo.write(0);
  Serial.println("Forward");
}

void motor_reverse(){
  servo.attach(10);
  servo.write(180);
  Serial.println("Reverse");
}

void motor_stop(){
  servo.write(93);
  Serial.println("Stopped");
  servo.detach();
}

void open_ball(){
  if(!ball_is_open()){
    Serial.println("distInCentimeters");
    Serial.println("Opening");
    motor_forward();
    int timeout = MAX_OPEN_CLOSE_TIME;
    while(!ball_is_open() && (timeout > 0)){
      timeout--;
      delay(100);
    };
    motor_stop();
    if(ball_is_open()){
      Serial.println("Opened");
    }
  }
}

void close_ball(){
  if(!ball_is_closed()){
    Serial.println("Closing");
    motor_reverse();
    int timeout = MAX_OPEN_CLOSE_TIME;
    while(!ball_is_closed() && (timeout > 0)){
      timeout--;
      delay(100);
    };
    motor_stop();
    if(ball_is_closed()){
      Serial.println("Closed");
    }
  }
}

bool ball_is_closed(){
  return !digitalRead(CLOSE_PIN);
  Serial.println("top pin");
}

bool ball_is_open(){
  return !digitalRead(OPEN_PIN);
  Serial.println("bottom pin");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(CLOSE_PIN, INPUT);
  pinMode(OPEN_PIN, INPUT);
  servo.attach(10);
  distanceRA.clear(); //clear the running average
}

void loop() {

  long duration = 0;
  int distInCentimeters = 0;
  int k;

  distInCentimeters = ping_cm_BugFix();
  if(distInCentimeters == BAD_READING){
    if(ball_is_open()){
      close_ball();
    }
    return;
  }
  
  distanceRA.addValue(distInCentimeters);
  distInCentimeters = distanceRA.getAverage();

  Serial.print("cm = ");
  Serial.print(distInCentimeters);
  Serial.println();
    

  if(ball_is_closed()){
    Serial.println("Ball is closed");
  } 

  if(ball_is_open()){
    Serial.println("Ball is opened");
  }

  if(distInCentimeters < FAR_LIMIT){
    open_ball();
    Serial.println(distInCentimeters);
    Serial.println(" ");
  }
//  else if((distInCentimeters > FAR_LIMIT) || (distInCentimeters == 0)){
    else if(distInCentimeters > FAR_LIMIT){
    Serial.println(distInCentimeters);
    close_ball();
    Serial.println(" ");
  }
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

// This is a wrapper function that tries to avoid a bug with the
// HC-SR04 modules where they can get stuck return zero forever
int ping_cm_BugFix(){
  
  long echoTime = sonar.ping();

  if(echoTime == 0){
    delay(100);
    pinMode(ECHO_PIN, OUTPUT);
    digitalWrite(ECHO_PIN, LOW);
    delay(100);
    pinMode(ECHO_PIN, INPUT);
    return BAD_READING;
  }
  return sonar.convert_cm(echoTime);
}
