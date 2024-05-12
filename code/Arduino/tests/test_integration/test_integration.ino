/*
  test_integration
  part of pneumatic control code used in the Hatton lab to pneumatically actuate devices

  Turns pneumatic pumps on or off as needed to reach a target pressure
  Toggles state of the input valves and one output valve
  Adaptation of Kurtis' code to use a SINGLE channel and test integration

  written 2024-02-11 by Katie Allison
  original pneumatics code by Kurtis Laqua and Aly Hassan
*/
#include "Arduino.h"

// Globals for valve states (where OPEN is HIGH because valves are NC)
#define OPEN true
#define CLOSED false

// Globals for valve ID order
#define OUT_0 3
#define IN_POS 2
#define IN_NEU 1
#define IN_NEG 0

// Pin specifications (input pins, for pressure sensor)
const int p_sensor_pin = A15;
const int n_sensor_pin = A14;
const int o_sensor_pin = A13;
const int num_sensor_pins = 3;
const int pressure_sensor_pins[3] = {p_sensor_pin,n_sensor_pin,o_sensor_pin};

// Pin specifications (output pins, for pumps)
const int pump_pins[4]= {10,11,8,9};
const int positivepump_pin1= 10;
const int positivepump_pin2= 11;
const int negativepump_pin1= 8;
const int negativepump_pin2= 9;

// Pin specifications (output pins, for input and output valves)
const int num_input_valves = 3;
const int input_valve_pins[3] = {53,52,51};
const int num_output_valves = 8;
const int output_valve_pins[8] = {47,46,45,44,31,30,29,28};

// State variables
bool input_valve_states[3] = {CLOSED,CLOSED,CLOSED};
bool output_valve_states[1] = {CLOSED};

// Variables into which to read reservoir and output sensor values (as raw analogue input)
int p_sensor_reading = 0;
int n_sensor_reading = 0;
int o_sensor_reading = 0;
float p_pressure_curr = 0;
float n_pressure_curr = 0;
float o_pressure_curr = 0;

// Variables for computing intermediate values for sensor readings
bool is_p_sensor;
float sensor_millivolt = 0;
float sensor_volt = 0;
float pressure_curr = 0;

// Calibration parameters
const float p_offset_calib = 2.512; // in V
const float n_offset_calib = 2.519; // in V
const float p_scale_calib = 50;
const float n_scale_calib = 50;

// Booleans to enable pump outputs
bool p_pump_enabled = true;
bool n_pump_enabled = true;

// Changeable constants: delay time, and test parameters
const int delay_time = 1000; // in ms
int pump_dutycycle = 190; // from 0 (fully OFF) to 255 (fully ON)
bool next_state = CLOSED;

// Variables for setting setpoint for reservoirs [in kPa]
int p_setpoint = 5;
int n_setpoint = -5;
float setpoint_buffer = 0.5; // in kPa

void setup() {
    // ensure analogue reference voltage is default (5V) and set up pins
  analogReference(DEFAULT);
  initialize_pins();
  
  // set up serial output
  Serial.begin(9600);
  Serial.println();

  // open valves for 1 minute
  set_all_input_valves(OPEN);
  Serial.println("Opened valves. Now waiting 15 seconds...");
  for (int i = 0; i < 15; i++) {
    delay(1000);
    Serial.println(i);
  }
  set_all_input_valves(CLOSED);
  Serial.println("Closed input valves. Ready to receive.");
}

