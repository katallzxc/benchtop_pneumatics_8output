# Summary of Arduino sketches for testing system

## List of test code

The table below lists test sketches in recommended execution order, starting from the most simple/baseline tests and progressing to the most complex tests.

| Directory | Link to Sketch | Purpose |
|------|------|---------|
|test_pump_actuation | ![Folder](../code/Arduino/tests/test_pump_actuation/test_pump_actuation.ino) | Verify that actuation signals can turn pumps on and off and check that pumps are being actuated in intended order. |
|test_valve_open | ![Folder](../code/Arduino/tests/test_valve_open/test_valve_open.ino) | Verify that actuation signals can open valves and check that valves are being opened in intended order. |
|test_pressure_sensing | ![Folder](../code/Arduino/tests/test_pressure_sensing/test_pressure_sensing.ino) | Verify that pressure sensor signals (for input channels only) can be read. With input channels opened (via valve control) to atmospheric pressure, verify that pressure sensor signals are near zero. |
|test_pump_control | ![Folder](../code/Arduino/tests/test_pump_control/test_pump_control.ino) | Test pump and sensor integration using hardcoded setpoints. Verify that each pump will turn on whenever the input pressure in the corresponding channel is far from the setpoint and then turn off whenever the input pressure reaches the setpoint. |
|test_valve_actuation | ![Folder](../code/Arduino/tests/test_valve_actuation/test_valve_actuation.ino) | Verify that actuation signals can actuate (open **and** close) valves and check that valves are being actuated in intended order. |
|test_integration | ![Folder](../code/Arduino/tests/test_integration/test_integration.ino) | Test integrated pump, valve, and sensor system using hardcoded setpoints. Runs a loop of continuous pressure regulation to keep both positive and negative channels at setpoint pressure levels while querying user for valve IDs. Whenever user supplies a valve ID, toggles the state of that valve (from open to closed or from closed to open). Possible valve IDs are positive, negative, and output 0. |

## Description of test code functionality

### Pump actuation testing
This stage of testing verifies that an assembled pump control circuit correctly actuates pumps based on supplied pump ID, state (on/off), and power level.

[!CAUTION]
Before using this code, ensure that the pump circuit is correct. Pumps can be proxied with standard DC motors provided that the voltage supply levels for the motors are respected (i.e., if using a 5V motor as a proxy for a 12V pump, ensure that input power is limited to 5V). Ensure that the motor power is not supplied by the microcontroller and that there is protective circuitry (e.g., a motor driver board, or an H-bridge circuit with protective components like diodes) between the pump/motor and the microcontroller to protect the microcontroller.

The following **setup** variables must be set or checked:
 - positive input channel pump pins: set the pin numbers for the two Arduino pins used to drive the positive input channel pump
 - negative input channel pump pins: set the pin numbers for the two Arduino pins used to drive the negative input channel pump

For each pump, the microcontroller will send a PWM signal (alternating logic high and logic low) over one pin and set the other pin to logic low. The pump rotation direction depends on which of its leads receives the PWM signal.

[!NOTE]
Since the directions of rotation for the pumps do not change (and so the lead that receives the PWM signal is the same every time for each pump), the circuit could alternatively be structured such that one of the leads for each pump is connected to the driver circuit and the other lead is connected directly to ground.

The following **testing** variable must then be set or checked:
 - pump power is set via the `pump_dutycycle` variable to a value between 0 and 255, with a higher duty cycle corresponding to a faster pump speed.

At this stage, the code can be uploaded to the Arduino. You should see the positive pump run for one second, then the negative pump runs for one second, and then both pumps run simultaneously for one second, after which the loop repeats.

[!TIP]
Since it is difficult to tell whether the pumps are turning in the correct directions given their corresponding input channel (i.e., positive pump should move air into the reservoir and negative pump should move air out), it's best to also try the `test_pump_control` test once the pressure sensors have been integrated and are functional.