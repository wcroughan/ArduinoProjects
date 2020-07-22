// #define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define SEND_CONFIRMATION_MESSAGES 1
#if SEND_CONFIRMATION_MESSAGES
#define CONFIRM(msg) Serial.println(msg)
#else
#define CONFIRM(msg)
#endif

#define X_COORD_IS_M1 0

#define WDC_USE_MEGA_PINS 1
//========================
// Pin numbers
//========================
#if WDC_USE_MEGA_PINS
const int ypina = 2;
const int ypinb = 3;

const int motor_pin_1in1 = 5;
const int motor_pin_1in2 = 6;
const int motor_pin_1ena = 11;
const int motor_pin_2ena = 12;
const int motor_pin_2in1 = 8;
const int motor_pin_2in2 = 9;

const int edge_pin_y1 = 19;
const int edge_pin_y2 = 21;
#else
const int ypina = 2;
const int ypinb = 12;

const int motor_pin_1in1 = 5;
const int motor_pin_1in2 = 6;
const int motor_pin_1ena = 9;
const int motor_pin_2ena = 11;
const int motor_pin_2in1 = 7;
const int motor_pin_2in2 = 8;

const int edge_pin_y1 = 3;
// const int edge_pin_y2 = 21;
#endif

//========================
// Position encoding
//========================
// Encoder xEnc(xpina, xpinb);
Encoder yEnc(ypina, ypinb);
long xposition = 0;
long yposition = 0;

//========================
// Communication constants
//========================
const int8_t CMD_F1 = 0; //float 1
const int8_t CMD_F2 = 1; //float 2
const int8_t CMD_B1 = 2; //brake 1
const int8_t CMD_B2 = 3; //brake 2
const int8_t CMD_G1 = 4; //go 1
const int8_t CMD_G2 = 5; //go 2
const int8_t CMD_R1 = 6; //reverse 1
const int8_t CMD_R2 = 7; //reverse 2
const int8_t CMD_READY_FOR_POS = 8;
const int8_t CMD_RESET_EDGE_DETECTION = 9;
const int8_t CMD_CALIBRATE_HOME = 10;
const int8_t CMD_GO_HOME = 11;
const int8_t CMD_QUERY_RANGES = 12;

//========================
// State saving globals
//========================
bool should_send_pos = false;
bool should_reset_led = true;
bool edge_detect_flag_y1 = false;
bool edge_detect_flag_y2 = false;
bool edge_detect_flag_x1 = false;
bool edge_detect_flag_x2 = false;
bool last_move_was_forward_m1 = false;
bool last_move_was_forward_m2 = false;
long unsigned int interrupt_count_y1 = 0;
long unsigned int interrupt_count_y2 = 0;
long unsigned int interrupt_count_x1 = 0;
long unsigned int interrupt_count_x2 = 0;
int8_t ignore_edge_detect = 0;

int calibrating_step = 0;
const int CALIBRATING_STEP_GO_TO_MIN = 1;
const int CALIBRATING_STEP_FIND_MAX = 2;
const int CALIBRATING_STEP_FIND_MIN = 3;
long motion_range_y = 0;
long motion_range_x = 0;
const int8_t CALIBRATING_SPEED = 50;

// void edge_detect_isr_m2_1()
// {
//     if (ignore_edge_detect)
//         return;

//     //TODO whatever edge detect stuff you want here
//     digitalWrite(LED_BUILTIN, HIGH);
//     edge_detect_flag_m2 = true;
//     interrupt_count_m2_1++;

//     //Brakes!
//     digitalWrite(motor_pin_2in1, LOW);
//     digitalWrite(motor_pin_2in2, LOW);
// }

