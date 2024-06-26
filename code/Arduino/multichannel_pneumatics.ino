// Code in this file is based off code by Kurtis Laqua; see https://github.com/sitruklaq/Hattonlab-Pneumatics/tree/master/Arduino%20Code

#include "multichannel_pneumatics.h"
//--pump control
//change these values to control how the pressure is controlled in the system.
// positive pressure control high/low values. Units are in KPa. Make sure High > Low
// Negative pressure control high/low values. Units are in KPa. Make sure High < Low, different than positive

//this is how hard the pumps work. 0= 0%, 255= 100% The pumps won't turn on if the value is too low.
int pumppower = 190;

float inputvalue;
float voltage;
float pressure;

void setup() {
  initializepins();
  Serial.begin(19200);
  int n_pres_sensor = analogRead(A14);
  int p_pres_sensor = analogRead(A15);
  int o_pres_sensor1 = analogRead(A13);
  int o_pres_sensor2=analogRead(A12);
  int o_pres_sensor3=analogRead(A11);
  int o_pres_sensor4=analogRead(A10);
  int o_pres_sensor5=analogRead(A9);
}

void loop() {
  pressurecontrol(n_pres_sensor, p_pres_sensor, o_pres_sensor1,o_pres_sensor2,o_pres_sensor3,o_pres_sensor4,o_pres_sensor5,pumppower);
  unsigned long currentMillis = millis(); 

  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    act_on_input();
    newData = false;
  }
}