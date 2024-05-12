// Code in this file is based off code by Kurtis Laqua; see https://github.com/sitruklaq/Hattonlab-Pneumatics/tree/master/Arduino%20Code
// Kurtis's code used this tutorial: https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
#ifndef minimal_pneumatics_h
#define minimal_pneumatics_h

#include "Arduino.h"

// define constants for pump state values
const int PUMP_ON = 1;
const int PUMP_OFF = 0;
const int DEFAULT_DUTY_CYCLE = 130; // set default at about 50% in case of accidentally turning pump on

// define constants for valve state values
const int VALVE_OPEN = 1;
const int VALVE_CLOSED = 0;

// define constants for pump pin assignments and pin indices
const int NUM_PUMPS = 2;
const int NUM_PUMP_PINS = 4;
const int PUMP_PINS[NUM_PUMP_PINS] = {8,10,9,11}; //last two are used as ground pins
enum pumps{
    NEG = 0,
    POS
};

// define constants for input valve pin assignments and pin indices
const int NUM_IN_VALVES = 3;
const int IN_VALVE_PINS[NUM_IN_VALVES] = {53, 52, 51};
enum input_valves{
    INV_NEG = 0,
    INV_NEU,
    INV_POS
};

// define constants for output valve pin assignments and pin indices
const int NUM_OUT_VALVES = 8;
const int OUT_VALVE_PINS[NUM_OUT_VALVES] = {47, 46, 45, 44, 31, 30, 29, 28};
//const int ovalve[8] = {28, 47, 29, 46,30,45,31,44}; // this pin arrangement is for the 8-channel device only
const int VALVE_O1_IND = 0;
const int VALVE_O2_IND = 1;
const int VALVE_O3_IND = 2;
const int VALVE_O4_IND = 3;
const int VALVE_O5_IND = 4;
const int VALVE_O6_IND = 5;
const int VALVE_O7_IND = 6;
const int VALVE_O8_IND = 7;
enum output_valves{
    OV1 = 0,
    OV2,
    OV3,
    OV4,
    OV5,
    OV6,
    OV7,
    OV8
};

// define constants for input sensor pin assignments and pin indices
const int NUM_IN_SENSORS = 2;
const int IN_SENSOR_PINS[NUM_IN_SENSORS] = {A14,A15};
enum input_sensors{
    INS_NEG = 0,
    INS_POS
};

// define constants for output sensor pin assignments and pin indices
const int NUM_OUT_SENSORS = 8;
const int OUT_SENSOR_PINS[NUM_OUT_SENSORS] = {A13,A12,A11,A10,A9,A9,A9,A9}; // TODO: fix last 3
enum output_sensors{
    OS1 = 0,
    OS2,
    OS3,
    OS4,
    OS5,
    OS6,
    OS7,
    OS8
};

// define pressure sensor calibration parameters
const float CALIBRATION_SCALE = 50.f;
const float IN_CALIBRATION_OFFSETS[NUM_IN_SENSORS] = {2.519,2.512}; // in V
const float OUT_CALIBRATION_OFFSETS[NUM_OUT_SENSORS] = {2.516,2.5,2.5,2.5,2.5,2.5,2.5,2.5}; // in V

// define serial command string format
namespace serial_command{
    // define serial markers and max length
    const char START_MARKER = '<';
    const char END_MARKER = '>';
    const byte MAX_INPUT_CHARS = 18;

    // define constant char arrays for text commands
    // (not used directly in code here; provided for reference and clarity)
    const char SET_SINGLE_IN_VALVE[] = "SI";    // command format: <SI, valve #, valve state>
    const char SET_SINGLE_OUT_VALVE[] = "SO";   // command format: <SO, valve #, valve state>
    const char SET_ALL_IN_VALVES[] = "AI";      // command format: <AI, 999, valve state>
    const char SET_ALL_OUT_VALVES[] = "AO";     // command format: <AO, 999, valve state>
    const char GET_IN_PRESSURE[] = "GI";        // command format: <GI, sensor #, 999>
    const char GET_OUT_PRESSURE[] = "GO";       // command format: <GO, sensor #, 999>
    const char GET_IN_VALVE_STATE[] = "VI";     // command format: <VI, valve #, 999>
    const char GET_OUT_VALVE_STATE[] = "VO";    // command format: <VO, valve #, 999>
    const char SET_REF_SETPOINT[] = "RS";       // command format: <RS, pump #, pump setpoint>
    const char GET_REF_SETPOINT[] = "RG";       // command format: <RG, pump #, 999>
    const char SET_PUMP_STATE[] = "PS";         // command format: <PS, pump #, pump state>
    const char GET_PUMP_STATE[] = "PG";         // command format: <PG, pump #, 999>

