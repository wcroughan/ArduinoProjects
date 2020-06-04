
const int pin_1in1 = 5;
const int pin_1in2 = 6;
const int pin_1ena = 2; 
const int pin_2ena = 3;
const int pin_2in1 = 8; 
const int pin_2in2 = 9;

long unsigned stop_motor_time1 = 0;
bool should_stop1 = false;
long unsigned stop_motor_time2 = 0;
bool should_stop2 = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_1in1, OUTPUT);
  pinMode(pin_1in2, OUTPUT);
  pinMode(pin_1ena, OUTPUT);
  pinMode(pin_2in1, OUTPUT);
  pinMode(pin_2in2, OUTPUT);
  pinMode(pin_2ena, OUTPUT);

  analogWrite(pin_1ena, 0);
  analogWrite(pin_2ena, 0);

  digitalWrite(pin_1in1, LOW);
  digitalWrite(pin_1in2, LOW);
  digitalWrite(pin_2in1, LOW);
  digitalWrite(pin_2in2, LOW);

  Serial.begin(9600);
  Serial.println("connected!");
}

void loop() {
  // put your main code here, to run repeatedly:

  if (should_stop1 && stop_motor_time1 <= millis()) {
    should_stop1 = false;
    digitalWrite(pin_1in1, HIGH);
    digitalWrite(pin_1in2, HIGH);
    Serial.println("time's up, floating motor 1");
  }

  if (should_stop2 && stop_motor_time2 <= millis()) {
    should_stop2 = false;
    digitalWrite(pin_2in1, HIGH);
    digitalWrite(pin_2in2, HIGH);
    Serial.println("time's up, floating motor 2");
  }

  if (Serial.available()) {
    delay(3);
    String s_in = Serial.readString();
    if (s_in.equals("f1\n")) {
      //float 1
      digitalWrite(pin_1in1, HIGH);
      digitalWrite(pin_1in2, HIGH);
      Serial.println("floating motor 1");
    } else if (s_in.equals("f2\n")) {
      digitalWrite(pin_2in1, HIGH);
      digitalWrite(pin_2in2, HIGH);
      Serial.println("floating motor 2");
    } else if (s_in.equals("b1\n")) {
      //brake
      digitalWrite(pin_1in1, LOW);
      digitalWrite(pin_1in2, LOW);
      Serial.println("braking motor 1");
    } else if (s_in.equals("b2\n")) {
      digitalWrite(pin_2in1, LOW);
      digitalWrite(pin_2in2, LOW);
      Serial.println("braking motor 2");
    } else if (s_in.equals("g1\n")) {
      //go
      Serial.println("awaiting speed (0-255)");
      while (!Serial.available()) {}
      delay(3);
      String spd_in = Serial.readString();
      short speed = atoi(spd_in.c_str());
      Serial.println("awaiting duration (ms)");
      while (!Serial.available()) {}
      delay(3);
      String dur_in = Serial.readString();
      stop_motor_time1 = millis() + atoi(dur_in.c_str());
      should_stop1 = true;
      analogWrite(pin_1ena, speed);
      digitalWrite(pin_1in1, HIGH);
      digitalWrite(pin_1in2, LOW);
      Serial.println("Setting motor 1 forward");
    } else if (s_in.equals("g2\n")) {
      Serial.println("awaiting speed (0-255)");
      while (!Serial.available()) {}
      delay(3);
      String spd_in = Serial.readString();
      short speed = atoi(spd_in.c_str());
      Serial.println("awaiting duration (ms)");
      while (!Serial.available()) {}
      delay(3);
      String dur_in = Serial.readString();
      stop_motor_time2 = millis() + atoi(dur_in.c_str());
      should_stop2 = true;
      analogWrite(pin_2ena, speed);
      digitalWrite(pin_2in1, HIGH);
      digitalWrite(pin_2in2, LOW);
      Serial.println("Setting motor 2 forward");
    } else if (s_in.equals("r1\n")) {
      //reverse
      Serial.println("awaiting speed (0-255)");
      while (!Serial.available()) {}
      delay(3);
      String spd_in = Serial.readString();
      short speed = atoi(spd_in.c_str());
      Serial.println("awaiting duration (ms)");
      while (!Serial.available()) {}
      delay(3);
      String dur_in = Serial.readString();
      stop_motor_time1 = millis() + atoi(dur_in.c_str());
      should_stop1 = true;
      //reverse
      analogWrite(pin_1ena, speed);
      digitalWrite(pin_1in1, LOW);
      digitalWrite(pin_1in2, HIGH);
      Serial.println("Setting motor 1 backward");
    } else if (s_in.equals("r2\n")) {
      Serial.println("awaiting speed (0-255)");
      while (!Serial.available()) {}
      delay(3);
      String spd_in = Serial.readString();
      short speed = atoi(spd_in.c_str());
      Serial.println("awaiting duration (ms)");
      while (!Serial.available()) {}
      delay(3);
      String dur_in = Serial.readString();
      stop_motor_time2 = millis() + atoi(dur_in.c_str());
      should_stop2 = true;
      analogWrite(pin_2ena, speed);
      digitalWrite(pin_2in1, LOW);
      digitalWrite(pin_2in2, HIGH);
      Serial.println("Setting motor 2 backward");
    } else {
      Serial.println("unknown command:");
      Serial.println(s_in);
    }
  }

}