void loop() {
   // read in current positive reservoir pressure sensor value
  p_sensor_reading = analogRead(p_sensor_pin);
  n_sensor_reading = analogRead(n_sensor_pin);
  o_sensor_reading = analogRead(o_sensor_pin);
  Serial.print("Positive pressure sensor reading [as raw input]: ");
  Serial.println(p_sensor_reading);
  Serial.print("Negative pressure sensor reading [as raw input]: ");
  Serial.println(n_sensor_reading);
  Serial.print("Output pressure sensor reading [as raw input]: ");
  Serial.println(o_sensor_reading);
  
  // calibrate voltage value to get pressure value (values taken from Big_system.h file)
  p_pressure_curr = get_calibrated_value(true,p_sensor_reading);
  n_pressure_curr = get_calibrated_value(false,n_sensor_reading);
  o_pressure_curr = get_calibrated_value(false,o_sensor_reading);
  Serial.print("Current positive pressure value [as estimated kPa]: ");
  Serial.println(p_pressure_curr);
  Serial.print("Current negative pressure value [as estimated kPa]: ");
  Serial.println(n_pressure_curr);
  Serial.print("Current output pressure value [as estimated kPa]: ");
  Serial.println(o_pressure_curr);

  // check positive pressure against setpoint and turn pump on/off if needed
  if (p_pump_enabled){
    if (p_pressure_curr < (p_setpoint - setpoint_buffer)){
      Serial.println("Turning positive reservoir pump ON");
      digitalWrite(positivepump_pin1, pump_dutycycle);
    }
    else{
      Serial.println("Turning positive reservoir pump OFF");
      digitalWrite(positivepump_pin1, 0);
    }
  }

  // check negative pressure against setpoint and turn pump on/off if needed
  if (n_pump_enabled){
    if (n_pressure_curr > (n_setpoint + setpoint_buffer)){
      Serial.println("Turning negative reservoir pump ON");
      digitalWrite(negativepump_pin1, pump_dutycycle);
    }
    else{
      Serial.println("Turning negative reservoir pump OFF");
      digitalWrite(negativepump_pin1, 0);
    }
  }

  Serial.println("Valve state? ");

  while (Serial.available() == 0) {
  }

  int valve = Serial.parseInt();

  switch (valve) {
    // case IN_NEG:
    //   Serial.print("Negative valve toggling to ");
    //   next_state = !input_valve_states[IN_NEG];
    //   Serial.println(next_state);
    //   set_input_valve(IN_NEG,next_state);
    //   break;

    case IN_NEU:
      Serial.print("Neutral valve toggling to ");
      next_state = !input_valve_states[IN_NEU];
      Serial.println(next_state);
      set_input_valve(IN_NEU,next_state);
      break;

    case IN_POS:
      Serial.print("Positive valves toggling to ");
      next_state = !input_valve_states[IN_POS];
      Serial.println(next_state);
      set_input_valve(IN_POS,next_state);
      break;

    case OUT_0:
      Serial.print("Output valve toggling to ");
      next_state = !output_valve_states[0];
      Serial.println(next_state);
      set_output_valve(0,next_state);
      break;

    case 4:
      break;

    default:
      Serial.println("Please choose a valid selection");
  }
}

// HELPER FUNCTIONS HERE ------------------------
void initialize_pins(){
  // set up sensor pins as inputs
  for (int i = 0; i < num_sensor_pins ; i++) {
    pinMode(pressure_sensor_pins[i], INPUT);
  }
  
  // set up valve pins as outputs
  for (int i = 0; i < num_input_valves ; i++) {
    pinMode(input_valve_pins[i], OUTPUT);
  }
  for (int i = 0; i < num_output_valves ; i++) {
    pinMode(output_valve_pins[i], OUTPUT);
  }

  // set up pump pins as outputs
  pinMode(positivepump_pin1, OUTPUT);
  pinMode(positivepump_pin2, OUTPUT);
  pinMode(negativepump_pin1, OUTPUT);
  pinMode(negativepump_pin2, OUTPUT);

  // set pin 2 for each pump to be LOW always
  digitalWrite(positivepump_pin2, LOW);
  digitalWrite(negativepump_pin2, LOW);
}

int get_calibrated_value(bool is_p_sensor, int sensor_reading){
  // map raw sensor reading to voltage (where analogue input max is 1023 and pin logic HIGH is 5V)
  sensor_millivolt = map(sensor_reading, 0, 1023, 0, 5000);
  sensor_volt = sensor_millivolt/1000;

  // calibrate voltage value to get pressure value (values taken from Big_system.h file)
  if (is_p_sensor){
    pressure_curr = p_scale_calib*(sensor_volt-p_offset_calib);
  }
  else{
    pressure_curr = n_scale_calib*(sensor_volt-n_offset_calib);
  }
  return pressure_curr;
}

void set_input_valve(int valve_id,bool valve_state){
  Serial.print("Setting valve of input valve ");
  Serial.println(valve_id);
  digitalWrite(input_valve_pins[valve_id], valve_state);
  input_valve_states[valve_id] = valve_state;
}
void set_output_valve(int valve_id,bool valve_state){
  Serial.print("Setting valve of output valve ");
  Serial.println(valve_id);
  digitalWrite(output_valve_pins[valve_id], valve_state);
  output_valve_states[valve_id] = valve_state;
}
void set_all_input_valves(bool valve_state){
  Serial.print("Setting valve of all input valves to ");
  Serial.println(valve_state);
  for (int i = 0; i < num_input_valves; i++) {
    digitalWrite(input_valve_pins[i], valve_state);
    input_valve_states[i] = valve_state;
  }
}
void set_all_output_valves(bool valve_state){
  Serial.print("Setting valve of all output valves to ");
  Serial.println(valve_state);
  for (int i = 0; i < num_output_valves; i++) {
    digitalWrite(output_valve_pins[i], valve_state);
    output_valve_states[i] = valve_state;
  }
}