void edge_detect_isr_y1()
{
    if (ignore_edge_detect)
        return;
    //TODO whatever edge detect stuff you want here
    digitalWrite(LED_BUILTIN, HIGH);
    edge_detect_flag_y1 = true;
    interrupt_count_y1++;

//Brakes!
#if X_COORD_IS_M1
    if (!last_move_was_forward_m2)
    {
        digitalWrite(motor_pin_2in1, LOW);
        digitalWrite(motor_pin_2in2, LOW);
    }
#else
    if (!last_move_was_forward_m1)
    {
        digitalWrite(motor_pin_1in1, LOW);
        digitalWrite(motor_pin_1in2, LOW);
    }
#endif

    if (calibrating_step == CALIBRATING_STEP_GO_TO_MIN)
    {
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        calibrating_step = CALIBRATING_STEP_FIND_MAX;
#if X_COORD_IS_M1
        last_move_was_forward_m2 = true;
        digitalWrite(motor_pin_2in1, HIGH);
        digitalWrite(motor_pin_2in2, LOW);
#else
        last_move_was_forward_m1 = true;
        digitalWrite(motor_pin_1in1, HIGH);
        digitalWrite(motor_pin_1in2, LOW);
#endif
    }
    else if (calibrating_step == CALIBRATING_STEP_FIND_MIN)
    {
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        motion_range_y = yEnc.read() - motion_range_y;
        yEnc.write(motion_range_y);
#if X_COORD_IS_M1
        last_move_was_forward_m2 = true;
        digitalWrite(motor_pin_2in1, HIGH);
        digitalWrite(motor_pin_2in2, LOW);
#else
        last_move_was_forward_m1 = true;
        digitalWrite(motor_pin_1in1, HIGH);
        digitalWrite(motor_pin_1in2, LOW);
#endif
        delay(500);
#if X_COORD_IS_M1
        digitalWrite(motor_pin_2in1, LOW);
        digitalWrite(motor_pin_2in2, LOW);
#else
        digitalWrite(motor_pin_1in1, LOW);
        digitalWrite(motor_pin_1in2, LOW);
#endif
    }
}

