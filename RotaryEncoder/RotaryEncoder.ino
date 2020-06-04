#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

const int pina = 2;
const int pinb = 18;
Encoder myEnc(pina, pinb);

const int ana_pin = 44;

void setup() {
  pinMode(ana_pin, OUTPUT);
  analogWrite(ana_pin, 0);
  
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

long oldPosition  = -999;

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    short outval = abs(newPosition % 255);
    analogWrite(ana_pin, outval);
//    digitalWrite(ana_pin, outval > 10 ? HIGH : LOW);
    Serial.println(outval);
  }
}