    // define integers for first letter of serial commands
    // (used to determine action based on command)
    const int SINGLE_VALVE_PREFIX = 'S';        // should match first letter of SET_SINGLE_IN_VALVE & SET_SINGLE_OUT_VALVE
    const int ALL_VALVES_PREFIX = 'A';          // should match first letter of SET_ALL_IN_VALVES & SET_ALL_OUT_VALVES
    const int GET_PRESSURE_PREFIX = 'G';        // should match first letter of GET_IN_PRESSURE & GET_OUT_PRESSURE
    const int GET_VALVE_STATE_PREFIX = 'V';     // should match first letter of GET_IN_VALVE_STATE & GET_OUT_VALVE_STATE
    const int REF_SETPOINT_PREFIX = 'R';        // should match first letter of SET_REF_SETPOINT & GET_REF_SETPOINT
    const int PUMP_STATE_PREFIX = 'P';          // should match first letter of SET_PUMP_STATE & GET_PUMP_STATE

    // define integers for second letter of serial commands
    // (used to determine whether action is taken on input or output channels for valves and sensors
    //  OR to determine whether state/setpoint is changed or set for pumps)
    const int INPUT_SUFFIX = 'I';               // should match second letter of valve and pressure sensor commands
    const int OUTPUT_SUFFIX = 'O';              // should match second letter of valve and pressure sensor commands
    const int SET_SUFFIX = 'S';                 // should match second letter of pump and reference setpoint commands
    const int GET_SUFFIX = 'G';                 // should match second letter of pump and reference setpoint commands
} //namespace serial_command

// define (global) variables for serial communication
bool newSerialInputReady = false;
char receivedChars[serial_command::MAX_INPUT_CHARS];
char serialCommand[serial_command::MAX_INPUT_CHARS] = {0};
int serialIndex = 0;
int serialValue = 0;

// define variables to hold valve and pump states
int pumpStates[NUM_PUMPS] = {PUMP_OFF, PUMP_OFF};
int inputValveStates[NUM_IN_VALVES];
int outputValveStates[NUM_OUT_VALVES];

// define variables to hold sensor pressure readings and pump setpoint values
int inputPressureRawReadings[NUM_IN_SENSORS];
int outputPressureRawReadings[NUM_OUT_SENSORS];
float inputPressureValsCurrent[NUM_IN_SENSORS];
float outputPressureValsCurrent[NUM_OUT_SENSORS];
float inputPressureValsAverage[NUM_IN_SENSORS];
float outputPressureValsAverage[NUM_OUT_SENSORS];
int pumpSetpoints[NUM_PUMPS] = {0, 0};

//----ARDUINO INITIALIZATION
void initialize_pins() {
    // set up pump pins as Arduino outputs
    for (int i = 0; i < NUM_PUMP_PINS; i++) {
        pinMode(PUMP_PINS[i], OUTPUT);
        digitalWrite(PUMP_PINS[i], PUMP_OFF);
    }

    // set up valve pins as Arduino outputs
    for (int i = 0; i < NUM_IN_VALVES; i++) {
        pinMode(IN_VALVE_PINS[i], OUTPUT);
        digitalWrite(IN_VALVE_PINS[i], VALVE_CLOSED);
    }
    for (int i = 0; i < NUM_OUT_VALVES; i++) {
        pinMode(OUT_VALVE_PINS[i], OUTPUT);
        digitalWrite(OUT_VALVE_PINS[i], VALVE_CLOSED);
    }

    // set up pressure sensor pins as Arduino inputs
    for (int i = 0; i < NUM_IN_SENSORS ; i++) {
        pinMode(IN_SENSOR_PINS[i], INPUT);
    }
    for (int i = 0; i < NUM_OUT_SENSORS ; i++) {
        pinMode(OUT_SENSOR_PINS[i], INPUT);
    }
}
//---------------