void edge_detect_isr_y2()
{
    if (ignore_edge_detect)
        return;
    //TODO whatever edge detect stuff you want here
    digitalWrite(LED_BUILTIN, HIGH);
    edge_detect_flag_y2 = true;
    interrupt_count_y2++;

//Brakes!
#if X_COORD_IS_M1
    if (last_move_was_forward_m2)
    {
        digitalWrite(motor_pin_2in1, LOW);
        digitalWrite(motor_pin_2in2, LOW);
    }
#else
    if (last_move_was_forward_m1)
    {
        digitalWrite(motor_pin_1in1, LOW);
        digitalWrite(motor_pin_1in2, LOW);
    }
#endif

    if (calibrating_step == CALIBRATING_STEP_FIND_MAX)
    {
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        calibrating_step = CALIBRATING_STEP_FIND_MIN;
#if X_COORD_IS_M1
        last_move_was_forward_m2 = false;
#else
        last_move_was_forward_m1 = false;
#endif
        motion_range_y = yEnc.read();
        digitalWrite(motor_pin_1in1, LOW);
        digitalWrite(motor_pin_1in2, HIGH);
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    //UGH for some reason this isn't working unless it's in the loop() function
    // digitalWrite(LED_BUILTIN, LOW);
    pinMode(edge_pin_y1, INPUT);
    attachInterrupt(digitalPinToInterrupt(edge_pin_y1), edge_detect_isr_y1, CHANGE);
    pinMode(edge_pin_y2, INPUT);
    attachInterrupt(digitalPinToInterrupt(edge_pin_y2), edge_detect_isr_y2, CHANGE);

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
    if (should_reset_led)
    {
        should_reset_led = false;
        digitalWrite(LED_BUILTIN, LOW);
    }

    // long xpos = xEnc.read();
    long xpos = 0;
    long ypos = yEnc.read();
    bool pos_changed = false;

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
            ignore_edge_detect = false;
            digitalWrite(motor_pin_1in1, HIGH);
            digitalWrite(motor_pin_1in2, HIGH);
            CONFIRM("floating motor 1");
        }

        else if (cmd == CMD_F2)
        {
            ignore_edge_detect = false;
            digitalWrite(motor_pin_2in1, HIGH);
            digitalWrite(motor_pin_2in2, HIGH);
            CONFIRM("floating motor 2");
        }

        else if (cmd == CMD_B1)
        {
            //brake
            ignore_edge_detect = false;
            digitalWrite(motor_pin_1in1, LOW);
            digitalWrite(motor_pin_1in2, LOW);
            CONFIRM("braking motor 1");
        }

        else if (cmd == CMD_B2)
        {
            ignore_edge_detect = false;
            digitalWrite(motor_pin_2in1, LOW);
            digitalWrite(motor_pin_2in2, LOW);
            CONFIRM("braking motor 2");
        }

        else if (cmd == CMD_G1)
        {
            //go
            int8_t speed = Serial.read();
            ignore_edge_detect = Serial.read();
#if X_COORD_IS_M1
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_x2;
#else
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_y2;
#endif
            if (edge_conflict)
            {
                CONFIRM("Cannot move again, edge detected!");
            }
            else
            {
                last_move_was_forward_m1 = true;
                analogWrite(motor_pin_1ena, speed);
                digitalWrite(motor_pin_1in1, HIGH);
                digitalWrite(motor_pin_1in2, LOW);
                CONFIRM("Setting motor 1 forward");
                CONFIRM(int(speed) & 0xFF);
            }
        }

        else if (cmd == CMD_G2)
        {
            int8_t speed = Serial.read();
            ignore_edge_detect = Serial.read();
#if X_COORD_IS_M1
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_y2;
#else
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_x2;
#endif
            if (edge_conflict)
            {
                CONFIRM("Cannot move again, edge detected!");
            }
            else
            {
                last_move_was_forward_m2 = true;
                analogWrite(motor_pin_2ena, speed);
                digitalWrite(motor_pin_2in1, HIGH);
                digitalWrite(motor_pin_2in2, LOW);
                CONFIRM("Setting motor 2 forward");
                CONFIRM(int(speed) & 0xFF);
            }
        }

        else if (cmd == CMD_R1)
        {
            //reverse
            int8_t speed = Serial.read();
            ignore_edge_detect = Serial.read();
#if X_COORD_IS_M1
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_x1;
#else
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_y1;
#endif
            if (edge_conflict)
            {
                CONFIRM("Cannot move again, edge detected!");
            }
            else
            {
                last_move_was_forward_m1 = false;
                analogWrite(motor_pin_1ena, speed);
                digitalWrite(motor_pin_1in1, LOW);
                digitalWrite(motor_pin_1in2, HIGH);
                CONFIRM("Setting motor 1 backward");
                CONFIRM(int(speed) & 0xFF);
            }
        }

        else if (cmd == CMD_R2)
        {
            int8_t speed = Serial.read();
            ignore_edge_detect = Serial.read();
#if X_COORD_IS_M1
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_y1;
#else
            bool edge_conflict = !ignore_edge_detect && edge_detect_flag_x1;
#endif
            if (edge_conflict)
            {
                CONFIRM("Cannot move again, edge detected!");
            }
            else
            {
                last_move_was_forward_m2 = false;
                analogWrite(motor_pin_2ena, speed);
                digitalWrite(motor_pin_2in1, LOW);
                digitalWrite(motor_pin_2in2, HIGH);
                CONFIRM("Setting motor 2 backward");
                CONFIRM(int(speed) & 0xFF);
            }
        }

        else if (cmd == CMD_READY_FOR_POS)
        {
            should_send_pos = true;
            CONFIRM("Sending pos soon...");
        }

        else if (cmd == CMD_RESET_EDGE_DETECTION)
        {
            ignore_edge_detect = false;
            digitalWrite(LED_BUILTIN, LOW);
            CONFIRM("Resetting edge detector");
            CONFIRM(interrupt_count_y1);
            CONFIRM(interrupt_count_y2);
            CONFIRM(edge_detect_flag_y1);
            CONFIRM(edge_detect_flag_y2);
            edge_detect_flag_y1 = false;
            edge_detect_flag_y2 = false;
        }

        else if (cmd == CMD_CALIBRATE_HOME)
        {
            //NOTE: right now only calibrates motor 1
            CONFIRM("calibrating motor 1");
            last_move_was_forward_m1 = false;
            ignore_edge_detect = 0;
            analogWrite(motor_pin_1ena, CALIBRATING_SPEED);
            digitalWrite(motor_pin_1in1, LOW);
            digitalWrite(motor_pin_1in2, HIGH);
            calibrating_step = CALIBRATING_STEP_GO_TO_MIN;
        }

        else if (cmd == CMD_GO_HOME)
        {
            CONFIRM("unimplemented");
        }

        else if (cmd == CMD_QUERY_RANGES)
        {
            CONFIRM("Ranges:");
            CONFIRM(motion_range_x);
            CONFIRM(motion_range_y);
        }

        else
        {
            CONFIRM("unknown command");
            CONFIRM(cmd);
        }
    }
}
