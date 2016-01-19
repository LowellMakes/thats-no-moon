
// thats no moon

#define TRIGGER_PIN 6
#define ECHO_PIN 7

#define FAR_LIMIT 200

#define FORWARD_PIN 3
#define REVERSE_PIN 9

#define SPEED 50

#define OPEN_TIME (6 * 1000)
#define CLOSE_TIME (6 * 1000)

void motor_forward(){
  analogWrite(FORWARD_PIN, SPEED);
  analogWrite(REVERSE_PIN, 0);
}

void motor_reverse(){
  analogWrite(FORWARD_PIN, 0);
  analogWrite(REVERSE_PIN, SPEED);  
}

void motor_stop(){
  analogWrite(FORWARD_PIN, 0);
  analogWrite(REVERSE_PIN, 0);  
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
    motor_forward();
    delay(OPEN_TIME);
    motor_stop();
    ball_open = true;
  }
    
}

void close_ball(){
  //ball_open=false;
  if(ball_open){
    motor_reverse();
    delay(CLOSE_TIME);
    motor_stop();
    ball_open = false;
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(REVERSE_PIN, OUTPUT);
}

void loop() {
  long duration = 0;
  int distInCentimeters = 0;

  Serial.println("Trigger");
  
  trigger();
  duration = pulseIn(ECHO_PIN, HIGH);
  Serial.print("Duration = ");
  Serial.print(duration);
  Serial.println();
  distInCentimeters = microsecondsToCentimeters(duration);
  
  // could be stuck, run the work around just in case.
  if ( distInCentimeters == 0 ) {
      delay(100);
      pinMode(ECHO_PIN, OUTPUT);
      digitalWrite(ECHO_PIN, LOW);
      delay(100);
      pinMode(ECHO_PIN, INPUT);
  } else {
    Serial.print("cm = ");
    Serial.print(distInCentimeters);
    Serial.println();
  
    if(distInCentimeters < FAR_LIMIT){
      open_ball();
    }
    else {
      close_ball();
    } 
  }
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
