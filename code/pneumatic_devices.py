''' PNEUMATIC_DEVICES v1.0 - Katherine Allison

Created: 2024-02-15
Updated: 2024-02-18

Defines object for pneumatic device control communication.
'''
import serial
import time

class PneumaticConnection:
    TERMINATOR = '\r'.encode('UTF8')
    FILLER_STRING = 99
    
    def __init__(self, device='COM7', baud=19200, timeout=1):
        self.serial = serial.Serial(device, baud, timeout=timeout)
        self.pos_string = "POS"
        self.neg_string = "NEG"
        self.neu_string = "NEU"
        self.input_strings = [self.pos_string,self.neg_string,self.neu_string]
        self.base_out_string = "OUT"
        self.ON,self.OFF = True,False
        self.OPEN,self.CLOSED = True,False

    def set_indices(self,valves,sensors,pumps):
        self.valves = valves
        self.sensors = sensors
        self.pumps = pumps

    def assemble_command(self,command_code, id=None, val=None,print_command=False):
        if not (isinstance(val,int) or isinstance(id,int)):
            print("Incorrect serial command call for pneumatics")
            raise TypeError
        elif not isinstance(id,int):
            id = PneumaticConnection.FILLER_STRING
        elif not isinstance(val,int):
            val = PneumaticConnection.FILLER_STRING
        command_string = '<{0},{1},{2}>'.format(command_code,id,val)
        if print_command: print(command_string)
        return command_string 

    def receive(self) -> str:
        line = self.serial.read_until(self.TERMINATOR)
        return line.decode('UTF8').strip()

    def send(self, text:str) -> bool:
        line = '%s\n'%(text)
        self.serial.write(line.encode('UTF8'))
        echo = self.receive()
        return text == echo
    
    def set_single_valve(self,valve_string,valve_state):
        # define serial commands and get command string
        SET_SINGLE_IN_VALVE = "SI"  # command format: <SI, valve #, valve state>
        SET_SINGLE_OUT_VALVE = "SO" # command format: <SO, valve #, valve state>
        if valve_string in self.input_strings:
            command_str = SET_SINGLE_IN_VALVE
        else:
            command_str = SET_SINGLE_OUT_VALVE

        # send serial command
        valve_id = self.valves[valve_string]
        full_command = self.assemble_command(command_str,id=valve_id,val=valve_state)
        self.send(full_command)
    
    def set_valve_group(self,use_inputs,valve_state):
        # define serial commands and get command string
        SET_ALL_IN_VALVES = "AI"    # command format: <AI, 999, valve state>
        SET_ALL_OUT_VALVES = "AO"   # command format: <AO, 999, valve state>
        if use_inputs:
            command_str = SET_ALL_IN_VALVES
        else:
            command_str = SET_ALL_OUT_VALVES

        # send serial command
        full_command = self.assemble_command(command_str,val=valve_state)
        self.send(full_command)

    def get_valve_state(self,valve_string):
        # define serial commands and get command string
        GET_IN_VALVE_STATE = "VI"  # command format: <VI, valve #, 999>
        GET_OUT_VALVE_STATE = "VO" # command format: <VO, valve #, 999>
        if valve_string in self.input_strings:
            command_str = GET_IN_VALVE_STATE
        else:
            command_str = GET_OUT_VALVE_STATE

        # send serial command
        valve_id = self.valves[valve_string]
        full_command = self.assemble_command(command_str,id=valve_id)
        self.send(full_command)

    def get_pressure_value(self,sensor_string):
        # define serial commands and get command string
        GET_IN_PRESSURE = "GI"      # command format: <GI, sensor #, 999>
        GET_OUT_PRESSURE = "GO"     # command format: <GO, sensor #, 999>
        if sensor_string in self.input_strings:
            command_str = GET_IN_PRESSURE
        else:
            command_str = GET_OUT_PRESSURE

        # send serial command
        sensor_id = self.sensors[sensor_string]
        full_command = self.assemble_command(command_str,id=sensor_id)
        self.send(full_command)
        return self.receive()
    
    def set_reference_setpoint(self,pump_string,pump_setpt):
        # define serial commands and get command string
        SET_REF_SETPOINT = "RS"    # command format: <RS, pump #, pump setpoint>
        command_str = SET_REF_SETPOINT
        pump_id = self.pumps[pump_string]

        # send serial command
        full_command = self.assemble_command(command_str,id=pump_id,val=pump_setpt)
        self.send(full_command)
    
    def get_reference_setpoint(self,pump_string,pump_setpt):
        # define serial commands and get command string
        GET_REF_SETPOINT = "RG"    # command format: <RG, pump #, 999>
        command_str = GET_REF_SETPOINT
        pump_id = self.pumps[pump_string]

        # send serial command
        full_command = self.assemble_command(command_str,id=pump_id)
        self.send(full_command)
    
    def set_pump_state(self,pump_string,pump_state):
        # define serial commands and get command string
        SET_PUMP_STATE = "PS"    # command format: <PS, pump #, pump state>
        command_str = SET_PUMP_STATE
        pump_id = self.pumps[pump_string]

        # send serial command
        full_command = self.assemble_command(command_str,id=pump_id,val=pump_state)
        self.send(full_command)
    
    def get_pump_state(self,pump_string):
        # define serial commands and get command string
        GET_PUMP_STATE = "PG"    # command format: <PG, pump #, 999>
        command_str = GET_PUMP_STATE
        pump_id = self.pumps[pump_string]

        # send serial command
        full_command = self.assemble_command(command_str,id=pump_id)
        self.send(full_command)
    
    def get_pump_pressures(self,print_vals=False):
        neg_pump_pressure = self.get_pressure_value(self.neg_string)
        pos_pump_pressure = self.get_pressure_value(self.pos_string)
        if print_vals:
            print("Positive pump pressure: {0}\nNegative pump pressure: {1}".format(neg_pump_pressure,pos_pump_pressure))
        return [neg_pump_pressure,pos_pump_pressure]

    def switch_input_channel(self,valve_string,delay_time=5):
        # close all input valves then perform neutral evacuation
        self.set_valve_group(True, self.CLOSED)
        self.set_single_valve(self.neu_string, self.OPEN)
        time.sleep(delay_time)

        # open desired input valve
        self.set_single_valve(self.neu_string, self.CLOSED)
        if valve_string in self.input_strings:
            self.set_single_valve(valve_string, self.OPEN)
        else:
            print("Input channel switch routine called on non-input valve!")
            raise ValueError
    
    def test_connection(self,test_string,verbose=True):
        sent_successfully = self.send(test_string)
        if (not sent_successfully and verbose):
            print("Warning: sending data to Arduino microcontroller over serial appears to be broken")

        returned = self.receive()
        if verbose: 
            print("When %s sent to Arduino, Arduino returned %s." %(test_string,str(returned)))

    def close(self):
        self.serial.close()

