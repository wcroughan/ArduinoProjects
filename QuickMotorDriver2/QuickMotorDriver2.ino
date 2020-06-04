const int dir_pin = 8;
const int step_pin = 5;
const int enable_pin = 7;
const int d = 10;
//const int d = 35;
//const int d = 800;
const int sig = LOW;

void setup() {
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
  pinMode(enable_pin, OUTPUT);
  
  digitalWrite(dir_pin, LOW);
  digitalWrite(enable_pin, LOW);
}

void loop() {
  digitalWrite(step_pin, HIGH);
  //delayMicroseconds(20);
  delayMicroseconds(d);
  digitalWrite(step_pin, sig);
  //delayMicroseconds(20);
  delayMicroseconds(d);
}
