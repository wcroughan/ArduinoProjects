const int NUM_ROWS = 6;
const int NUM_COLS = 6;
const int NUM_WELLS = NUM_ROWS * NUM_COLS;

#define SEND_CONFIRMATION_MESSAGES 1
#if SEND_CONFIRMATION_MESSAGES
#define CONFIRM(msg) Serial.println(msg)
#else
#define CONFIRM(msg)
#endif

const inline int get_well_idx(int row, int col)
{
    return row * NUM_COLS + col;
}

int8_t *well_pins;
bool *smooth_well_state;
long *well_switch_times;

int8_t *pump_pins;

const int8_t CMD_SEND_WELL_STATE = 1;
const int8_t CMD_ACTIVATE_PUMP = 2;
const int8_t CMD_DEACTIVATE_PUMP = 3;

const long MIN_DURATION_SWITCH_STATE = 100; //units: ms

void setup()
{
    well_pins = new int8_t[NUM_WELLS];
    pump_pins = new int8_t[NUM_WELLS];
    smooth_well_state = new bool[NUM_WELLS];
    well_switch_times = new long[NUM_WELLS];

    // put your setup code here, to run once:
    //TODO assign pin numbers
    int8_t first_well_pin = 2;
    int8_t first_pump_pin = 20;
    for (int i = 0; i < NUM_WELLS; i++)
    {
        well_pins[i] = first_well_pin++;
        pinMode(well_pins[i], INPUT);
        pump_pins[i] = first_pump_pin++;
        pinMode(pump_pins[i], OUTPUT);
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
    for (int i = 0; i < NUM_WELLS; i++)
    {
        int state = digitalRead(well_pins[i]);
        if (state == smooth_well_state[i])
        {
            well_switch_times[i] = 0;
        }
        else
        {
            //well is in different state that smooth state says
            if (well_switch_times[i] == 0)
            {
                //this is first recording of different state, set well_switch_times
                well_switch_times[i] = millis();
            }
            else
            {
                //been switched, check duration
                if (millis() - well_switch_times[i] > MIN_DURATION_SWITCH_STATE)
                {
                    well_switch_times[i] = 0;
                    smooth_well_state[i] = state;
                }
            }
        }
    }

    if (Serial.available())
    {
        delay(3);
        int8_t cmd = Serial.read();
        if (cmd == CMD_SEND_WELL_STATE)
        {
            int8_t wi = Serial.read();
            Serial.println(smooth_well_state[wi]);
        }

        else if (cmd == CMD_ACTIVATE_PUMP)
        {
            int8_t pi = Serial.read();
            CONFIRM("Activating pump");
            digitalWrite(pump_pins[pi], HIGH);
        }

        else if (cmd == CMD_DEACTIVATE_PUMP)
        {
            int8_t pi = Serial.read();
            CONFIRM("Deactivating pump");
            digitalWrite(pump_pins[pi], LOW);
        }
    }
}
