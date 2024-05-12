// Code in this file is based off code by Kurtis Laqua; see https://github.com/sitruklaq/Hattonlab-Pneumatics/tree/master/Arduino%20Code
#ifndef multichannel_pneumatics_h
#define multichannel_pneumatics_h

#include "Arduino.h"
#define OPEN 1
#define CLOSE 0

// should also define globals for input/output numbers - prelim below
// guessing intention was to be pin-agnostic but neutral in is hardcoded in pressure_switch so irrelevant
#define IN_POS 2
#define IN_NEU 1 // conflict: this function uses 2 for neutral but MATLAB uses 1?
#define IN_NEG 0

// define variables for serial communication
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
float floatFromPC = 0.0;
boolean newData = false;

// define text commands
char invalve_read[] = "VI";
char ovalve_read[] = "VO";
char psp[] = "PSP";
char nsp[] = "NSP";
char pvp[] = "PVP";
char pvn[] = "PVN";
char pvo[] = "PVO";
char aov[] = "AOV";
char aiv[] = "AIV";
char pvt[] = "PVT";

// define pressure switch variables
int switch_delay = 25;
bool p_switch;
int pin_to_open;

// define valve pins and status
const int invalve[3] = { 53, 52, 51};
const int ovalve[8] = {47, 46, 45, 44, 31, 30, 29, 28};
//const int ovalve[8] = {28, 47, 29, 46,30,45,31,44}; // this pin arrangement is for the 8-channel device only
int invalvestatus[3];
int ovalvestatus[8];

// define pump pins and status
const int positivepumppin = 6;
const int negativepumppin = 5;
const int pump[2] = {positivepumppin, negativepumppin};
int pumpstatus[2] = {0, 0};

// define pressures and setpoints
int nsetpoint = 0;
int psetpoint = 0;
float p_pressure;
float n_pressure;
float o1_pressure;
float o2_pressure;
float o3_pressure;
float o4_pressure;
float o5_pressure;
float p_pressure_avg = 0;
float n_pressure_avg = 0;
float o1_pressure_avg = 0;
float o2_pressure_avg = 0;
float o3_pressure_avg = 0;
float o4_pressure_avg = 0;
float o5_pressure_avg = 0;

//----ARDUINO INITIALIZATION
void initialize_pins() {
  for (int i = 0; i < 3; i++) {
    pinMode(invalve[i], OUTPUT);
    digitalWrite(invalve[i], LOW);
  }

  for (int i = 0; i < 8; i++) {
    pinMode(ovalve[i], OUTPUT);
    digitalWrite(ovalve[i], LOW);
  }
}
//---------------

//---THIS FUNCTION TURNS ON/OFF ALL INPUT VALVES
void setall_invalves(int position) {
  if (position == OPEN) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(invalve[i], HIGH);
    }
  }

  if (position == CLOSE) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(invalve[i], LOW);
    }
  }
}
//---------------------

//---THIS FUNCTION TURNS ON/OFF A SPECIFIC INPUT VALVE
void set_invalve(int number, int position) {
  if (position == OPEN) {
    digitalWrite(invalve[number], HIGH);
  }

  if (position == CLOSE) {
    digitalWrite(invalve[number], LOW);
  }
}
//---------------

//---THIS FUNCTION TURNS ON/OFF ALL OUTPUT VALVES
void setall_outvalves(int position) {
  if (position == OPEN) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(ovalve[i], HIGH);
    }
  }

  if (position == CLOSE) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(ovalve[i], LOW);
    }
  }
}
//----------------------------

//---THIS FUNCTION TURNS ON/OFF A SPECIFIC OUTPUT VALVE
void set_outvalve(int number, int position) {
  if (position == OPEN) {
    digitalWrite(ovalve[number], HIGH);
  }
  if (position == CLOSE) {
    digitalWrite(ovalve[number], LOW);
  }
}
//-------

//---THIS FUNCTION SWITCHES PRESSURE USING A QUICK NEUTRAL PRESSURE EVACUATION FIRST
void pressure_switch(int pin_to_open, int current, int when) {
  // switch_delay is delay, set as hardcoded global value
  setinvalve(IN_NEU, OPEN); // used to use 2 instead of IN_NEU
  if ((unsigned long)(current - when) >= switch_delay) {
    p_switch = false;
    setinvalve(IN_NEU, CLOSE); // used to use 2 instead of IN_NEU
    setinvalve(pin_to_open, OPEN);
  }
}