//---VALVE CONTROL FUNCTIONS
void set_invalve_all(const int next_valve_state) {
    for (int i = 0; i < NUM_IN_VALVES; i++) {
      digitalWrite(IN_VALVE_PINS[i], next_valve_state);
      inputValveStates[i] = next_valve_state;
    }
}
void set_invalve_single(const int valve_ind, const int next_valve_state) {
    digitalWrite(IN_VALVE_PINS[valve_ind], next_valve_state);
    inputValveStates[valve_ind] = next_valve_state;
}
void set_outvalve_all(const int next_valve_state) {
    for (int i = 0; i < NUM_OUT_VALVES; i++) {
      digitalWrite(OUT_VALVE_PINS[i], next_valve_state);
      outputValveStates[i] = next_valve_state;
    }
}
void set_outvalve_single(const int valve_ind, const int next_valve_state) {
    digitalWrite(OUT_VALVE_PINS[valve_ind], next_valve_state);
    outputValveStates[valve_ind] = next_valve_state;
}
//-------

//---PRESSURE SENSOR INPUTS AND CALIBRATION
float calibrate_sensor_reading(int raw_value,float calibration_offset){
    float sensor_millivolt = map(raw_value, 0, 1023, 0, 5000);
    float sensor_volt = sensor_millivolt/1000;
    float sensor_pressure = CALIBRATION_SCALE*(sensor_volt - calibration_offset);
    return sensor_pressure;
}
void read_pressure_sensors(){
    int analogue_sensor_reading;
    float sensor_calib_offset;

    // read each input-side pressure sensor
    for (int i = 0; i < NUM_IN_SENSORS ; i++) {
        // get and store raw sensor value (analogue input)
        analogue_sensor_reading = analogRead(IN_SENSOR_PINS[i]);
        inputPressureRawReadings[i] = analogue_sensor_reading;

        // look up calibration offset for current sensor and calibrate value to pressure
        sensor_calib_offset = IN_CALIBRATION_OFFSETS[i];
        inputPressureValsCurrent[i] = calibrate_sensor_reading(analogue_sensor_reading,sensor_calib_offset);
    }

    // read each output-side pressure sensor
    for (int i = 0; i < NUM_OUT_SENSORS ; i++) {
        // get and store raw sensor value (analogue input)
        analogue_sensor_reading = analogRead(OUT_SENSOR_PINS[i]);
        outputPressureRawReadings[i] = analogue_sensor_reading;

        // look up calibration offset for current sensor and calibrate value to pressure
        sensor_calib_offset = OUT_CALIBRATION_OFFSETS[i];
        outputPressureValsCurrent[i] = calibrate_sensor_reading(analogue_sensor_reading,sensor_calib_offset);
    }
}
//---------------

//---PUMP CONTROL AND PUMP SETPOINT UPDATE
void set_pump_state(const int pump_ind,const int next_pump_state,const int duty){
    analogWrite(PUMP_PINS[pump_ind], next_pump_state*duty);
    pumpStates[pump_ind] = next_pump_state;
}
void set_pump_setpoint(const int pump_ind, const int next_setpoint) {
    pumpSetpoints[pump_ind] = next_setpoint;
}
//---------------

//---RECEIVE, PARSE, and ACT ON A SERIAL COMMAND INPUT
void recv_serial_command() {
    // if this is the first function call, initialize serial input state variable & index
    static boolean recvInProgress = false;
    static byte ndx = 0;
    
    // read in serial inputs until command ends or serial data stops
    char rc;
    while ((Serial.available() > 0) && !newSerialInputReady) {
        rc = Serial.read();

        // if command has not started, look for start marker
        if (!recvInProgress){
            if (rc == serial_command::START_MARKER) {
                recvInProgress = true;
            }
        }

        // if command in progress, read and store characters until end marker
        else {
            if (rc != serial_command::END_MARKER) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= serial_command::MAX_INPUT_CHARS) {
                    ndx = serial_command::MAX_INPUT_CHARS - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                ndx = 0;
                recvInProgress = false;
                newSerialInputReady = true;
                Serial.println(receivedChars);
            }
        }
    }
}

