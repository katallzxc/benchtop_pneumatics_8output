/*
  test_pressure_sensing
  part of pneumatic control code used in the Hatton lab to pneumatically actuate devices

  Opens input valves for 1 minute then continously:
    Reads value of positive and negative reservoir pressure sensors.
    Prints raw and calibrated sensor readings to serial monitor.

  Testing with pressure reservoirs at steady state:
    with p_offset_calib = 2.512, positive sensor almost ALWAYS reads 0 kPa (rare dip to -0.25)
    with n_offset_calib = 2.519, negative oscillates between -0.10 and +0.10 kPa

  written 2023-11-09 by Katie Allison
  last modified 2023-11-10
  original pneumatics code (pressure_sensor and pressure_sensor_2) by Kurtis Laqua
*/
// Globals for valve states (where OPEN is HIGH because valves are NC)
#define OPEN HIGH
#define CLOSED LOW

// Pin specifications (output pins, for input valves)
const int num_input_valves = 3;
const int input_valve_pins[num_input_valves] = {53,52,51};

// Pin specifications (input pins, for pressure sensor)
const int pressure_sensor_pins[2] = {A15,A14};
const int p_sensor_pin = A15;
const int n_sensor_pin = A14;

// Variables into which to read reservoir sensor values (as raw analogue input)
int p_sensor_reading = 0;
int n_sensor_reading = 0;

// Variables for computing intermediate values for sensor readings
float p_sensor_millivolt = 0;
float p_sensor_volt = 0;
float n_sensor_millivolt = 0;
float n_sensor_volt = 0;

// Variables for current computed (approx. actual) reservoir pressure values
float p_pressure_curr = 0;
float n_pressure_curr = 0;

// Variables for expected offset and scaling factor (from sensor datasheet)
// Use of values is: pressure_curr = scale_nominal*(sensor_voltage-offset_nominal)
float offset_nominal = 2.5; // in V
float scale_nominal = 50;

// Changeable constants: length of delays and calibration parameters
int delay_time = 1000; // in ms
float p_offset_calib = 2.512; // in V
float n_offset_calib = 2.519; // in V
float p_scale_calib = 50;
float n_scale_calib = 50;

// Boolean that either prints all intermediate values to serial monitor or prints only the final values to serial plotter
bool plot_on = 1;

void setup(){
  // ensure analogue reference voltage is default (5V)
  analogReference(DEFAULT);
  
  // set up serial output
  Serial.begin(9600);
  Serial.println();

  // set up sensor pins as inputs
  pinMode(p_sensor_pin, INPUT);
  pinMode(n_sensor_pin, INPUT);

  // set up valve pins as outputs
  for (int i = 0; i < num_input_valves ; i++) {
    pinMode(input_valve_pins[i], OUTPUT);
  }

  // open valves for 1 minute
  for (int i = 0; i < num_input_valves ; i++) {
    digitalWrite(input_valve_pins[i], OPEN);
  }
  Serial.println("Opened valves. Now waiting 1 minute...");
  for (int i = 0; i < 60; i++) {
    delay(1000);
    Serial.println(i);
  }
}

void loop() {
  // read in current positive reservoir pressure sensor value
  p_sensor_reading = analogRead(p_sensor_pin);
  if (not plot_on){
    Serial.print("Positive pressure sensor reading [as raw input]: ");
    Serial.println(p_sensor_reading);
  }
  
  // read in current positive reservoir pressure sensor value
  n_sensor_reading = analogRead(n_sensor_pin);
  if (not plot_on){
    Serial.print("Negative pressure sensor reading [as raw input]: ");
    Serial.println(n_sensor_reading);
  }

  // map raw sensor reading to voltage (where analogue input max is 1023 and pin logic HIGH is 5V)
  p_sensor_millivolt = map(p_sensor_reading, 0, 1023, 0, 5000);
  n_sensor_millivolt = map(n_sensor_reading, 0, 1023, 0, 5000);
  if (not plot_on){
    Serial.print("Positive pressure sensor voltage [as mapped mV]: ");
    Serial.println(p_sensor_millivolt);
    Serial.print("Negative pressure sensor voltage [as mapped mV]: ");
    Serial.println(n_sensor_millivolt);
  }
  
  // convert from mV to V
  p_sensor_volt = p_sensor_millivolt/1000;
  n_sensor_volt = n_sensor_millivolt/1000;

  // calibrate voltage value to get pressure value (values taken from Big_system.h file)
  p_pressure_curr = p_scale_calib*(p_sensor_volt-p_offset_calib);
  n_pressure_curr = n_scale_calib*(n_sensor_volt-n_offset_calib);
  if (not plot_on){
    Serial.print("Current positive pressure value [as estimated kPa]: ");
    Serial.println(p_pressure_curr);
    Serial.print("Current negative pressure value [as estimated kPa]: ");
    Serial.println(n_pressure_curr);
  }
  else{
    Serial.print("+P[kPa]:");
    Serial.print(p_pressure_curr);
    Serial.print("\t");
    Serial.print("-P[kPa]:");
    Serial.println(n_pressure_curr);
  }
  
  // pause before next reading
  if (not plot_on){
    Serial.println("===========");
  }
  delay(delay_time);
}