void pressure_control(int n_input, int p_input, int o1_input, int o2_input, int o3_input, int o4_input, int o5_input, int power) {

  //sensor math to convert input voltage to pressure in KPa
  float  o1_voltage = map(o1_input, 0, 1023, 0, 5000);
  float  o2_voltage = map(o2_input, 0, 1023, 0, 5000);
  float  o3_voltage = map(o3_input, 0, 1023, 0, 5000);
  float  o4_voltage = map(o4_input, 0, 1023, 0, 5000);
  float  o5_voltage = map(o5_input, 0, 1023, 0, 5000);
  float  n_voltage = map(n_input, 0, 1023, 0, 5000);
  float  p_voltage = map(p_input, 0, 1023, 0, 5000);

  n_pressure = 50 * (n_voltage / 1000) - 125.1;
  p_pressure = 50 * (p_voltage / 1000) - 124.85;
  o1_pressure = 50 * (o1_voltage / 1000) - 124.6;
  o2_pressure = 50 * (o2_voltage / 1000) - 125.35;
  o3_pressure = 50 * (o3_voltage / 1000) - 124.1;
  o4_pressure = 50 * (o4_voltage / 1000) - 124.85;
  o5_pressure = 50 * (o5_voltage / 1000) - 125.35;

  p_pressure_avg = p_pressure_avg / 100 + (99/100)*p_pressure;
  n_pressure_avg = n_pressure_avg / 100 + (99/100)*n_pressure;
  o1_pressure_avg = o1_pressure_avg / 100 + (99/100)*o1_pressure;
  o2_pressure_avg = o2_pressure_avg / 100 + (99/100)*o2_pressure;
  o3_pressure_avg = o3_pressure_avg / 100 + (99/100)*o3_pressure;
  o4_pressure_avg = o4_pressure_avg / 100 + (99/100)*o4_pressure;
  o5_pressure_avg = o5_pressure_avg / 100 + (99/100)*o5_pressure;

  // Serial.print("Positive Pressure is: ");
  // Serial.print(p_pressure_avg);
  // Serial.print(", Negative Pressure is: ");
  // Serial.println(n_pressure_avg);
  // Serial.println(",");
  //Serial.print("Output Pressures: ");
  //Serial.print(o1_pressure_avg);
  //Serial.print(", ");
  //Serial.print(o2_pressure_avg);
  //Serial.print(", ");
  //Serial.print(o3_pressure_avg);
  //Serial.print(", ");
  //Serial.print(o4_pressure_avg);
  //Serial.print(", ");
  //Serial.println(o5_pressure_avg);
  
  // sources of error:
  //- insufficient voltage supply to motors (likely not this given 0 out V),
  //- incorrect reading/eval of current average pressure (possible, code is confusing here),
  //- incorrect delivery of control signal (need scope)

  //pressure regulation
  if (p_pressure_avg < (psetpoint - .3)) {     
    analogWrite(positivepumppin, power); // perhaps debug with scope
  }
  if (abs(p_pressure_avg) > psetpoint) {
    analogWrite(positivepumppin, 0);
  }

  if (n_pressure_avg > (nsetpoint + .3)) {
    analogWrite(negativepumppin, power);
  }
  if (n_pressure_avg < nsetpoint) {
    analogWrite(negativepumppin, 0);
  }
}
//-----------------------

//---THIS SECTION PULLS OUT VALUES FROM A SERIAL INPUT SO IT CAN BE READ AND EXECUTED ON
void parse_data() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  integerFromPC = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  floatFromPC = atof(strtokIndx);     // convert this part to a float
}

// This section is looking for new serial inputs and parsing them
void recv_with_endmarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}



//============ This section acts 
void act_on_input() {
  if (strcmp(messageFromPC, invalve_read) == 0) {
    setinvalve(integerFromPC, floatFromPC);
  }
  if (strcmp(messageFromPC, ovalve_read) == 0) {
    setoutvalve(integerFromPC, floatFromPC);
  }
  if (strcmp(messageFromPC, nsp) == 0) {
    nsetpoint = integerFromPC;
  }
  if (strcmp(messageFromPC, psp) == 0) {
    psetpoint = integerFromPC;
  }
  if (strcmp(messageFromPC, pvp) == 0) { // this function seems to be unused in MATLAB app (no send routine) but would be handy
    Serial.println(p_pressure_avg);
  }
  if (strcmp(messageFromPC, pvn) == 0) { // ditto above
    Serial.println(n_pressure_avg);
  }

  if (strcmp(messageFromPC, pvo) == 0) { // ditto above
    if (integerFromPC == 1) {
      Serial.println(o1_pressure_avg);
    }
    if (integerFromPC == 2) {
      Serial.println(o2_pressure_avg);
    }
    if (integerFromPC == 3) {
      Serial.println(o3_pressure_avg);
    }
    if (integerFromPC == 4) {
      Serial.println(o4_pressure_avg);
    }
    if (integerFromPC == 5) {
      Serial.println(o5_pressure_avg);
    }
  }
  if (strcmp(messageFromPC, pvt) == 0) { // ditto above
    Serial.println(o2_pressure_avg);
  }
  if (strcmp(messageFromPC, aov) == 0) {
    setalloutvalves(integerFromPC);
  }
  if (strcmp(messageFromPC, aiv) == 0) {
    setallinvalves(integerFromPC);
  }
}
#endif //multichannel_pneumatics_h