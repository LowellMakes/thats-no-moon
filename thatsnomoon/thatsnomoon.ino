// thats no moon

#define TRIGGER_PIN A0
#define ECHO_PIN A1

#define FAR_LIMIT 200

#define SPEED 50

#define MAX_DISTANCE      300

#define OPEN_TIME (2 * 1200)
#define CLOSE_TIME (2 * 1200)

#include <Servo.h>
#include <NewPing.h>

// Forward declarations
int ping_cm_BugFix();


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
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

void trigger() {
  pinMode(ECHO_PIN, OUTPUT);
  digitalWrite(ECHO_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
}

int ball_open = false;
void open_ball(){
  if(!ball_open){
    Serial.println("Opening");
    motor_forward();
    delay(OPEN_TIME);
    motor_stop();
    delay(2000);
    ball_open = true;
  }
    
}

void close_ball(){
  //ball_open=false;
  if(ball_open){
    Serial.println("Closing");
    motor_reverse();
    delay(CLOSE_TIME);
    motor_stop();
    delay(2000);
    ball_open = false;
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  servo.attach(10);
//  servo.write(93);
}

void loop() {

  long duration = 0;
  int distInCentimeters = 0;
  int k;

  distInCentimeters = ping_cm_BugFix();

    Serial.print("cm = ");
    Serial.print(distInCentimeters);
    Serial.println();
    
    if(distInCentimeters < 10){
     delay(100);
    }
    else if(distInCentimeters < FAR_LIMIT){
     open_ball();
    }
    else if(distInCentimeters > FAR_LIMIT){
     close_ball();
    }
  }

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

// This is a wrapper function that tries to avoid a bug with the
// HC-SR04 modules where they can get stuck return zero forever
int ping_cm_BugFix(){
  
        int distCm = sonar.ping_cm();
        
        if(distCm == 0){
          delay(100);
          pinMode(ECHO_PIN, OUTPUT);
          digitalWrite(ECHO_PIN, LOW);
          delay(100);
          pinMode(ECHO_PIN, INPUT);
        }

        return distCm;
}
