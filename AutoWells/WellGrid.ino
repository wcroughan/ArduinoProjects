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

long next_well_check_millis = 0;
const long MILLIS_BETWEEN_CHECKS = 500;

void setup()
{
    next_well_check_millis = millis();
}

void loop()
{
    long dtime = next_well_check_millis - millis();
    delay(dtime);
    next_well_check_millis = millis() + MILLIS_BETWEEN_CHECKS;

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
