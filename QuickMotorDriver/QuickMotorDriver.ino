const int dir_pin = 3;
const int step_pin = 2;
const int min_d = 25;
const int start_d = 70;
const int sig = HIGH;

const double RAMP_SPEED = 0.001;

double dd;
int d;

void setup() {
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
  digitalWrite(dir_pin, LOW);

  d = start_d;
  dd = start_d;
}

void loop() {
  digitalWrite(step_pin, HIGH);
  //delayMicroseconds(20);
  delayMicroseconds(d);
  digitalWrite(step_pin, sig);
  //delayMicroseconds(20);
  delayMicroseconds(d);

  if (dd > min_d)
    dd -= RAMP_SPEED;
  d = int(dd);
}