def define_setup_indices(pneum_obj,num_out_channels=1):
    #define input-side pump indices
    pump_indices = {
        pneum_obj.neg_string:0,
        pneum_obj.pos_string:1
    }

    #define input-side valve indices
    valve_indices = {
        pneum_obj.neg_string:0,
        pneum_obj.neu_string:1,
        pneum_obj.pos_string:2,
    }

    #define input-side sensor indices
    sensor_indices = {
        pneum_obj.neg_string:0,
        pneum_obj.pos_string:1
    }

    # add in output_side valve and sensor indices
    out_channel_strings = [pneum_obj.base_out_string + str(i) for i in range(num_out_channels)]
    for i in range(num_out_channels):
        valve_indices[out_channel_strings[i]] = i
        sensor_indices[out_channel_strings[i]] = i

    return valve_indices,sensor_indices,pump_indices

if __name__ == "__main__":
    pneum = PneumaticConnection('COM7')#might need 19200 baud
    valve_inds,sensor_inds,pump_inds = define_setup_indices(pneum,1)
    pneum.set_indices(valve_inds,sensor_inds,pump_inds)
    
    for i in range (100):
        user_code = input("Enter command ")
        user_id = int(input("Enter ID "))
        user_val = int(input("Enter val "))
        print(pneum.send(pneum.assemble_command(user_code,user_id,user_val)))
        print(pneum.receive())