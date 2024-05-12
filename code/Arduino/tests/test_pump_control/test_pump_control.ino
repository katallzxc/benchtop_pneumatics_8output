/*
  test_pump_control
  part of pneumatic control code used in the Hatton lab to pneumatically actuate devices

  Reads value of positive and negative reservoir pressure sensors.
  If positive pressure is too low, turns positive pump on.
  If positive pressure is sufficiently high, turns positive pump off.
  If negative pressure is too high, turns negative pump on.
  If negative pressure is sufficiently low, turns negative pump off.

  CAUTION: this code will run pumps for up to 5 minutes if sensor feedback is wrong
  DO NOT leave running unattended

  written 2023-11-03 by Katie Allison
  last modified 2023-11-09
  original pneumatics code (pressure_sensor and pressure_sensor_2) by Kurtis Laqua
*/

// Pin specifications (output)
const int pump_pins[4] = {10,11,8,9};
const int p_pump_pin1 = 10;
const int p_pump_pin2 = 11;
const int n_pump_pin1 = 8;
const int n_pump_pin2 = 9;

// Pin specifications (input)
const int pressure_sensor_pins[2] = {A15,A14};
const int p_sensor_pin = A15;
const int n_sensor_pin = A14;

// Variables into which to read reservoir sensor values (as raw analogue input)
int p_sensor_reading = 0;
int n_sensor_reading = 0;

// Variables for computing intermediate values for sensor readings
float p_sensor_millivolt = 0;
float p_sensor_voltage = 0;
float n_sensor_millivolt = 0;
float n_sensor_voltage = 0;

// Calibration values (calibrated in test_pressure_sensor) for converting voltage to pressure
// Use of values is: pressure_curr = scale_calib*(sensor_voltage-offset_calib)
const float p_offset_calib = 2.509; // in V
const float n_offset_calib = 2.519; // in V
const float p_scale_calib = 50;
const float n_scale_calib = 50;

// Variables for current computed (approx. actual) reservoir pressure values
float p_pressure_curr = 0;
float n_pressure_curr = 0;

// Changeable constants: pump duty cycle (essentially power) and length of delays
int pump_dutycycle = 190; // from 0 (fully OFF) to 255 (fully ON)
const int delay_time = 1000;

// Variables for setting setpoint for reservoirs [in kPa]
int p_setpoint = 0;
int n_setpoint = 0;
float setpoint_buffer = 0.5; // in kPa

// Booleans to enable pump outputs
bool p_pump_enabled = 1;
bool n_pump_enabled = 1;

void setup() {
  // set up pump output pins
  pinMode(p_pump_pin1, OUTPUT);
  pinMode(p_pump_pin2, OUTPUT);
  pinMode(n_pump_pin1, OUTPUT);
  pinMode(n_pump_pin2, OUTPUT);

  // set pin 2 for each pump to be LOW always
  digitalWrite(p_pump_pin2, LOW);
  digitalWrite(n_pump_pin2, LOW);

  // specify pressure setpoints [in kPa]
  p_setpoint = 40;
  n_setpoint = -40;
  
  // ensure analogue reference voltage is default (5V)
  analogReference(DEFAULT);
  
  // set up serial output
  Serial.begin(9600);
  Serial.println();
}

void loop() {
  // read in current positive reservoir pressure sensor value
  p_sensor_reading = analogRead(p_sensor_pin);
  Serial.print("Positive pressure sensor reading [as raw input]: ");
  Serial.println(p_sensor_reading);
  
  // read in current negative reservoir pressure sensor value
  n_sensor_reading = analogRead(n_sensor_pin);
  Serial.print("Negative pressure sensor reading [as raw input]: ");
  Serial.println(n_sensor_reading);

  // map raw sensor reading to mV (where analogue input max is 1023 and pin logic HIGH is 5V)
  p_sensor_millivolt = map(p_sensor_reading, 0, 1023, 0, 5000);
  n_sensor_millivolt = map(n_sensor_reading, 0, 1023, 0, 5000);

  // scale mV values to V
  p_sensor_voltage = p_sensor_millivolt/1000;
  n_sensor_voltage = n_sensor_millivolt/1000;
  Serial.print("Positive pressure sensor voltage [in V]: ");
  Serial.println(p_sensor_voltage);
  Serial.print("Negative pressure sensor voltage [in V]: ");
  Serial.println(n_sensor_voltage);

  // calibrate voltage value to get pressure value (values taken from Big_system.h file)
  p_pressure_curr = p_scale_calib*(p_sensor_voltage - p_offset_calib);
  n_pressure_curr = n_scale_calib*(n_sensor_voltage - n_offset_calib);
  Serial.print("Current positive pressure value [as estimated kPa]: ");
  Serial.println(p_pressure_curr);
  Serial.print("Current negative pressure value [as estimated kPa]: ");
  Serial.println(n_pressure_curr);
  
  // check positive pressure against setpoint and turn pump on/off if needed
  if (p_pump_enabled){
    if (p_pressure_curr < (p_setpoint - setpoint_buffer)){
      Serial.println("Turning positive reservoir pump ON");
      digitalWrite(p_pump_pin1, pump_dutycycle);
    }
    else{
      Serial.println("Turning positive reservoir pump OFF");
      digitalWrite(p_pump_pin1, 0);
    }
  }

  // check negative pressure against setpoint and turn pump on/off if needed
  if (n_pump_enabled){
    if (n_pressure_curr > (n_setpoint + setpoint_buffer)){
      Serial.println("Turning negative reservoir pump ON");
      digitalWrite(n_pump_pin1, pump_dutycycle);
    }
    else{
      Serial.println("Turning negative reservoir pump OFF");
      digitalWrite(n_pump_pin1, 0);
    }
  }

  // pause before next reading
  Serial.println("===========");
  delay(delay_time);
}