void parse_command_data() {
    char * strtokIndex;

    // get command string from serial input and copy to global variable
    strtokIndex = strtok(receivedChars, ",");
    strcpy(serialCommand, strtokIndex);

    // get pin index from serial input and copy to global variable as integer
    strtokIndex = strtok(NULL, ",");
    serialIndex = atoi(strtokIndex);

    // get command value from serial input and copy to global variable as integer
    strtokIndex = strtok(NULL, ",");
    serialValue = atoi(strtokIndex);

    // mark serial data input completed
    newSerialInputReady = false;
}

bool verify_command_validity(int prefix, int suffix){
    bool valid = true;
    // check command prefix against valid prefixes
    switch(prefix){
        case (serial_command::SINGLE_VALVE_PREFIX): break;      // S
        case (serial_command::ALL_VALVES_PREFIX): break;        // A
        case (serial_command::GET_PRESSURE_PREFIX): break;      // G
        case (serial_command::GET_VALVE_STATE_PREFIX): break;   // V
        case (serial_command::REF_SETPOINT_PREFIX): break;      // R
        case (serial_command::PUMP_STATE_PREFIX): break;        // P
        default:
            Serial.print("Invalid serial command prefix! Received: ");
            Serial.print(char(prefix));
            Serial.println(char(suffix));
            valid = false;
    }
    // if command prefix is valid, check command suffix against valid suffixes
    if(valid){
        switch(suffix){
            case (serial_command::INPUT_SUFFIX): break;     // I
            case (serial_command::OUTPUT_SUFFIX): break;    // O
            case (serial_command::SET_SUFFIX): break;       // S
            case (serial_command::GET_SUFFIX): break;       // G
            default:
                Serial.print("Invalid serial command suffix! Received: ");
                Serial.print(char(prefix));
                Serial.println(char(suffix));
                valid = false;
        }
    }
    return valid;
}
 
void act_on_command() {
    // pull letters of serial command into separate variables
    int command_prefix = serialCommand[0]; // should be S, A, G, V, R, or P
    int command_suffix = serialCommand[1]; // should be I or O (input or output) or S or G (set or get)

    // if prefix and suffix letters are valid, recognized commands, carry out command
    if (verify_command_validity(command_prefix,command_suffix)){
        switch(command_prefix){
            case (serial_command::SINGLE_VALVE_PREFIX): // S
                if (command_suffix == serial_command::INPUT_SUFFIX) {
                    set_invalve_single(serialIndex, serialValue);
                }
                else if (command_suffix == serial_command::OUTPUT_SUFFIX) {
                    set_outvalve_single(serialIndex, serialValue);
                }
                break;
            case (serial_command::ALL_VALVES_PREFIX): // A
                if (command_suffix == serial_command::INPUT_SUFFIX) {
                    set_invalve_all(serialValue);
                }
                else if (command_suffix == serial_command::OUTPUT_SUFFIX) {
                    set_outvalve_all(serialValue);
                }
                break;
            case (serial_command::GET_PRESSURE_PREFIX): // G
                if (command_suffix == serial_command::INPUT_SUFFIX) {
                    Serial.println(inputPressureValsAverage[serialIndex]);
                }
                else if (command_suffix == serial_command::OUTPUT_SUFFIX) {
                    Serial.println(outputPressureValsAverage[serialIndex]);
                }
                break;
            case (serial_command::GET_VALVE_STATE_PREFIX): // V
                if (command_suffix == serial_command::INPUT_SUFFIX) {
                    Serial.println(inputValveStates[serialIndex]);
                }
                else if (command_suffix == serial_command::OUTPUT_SUFFIX) {
                    Serial.println(outputValveStates[serialIndex]);
                }
                break;
            case (serial_command::REF_SETPOINT_PREFIX): // R
                if (command_suffix == serial_command::SET_SUFFIX) {
                    set_pump_setpoint(serialIndex,serialValue);
                }
                else if (command_suffix == serial_command::GET_SUFFIX) {
                    Serial.println(pumpSetpoints[serialIndex]);
                }
                break;
            case (serial_command::PUMP_STATE_PREFIX): // P
                if (command_suffix == serial_command::SET_SUFFIX) {
                    set_pump_state(serialIndex,serialValue,DEFAULT_DUTY_CYCLE);
                }
                else if (command_suffix == serial_command::GET_SUFFIX) {
                    Serial.println(pumpStates[serialIndex]);
                }
                break;
            default:
                Serial.println("Serial command validity check failed!");
        }
    }
}
#endif //minimal_pneumatics_h