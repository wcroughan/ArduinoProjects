#include <AccelStepper.h>

#define BAUD_RATE 9600
#define FREEZE_COMMAND 'F'
#define HOME_COMMAND 'H'
#define COORDS_COMMAND 'C'

bool waiting_on_coord = false;
long goal_coord = 0;
bool freeze_motor = false;

AccelStepper motor1 = AccelStepper(AccelStepper::FULL2WIRE, 2, 3);

const float MOTOR_MAX_SPEED = 1000.0;
const float MOTOR_ACCELERATION = 1000.0;

void setup()
{
    Serial.begin(BAUD_RATE);

    motor1.setMaxSpeed(MOTOR_MAX_SPEED);
    motor1.setAcceleration(MOTOR_ACCELERATION);
}

void loop()
{
    //handle serial
    while (Serial.available())
    {
        if (waiting_on_coord)
        {
            Serial.println("WOC");
            String s_in = Serial.readString();
            Serial.println(s_in);
            goal_coord = atoi(s_in.c_str());
            motor1.moveTo(goal_coord);
            waiting_on_coord = false;
        }
        else
        {
            char c_in = Serial.read();
            Serial.println("char:");
            Serial.println(c_in);
            switch (c_in)
            {
            case FREEZE_COMMAND:
                freeze_motor = !freeze_motor;
                Serial.println("(un)freezing");
                break;
            case HOME_COMMAND:
                Serial.println("homing");
                motor1.moveTo(0);
                break;
            case COORDS_COMMAND:
                delay(3); //let buffer fill
                waiting_on_coord = true;
                break;
            }
            Serial.read(); //get rid of newline
        }
    }

    //handle motors
    if (!freeze_motor)
    {
        motor1.run();
    }
}
