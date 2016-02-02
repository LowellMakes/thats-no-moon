#define USE_PING

#ifdef USE_PING
#include <NewPing.h>
#endif

// thats no moon

#define TRIGGER_PIN A0
#define ECHO_PIN A1

#define CLOSE_PIN 4
#define OPEN_PIN  5

#define FAR_LIMIT 125

#define SPEED 50

#define MAX_DISTANCE      500
#define BAD_READING 500

#define MAX_OPEN_CLOSE_TIME 50

#define CLOCK_WISE 180
#define STOP 93
#define COUNTER_CLOCK_WISE 0

#include <Servo.h>
#include <NewPing.h>
#include <RunningAverage.h>


// Forward declarations
int ping_cm_BugFix();

int initialDirection = 0;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
RunningAverage distanceRA(10);
Servo servo;


void move_motor(int dir){
  servo.attach(10);
  if(dir){
      servo.write(CLOCK_WISE);
  } else {
    servo.write(COUNTER_CLOCK_WISE);
  }
}

void motor_stop(){
  servo.write(STOP);
  Serial.println("Stopped");
  servo.detach();
}

void open_ball(){
  if(ball_is_open()){
    Serial.println("Already Opened");
  } else {
  
    move_motor(initialDirection);
    delay(100);
    Serial.println("Open");
  
    int timeout = MAX_OPEN_CLOSE_TIME;
    
    while(!ball_is_open() && (timeout > 0)){
      Serial.println("Opening");
      timeout--;
      delay(100);
      if(ball_is_closed()){
        initialDirection != initialDirection;   
      }
    }
  
    motor_stop();
    delay(250); 
    if(ball_is_open()){
      Serial.println("Opened");
    }
  }
}

void close_ball(){
  if(ball_is_closed()){
    Serial.println("Already Closed");
  } else {
    move_motor(!initialDirection);
    delay(100);
    Serial.println("Close");
  
    int timeout = MAX_OPEN_CLOSE_TIME;
    
    while(!ball_is_closed() && (timeout > 0)){
        timeout--;
        delay(100);
        if(ball_is_open()){
          initialDirection != initialDirection;   
        }
    }
    
    motor_stop();

    delay(250);  
    if(ball_is_closed()){
      Serial.println("Closed");
    }
  }
}

bool ball_is_closed(){
  Serial.println("top pin");
  return !digitalRead(CLOSE_PIN);
}

bool ball_is_open(){
  Serial.println("bottom pin");
  return !digitalRead(OPEN_PIN);
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

  close_ball();
}

int oldDistInCentimeters = 0;

void loop() {

  long duration = 0;
  int distInCentimeters = 0;
  int k;

  delay(150);
  
  distInCentimeters = ping_cm_BugFix();

  if(distInCentimeters == oldDistInCentimeters){
    return;
  } else {
    oldDistInCentimeters = distInCentimeters; 
  }
  
  while(distInCentimeters == BAD_READING){
    Serial.println("Bad Reading");
    if(ball_is_open()){
      close_ball();
    }
    distInCentimeters = ping_cm_BugFix();
    
  }
  
//  distanceRA.addValue(distInCentimeters);
//  distInCentimeters = distanceRA.getAverage();
  
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

  while(echoTime == 0){
    delay(100);
    pinMode(ECHO_PIN, OUTPUT);
    digitalWrite(ECHO_PIN, LOW);
    delay(100);
    pinMode(ECHO_PIN, INPUT);
    return BAD_READING;
  }
  return sonar.convert_cm(echoTime);
}
