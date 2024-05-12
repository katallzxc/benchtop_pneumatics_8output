/* Minimal Pneumatics
    for pneumatic device control using multichannel pneumatic setup in the Hatton lab
    Katie Allison, 2024;
        see 
    Loosely based off code by Kurtis Laqua; 
        see https://github.com/sitruklaq/Hattonlab-Pneumatics/tree/master/Arduino%20Code
*/
#include "minimal_pneumatics.h"

// set pump duty cycle (i.e., how hard air is pumped)
int pump_duty_cycle = 190;

// set values of buffers and delays
float pumpSetpointBuffer = 0.3; // in kPa
int pressureSwitchDelay = 25; // in ms?

// initialize pump control variables (for getting current pressure and updating pump state)
float updated_pressure_val;
float neg_pressure_setpoint = 0;
float pos_pressure_setpoint = 0;
float neg_pressure_running_avg;
float pos_pressure_running_avg;
int updated_pump_state;

void setup() {
    initialize_pins();
    Serial.begin(19200);
}

void loop() { 
    // receive and act on any serial inputs
    recv_serial_command();
    if (newSerialInputReady) {
        parse_command_data();
        act_on_command();   // based on input command, open/close valves or change pump setpoints
    }
    // regulate input channel pressures to current setpoints
    pressure_control(pump_duty_cycle);
}

//---THIS FUNCTION IS THE MAIN CONTROL LOOP
void pressure_control(int pump_dutycycle) {
    // get pressure sensor readings (calibrated to kPa) and update running averages
    read_pressure_sensors();
    for (int i = 0; i < NUM_IN_SENSORS ; i++) {
        updated_pressure_val = inputPressureValsCurrent[i];
        inputPressureValsAverage[i] = (inputPressureValsAverage[i] + 99.f*updated_pressure_val)/100.f;
    }
    for (int i = 0; i < NUM_OUT_SENSORS ; i++) {
        updated_pressure_val = outputPressureValsCurrent[i];
        outputPressureValsAverage[i] = (outputPressureValsAverage[i] + 99.f*updated_pressure_val)/100.f;
    }

    // control negative reservoir pump state based on average negative input channel pressure values
    neg_pressure_running_avg = inputPressureValsAverage[input_sensors::INS_NEG];
    neg_pressure_setpoint = pumpSetpoints[pumps::NEG];
    if (neg_pressure_running_avg > (neg_pressure_setpoint + pumpSetpointBuffer)) {
        updated_pump_state = PUMP_ON;
    }
    else if (neg_pressure_running_avg < (neg_pressure_setpoint - pumpSetpointBuffer)){
        updated_pump_state = PUMP_OFF;
    }
    else{
        //Serial.println("INFO: Negative pump in buffer range of setpoint");
    }
    set_pump_state(pumps::NEG,updated_pump_state,pump_dutycycle);

    // control positive reservoir pump state based on average positive input channel pressure values
    pos_pressure_running_avg = inputPressureValsAverage[input_sensors::INS_POS];
    pos_pressure_setpoint = pumpSetpoints[pumps::POS];
    if (pos_pressure_running_avg < (pos_pressure_setpoint - pumpSetpointBuffer)) {
        updated_pump_state = PUMP_ON;
    }
    else if (pos_pressure_running_avg > (pos_pressure_setpoint + pumpSetpointBuffer)){
        updated_pump_state = PUMP_OFF;
    }
    else{
        //Serial.println("INFO: Positive pump in buffer range of setpoint");
    }
    set_pump_state(pumps::POS,updated_pump_state,pump_dutycycle);
}
//-----------------------