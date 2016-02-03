/*
*
* thatsnomoon
* Uses a servo to open and close an IKEA light based on motion detection.
* 
* 
*/

#define PIR_PIN A0
#define PIR_CAL_TIME 10 //PIR Calibration time in seconds, datasheet says 10-60 seconds
//close and open pins are connected to limit switches 
#define CLOSE_PIN 4
#define OPEN_PIN  5

#define MIN_TIME_FOR_OPEN_BALL (2 * 1000) //min time for the ball to be open in milli seconds

#define MAX_OPEN_CLOSE_TIME (5 * 1000) //max time to open or close the ball in seconds, this is a safety incase the limit switch doesn't engage 

#define CLOCK_WISE 180
#define STOP 93
#define COUNTER_CLOCK_WISE 0

#include <Servo.h>

int initialDirection = 0;
int timeBallOpened = 0;
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
//  delay(2000);
}

void open_ball(){
  if(ball_is_open()){
    Serial.println("Already Opened");
    timeBallOpened = millis();
  } else {
  
    move_motor(initialDirection);
    delay(100);
    Serial.println("Open");
  
    int timeout = MAX_OPEN_CLOSE_TIME;
    
    while(!ball_is_open() && (timeout > 0)){
      Serial.println("Opening");
      timeout--;
      delay(1);
      if(ball_is_closed()){
        initialDirection != initialDirection;   
      }
    }
  
    motor_stop();
    delay(500); 
    if(ball_is_open()){
      timeBallOpened = millis();
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
        delay(1);
        if(ball_is_open()){
          initialDirection != initialDirection;   
        }
    }
    
    motor_stop();

    delay(250);  
    if(ball_is_closed()){
      Serial.println("Closed");
      delay(5000);
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

/*
  this will check if the ball is open and for how long, if there is no longer
  motion detected and the ball has been open for the set amount of time, it will
  return true to close the ball, otherwise it will return false
*/
bool should_close_ball(){
  int timeBallHasBeenOpened = millis() - timeBallOpened;

  if((pir_motion_detected() == false) &&
    (timeBallHasBeenOpened > MIN_TIME_FOR_OPEN_BALL)){
    Serial.println("slept for 1 second");
    return true;
  }
  return false;
}

// PIR methods
/*
  This will wait the calibration time, before allow motion to be detected
*/
void pir_init(){
  for(int i=0; i < PIR_CAL_TIME; i++){
    Serial.println("Calibrating PIR");
    delay(1000);
  }
  Serial.println("PIR Calibrated");
}

bool pir_motion_detected(){
  if(digitalRead(PIR_PIN) == HIGH){
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
  //initialize IO
  pinMode(PIR_PIN, INPUT);
  pinMode(CLOSE_PIN, INPUT);
  pinMode(OPEN_PIN, INPUT);

  servo.attach(10);
  pir_init();
  //initally close the ball
  close_ball();
}

int oldDistInCentimeters = 0;

void loop() {
      
  if(pir_motion_detected()){
    Serial.println("Detected Motion");
    open_ball();
  }

  if(should_close_ball()) {
    close_ball();
  }
}
