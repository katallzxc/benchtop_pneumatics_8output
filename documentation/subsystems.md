# Apparatus Description

The multi-channel pneumatics system maintains three reference pressure levels (usually, positive, negative, and neutral) as inputs and allows up to eight output lines to be pressurized using a reference pressure level and the common line that connects inputs and outputs. Each of the three input lines can be opened or closed to connect them to or disconnect them from the common line.

The neutral pressure level is open to the atmosphere and hence always maintains a pressure of 0 kPa relative to environmental conditions. The positive and negative pressure levels, conversely, are pressurized by using pumps to transfer air into or out of air reservoirs, raising or lowering the internal reservoir pressure.

The workflow to actuate a pneumatic device as an output is as follows:

1. Begin with all input and output valves closed so that nothing is connected to the shared line.
2. Set the input connection to either the negative input (if the desired output pressure is below 0 kPa) or the positive input (if the desired output pressure is above 0 kPa).
3. Run the pump for the selected input line until the air reservoir contains air at the desired output pressure.
4. Open the valve connecting the shared line to the neutral pressure level to neutralize the shared line.
5. Close the valve between the neutral pressure level and the shared line, disconnecting the shared line from the environmental pressure conditions.
6. Open the valve connecting the shared line to the selected input line to bring the shared line to the desired output pressure.
7. Once pressure in the shared line has reached the desired output pressure, open the valve connecting the shared line to the desired output device.
8. Keeping the selected input line and the desired output line open to the shared line, run the pump for the selected input line as needed to regulate the pressure in the system.

The pneumatics system can be divided into four main subsystems:

1. Pressure regulation subsystem
2. Valve subsystem
3. Pressure sensor subsystem
4. Structural subsystem

The first subsystem, pressure regulation, uses only the input pressure lines. The other subsystems relate to both inputs and outputs.

## Pressure regulation subsystem

The pressure regulation subsystem maintains pressure setpoints in the positive and negative input air reservoirs. This subsystem involves multiple components:

1. The **air reservoirs** hold air and are used to maintain a reservoir of air pressurized to a set level. The two reservoirs in this system are designated as “positive” and “negative”.
2. The **air pumps** move air into or out of the air reservoirs.
3. The **motor driver** controls the power at which the pumps operate and sends driving signals to the pumps.
4. The **Arduino microcontroller** sends control signals to the driver turn the pumps on or off and to change the power at which the pumps operate. 

## Valve subsystem

The valve subsystem consists of the 11 valves (3 input, 8 output) used to control connections between the input/output channels and the common channel, as well as the Arduino microcontroller board used to control these valves and a 5V power input.

The valves are 2-way solenoid valves (RLM204P30B, Asco) and normally closed (meaning that they are closed at 0 VDC and open at 5 VDC). The control circuit for the valves uses NPN transistors (SS8050, Fairchild Semiconductor Corporation) to direct a 5V input flow to or away from the valves based on a logic signal from the Arduino microcontroller board.

An example of a PCB layout for one of the valve circuits made by Kurtis Laqua is shown below.

![InputValvePCB](..\figs\3_valve_board_image.png)

#TODO: confirm 5V from L298N onboard regulator & fill out here

## Pressure sensor subsystem

The pressure sensor subsystem consists of the 10 sensors (2 input, 8 output) used to read pressure levels in the positive input, negative input, and output channels and the common channel, as well as the Arduino microcontroller board used to take these readings.

The sensors (ADP5101, Panasonic) have a range of +/-100 kPa and are active sensors (meaning that they require a voltage input, which in this case is 5 VDC). The control circuit for the sensors connects the 5V output from the Arduino microcontroller board to the sensor power input and connects the Arduino input pin to the sensor data output.

An example of a PCB layout for one of the pressure sensor circuits made by Kurtis Laqua is shown below.

![SensorPCB](..\figs\sensor_board_image.png)

## Structural subsystem

The structural subsystem consists of pneumatic and mechanical connectors. Pneumatic connectors include tubes and luer fittings connecting the input pumps and output connectors to the shared channel. Mechanical connectors mainly consist of bolts and standoffs used to connect components and circuits to sheets of acrylic, with the base sheet holding the pumps and reservoirs, a sheet above the pumps and reservoirs holding the majority of the remaining components, and a few small raised platforms holding power switches and connectors.

# System Architecture

The following image shows an alternate, more granular system architecture.

![SystemArchitecture](..\figs\SystemArchitecture.jpg)