#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define SEND_CONFIRMATION_MESSAGES 1
#if SEND_CONFIRMATION_MESSAGES
#define CONFIRM(msg) Serial.println(msg)
#else
#define CONFIRM(msg)
#endif

//========================
// Pin numbers
//========================
//TODO these pins are random numbers right now
const int xpina = 0;
const int xpinb = 0;
const int ypina = 0;
const int ypinb = 0;

const int motor_pin_1in1 = 0;
const int motor_pin_1in2 = 0;
const int motor_pin_1ena = 0;
const int motor_pin_2ena = 0;
const int motor_pin_2in1 = 0;
const int motor_pin_2in2 = 0;

//========================
// Position encoding
//========================
Encoder xEnc(xpina, xpinb);
Encoder yEnc(ypina, ypinb);
long xposition = 0;
long yposition = 0;

//========================
// Communication constants
//========================
const int8_t CMD_F1 = 0;            //float 1
const int8_t CMD_F2 = 1;            //float 2
const int8_t CMD_B1 = 2;            //brake 1
const int8_t CMD_B2 = 3;            //brake 2
const int8_t CMD_G1 = 4;            //go 1
const int8_t CMD_G2 = 5;            //go 2
const int8_t CMD_R1 = 6;            //reverse 1
const int8_t CMD_R2 = 7;            //reverseo 2
const int8_t CMD_READY_FOR_POS = 8; //send next position

bool should_send_pos = false;

void setup()
{
    pinMode(motor_pin_1in1, OUTPUT);
    pinMode(motor_pin_1in2, OUTPUT);
    pinMode(motor_pin_1ena, OUTPUT);
    pinMode(motor_pin_2in1, OUTPUT);
    pinMode(motor_pin_2in2, OUTPUT);
    pinMode(motor_pin_2ena, OUTPUT);

    analogWrite(motor_pin_1ena, 0);
    analogWrite(motor_pin_2ena, 0);

    digitalWrite(motor_pin_1in1, LOW);
    digitalWrite(motor_pin_1in2, LOW);
    digitalWrite(motor_pin_2in1, LOW);
    digitalWrite(motor_pin_2in2, LOW);

    Serial.begin(9600);
    //    Serial.println("Hello from motor controller!");
    should_send_pos = false;
}

void loop()
{
    long xpos = xEnc.read();
    long ypos = yEnc.read();
    bool pos_changed = true;

    if (xpos != xposition)
    {
        xposition = xpos;
        pos_changed = true;
    }
    if (ypos != yposition)
    {
        yposition = ypos;
        pos_changed = true;
    }

    if (should_send_pos)
    {
        xposition = 2;
        yposition = -4;
        Serial.write(0);
        Serial.write((byte)(xposition & 0xFF));
        Serial.write((byte)((xposition >> 8) & 0xFF));
        Serial.write((byte)((xposition >> 16) & 0xFF));
        Serial.write((byte)((xposition >> 24) & 0xFF));
        Serial.write((byte)(yposition & 0xFF));
        Serial.write((byte)((yposition >> 8) & 0xFF));
        Serial.write((byte)((yposition >> 16) & 0xFF));
        Serial.write((byte)((yposition >> 24) & 0xFF));
        should_send_pos = false;
    }

    if (Serial.available())
    {
        delay(3);
        int8_t cmd = Serial.read();
        if (cmd == CMD_F1)
        {
            //float 1
            digitalWrite(motor_pin_1in1, HIGH);
            digitalWrite(motor_pin_1in2, HIGH);
            CONFIRM("floating motor 1");
        }
        else if (cmd == CMD_F2)
        {
            digitalWrite(motor_pin_2in1, HIGH);
            digitalWrite(motor_pin_2in2, HIGH);
            CONFIRM("floating motor 2");
        }
        else if (cmd == CMD_B1)
        {
            //brake
            digitalWrite(motor_pin_1in1, LOW);
            digitalWrite(motor_pin_1in2, LOW);
            CONFIRM("braking motor 1");
        }
        else if (cmd == CMD_B2)
        {
            digitalWrite(motor_pin_2in1, LOW);
            digitalWrite(motor_pin_2in2, LOW);
            CONFIRM("braking motor 2");
        }
        else if (cmd == CMD_G1)
        {
            //go
            int8_t speed = Serial.read();
            analogWrite(motor_pin_1ena, speed);
            digitalWrite(motor_pin_1in1, HIGH);
            digitalWrite(motor_pin_1in2, LOW);
            CONFIRM("Setting motor 1 forward");
            CONFIRM(speed);
        }
        else if (cmd == CMD_G2)
        {
            int8_t speed = Serial.read();
            analogWrite(motor_pin_2ena, speed);
            digitalWrite(motor_pin_2in1, HIGH);
            digitalWrite(motor_pin_2in2, LOW);
            CONFIRM("Setting motor 2 forward");
            CONFIRM(speed);
        }
        else if (cmd == CMD_R1)
        {
            //reverse
            int8_t speed = Serial.read();
            analogWrite(motor_pin_1ena, speed);
            digitalWrite(motor_pin_1in1, LOW);
            digitalWrite(motor_pin_1in2, HIGH);
            CONFIRM("Setting motor 1 backward");
            CONFIRM(speed);
        }
        else if (cmd == CMD_R2)
        {
            int8_t speed = Serial.read();
            analogWrite(motor_pin_2ena, speed);
            digitalWrite(motor_pin_2in1, LOW);
            digitalWrite(motor_pin_2in2, HIGH);
            CONFIRM("Setting motor 2 backward");
            CONFIRM(speed);
        }
        else if (cmd == CMD_READY_FOR_POS)
        {
            should_send_pos = true;
            CONFIRM("Sending pos soon...");
        }
        else
        {
            Serial.println("unknown command");
            Serial.println(cmd);
        }
    }
}
