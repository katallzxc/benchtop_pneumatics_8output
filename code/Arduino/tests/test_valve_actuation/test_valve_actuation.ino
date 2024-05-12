/*
  test_valve_actuation
  part of pneumatic control code used in the Hatton lab to pneumatically actuate devices

  Opens each input and output valve, then closes each input and output valve.

  written 2023-11-10 by Katie Allison
  last modified 2023-11-10
  original pneumatics code by Kurtis Laqua
*/
// Globals for valve states (where OPEN is HIGH because valves are NC)
#define OPEN HIGH
#define CLOSED LOW

// Globals for valve ID order
#define IN_POS 2
#define IN_NEU 1
#define IN_NEG 0

// Pin specifications (output pins, for input and output valves)
const int input_valve_pins[3] = {53,52,51};
const int output_valve_pins[8] = {47,46,45,44,31,30,29,28};

// Changeable constants: length of delay and test set
int delay_time = 1000; // in ms
const int num_input_tests = 3;
int input_valves_to_test[num_input_tests] = {IN_POS,IN_NEU,IN_NEG}; // indices for selected valves to test
const int num_output_tests = 8;
int output_valves_to_test[num_output_tests] = {0,1,2,3,4,5,6,7}; // indices for selected valves to test

void setup() {
  // set up serial output
  Serial.begin(9600);
  Serial.println();

  // set up valve pins as outputs
  Serial.println(input_valve_pins[0]);
  for (int i = 0; i < 3 ; i++) {
    pinMode(input_valve_pins[i], OUTPUT);
  }
  for (int i = 0; i < 8 ; i++) {
    pinMode(output_valve_pins[i], OUTPUT);
  }
}

void loop() {
  // open all test input valves
  set_input_valves(OPEN);
  Serial.println("Input valve(s) opened");

  // open all test output valves
  set_output_valves(OPEN);
  Serial.println("Output valve(s) opened");

  // close all test input valves
  set_input_valves(CLOSED);
  Serial.println("Input valve(s) closed");

  // open all test output valves
  set_output_valves(CLOSED);
  Serial.println("Output valve(s) closed");
}

// HELPER FUNCTIONS HERE ------------------------
void set_input_valves(int next_position){
  int valve_ind = 0;
  for (int i = 0; i < num_input_tests ; i++) {
    // Get index of next valve to set
    valve_ind = input_valves_to_test[i];
    Serial.print("Setting valve: ");
    Serial.println(input_valve_pins[valve_ind]);

    // Set valve and wait
    digitalWrite(input_valve_pins[valve_ind], next_position);
    delay(delay_time);
  }
}
void set_output_valves(int next_position){
  int valve_ind = 0;
  for (int i = 0; i < num_output_tests ; i++) {
    // Get index of next valve to set
    valve_ind = output_valves_to_test[i];
    Serial.print("Setting valve: ");
    Serial.println(output_valve_pins[valve_ind]);

    // Set valve and wait
    digitalWrite(output_valve_pins[valve_ind], next_position);
    delay(delay_time);
  }
}