#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

const int pina = 2;
const int pinb = 18;
Encoder myEnc(pina, pinb);

const int motor_pin_fwd = 44;
const int motor_pin_rev = 46;
const int braking_pin = 45;

long goal = 0;
long pos = 0;
long oldPosition = -999;
bool dir_forward

const int brakingDistance = 4; //Can adjust this based on distance travelled to reflect gaining momentum
const short motor_speed = 255;

void setup() {
  // put your setup code here, to run once:
  pinMode(motor_pin_fwd, OUTPUT);
  analogWrite(motor_pin_fwd, 0);
  pinMode(motor_pin_rev, OUTPUT);
  analogWrite(motor_pin_rev, 0);
  pinMode(braking_pin, OUTPUT);
  digitalWrite(braking_pin, LOW);

  Serial.begin(9600);
  Serial.println("Hello!");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    delay(3);
    String s_in = Serial.readString();
    goal = s_in.toInt();
    dir_forward = goal > pos;

    if (abs(goal - pos) > brakingDistance) {
      digitalWrite(braking_pin, LOW);
      analogWrite(dir_forward ? motor_pin_fwd : motor_pin_rev, motor_speed);
    }
  }

  pos = myEnc.read();
  if (pos != oldPosition) {
    oldPosition = pos;
    if ((dir_forward && pos > goal - brakingDistance) ||
        (!dir_forward && pos < goal + brakingDistance)) {
      //stop motor
      analogWrite(motor_pin_fwd, 0);
      analogWrite(motor_pin_rev, 0);
      digitalWrite(braking_pin, HIGH);
    }
  }
  
  
}
