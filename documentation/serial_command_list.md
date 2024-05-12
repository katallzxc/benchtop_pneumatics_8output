# Serial commands for pneumatics apparatus

## Communication protocol

### Component indices
Individual components are referenced in commands by integer IDs that reference the index of the component within an enum variable assignment. Component indices are assigned as follows:

**Pumps**:
0. NEG
1. POS

**Input valves**:
0. NEG
1. NEU
2. POS

**Input pressure sensors**:
0. NEG
1. NEU
2. POS

**Output valves**:
Integers in the 0-7 range used (in order) as indices for outputs 1-8

**Output pressure sensors**:
Integers in the 0-7 range used (in order) as indices for outputs 1-8

### Component states and setpoints
Actuator components (pumps or valves) have associated binary states.

For pumps, the states are:
0. OFF
1. ON

For valves, the states are:
0. CLOSED
1. OPEN

Pumps take a *setpoint* value in addition to the aforementioned state value. The setpoint defines the pressure that a specific input channel should maintain. For instance, the NEG pump might receive a setpoint of -40 kPa--in this case, the pump will turn ON when the NEG pressure sensor value (reading the pressure in the NEG input channel) is greater than -40 kPa. The pump will turn OFF once the NEG pressure sensor value falls to or below -40 kPa.

### Command string format
All commands are provided in a string with "<" as the start marker character and ">" as the end marker character. Each command consists of three parts separated by a comma and space (", "):
1. Command string (two letters)
2. Component index (integer)
3. State or setpoint (integer)
If any of this three parts are not applicable to a specific command (e.g., if using a command that applies to *all* input/output elements, where a component index is not necessary), the default value 999 should be used.

## Command list
| Command | Format | Action |
| ----------- | ----------- | ----------- |
| SI | <SI, valve #, valve state> | Sets the state of a single input valve. |
| SO | <SO, valve #, valve state> | Sets the state of a single output valve. |
| AI | <AI, 999, valve state> | Sets the state of all input valves. |
| AO | <AO, 999, valve state> | Sets the state of all output valves. |
| GI | <GI, sensor #, 999> | Returns ("gets") the pressure sensor reading for a single input channel. |
| GO | <GO, sensor #, 999> | Returns ("gets") the pressure sensor reading for a single output channel. |
| VI | <VI, valve #, 999> | Returns the current state of a single input valve. |
| VO | <VO, valve #, 999> | Returns the current state of a single output valve. |
| RS | <RS, pump #, pump setpoint> | Sets the setpoint for a single pump (and hence for the corresponding input channel). |
| RG | <RG, pump #, 999> | Returns the current setpoint for a single pump. |
| PS | <PS, pump #, pump state> | Sets the state of a single pump. |
| PG | <PG, pump #, 999> | Returns the current state of a single pump. |