void setup() {
  // put your setup code here, to run once:

    Serial.begin(9600);
    Serial.println("Hello from motor controller!");
}

unsigned long last_output_time = 0;

void loop() {
  // put your main code here, to run repeatedly:

    if (Serial.available())
    {
        delay(3);
        unsigned long t1 = millis();
        int8_t in = Serial.read();
        Serial.println(in);
    }
}
