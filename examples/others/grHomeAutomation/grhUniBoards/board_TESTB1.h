#define LIGHT_LOFT_1            1                       
#define LIGHT_LOFT_2            2
#define LIGHT_TERRACE_1                 3
#define LIGHT_TERRACE_2                 4
#define LIGHT_TERRACE_3                 5
#define LIGHT_TOILET                    6

#define TEMP_BOILER_ACS                 7
#define TEMP_BOILER_ACS_H               8
#define TEMP_BOILER_HEATING             9
#define TEMP_BOILER_HEATING_H   10
#define TEMP_BOILER_BOTTOM              11
#define TEMP_BOILER_BOTTOM_H    12

#define HEATPUMP_AUTO                           13
#define HEATPUMP_REMOTE_SWITCH          14
#define HEATPUMP_CIRCULATION_PUMP       15
#define HEATPUMP_ACS_REQUEST            16
#define HEATPUMP_COOL                           17      

#define TEMP_HEATING_FLOW                       18      // floor heating system water temp
#define TEMP_HEATING_FLOW_H                     19
#define TEMP_HEATING_RETURN                     20      // floor heating system water return temp
#define TEMP_HEATING_RETURN_H           21

#define HEATING_KITCHEN         22
#define HEATING_BATH2           23
#define HEATING_BED3            24
#define HEATING_LIVING          25
#define HEATING_BED2            26
#define HEATING_BATH1           27
#define HEATING_BED1            28
#define HEATING_LOFT            29

#define HEATING_MIX_VALVE_SWITCH	30
#define HEATING_MIX_VALVE_DIRECTION	31

#define TEMP_BOILER_ACS_PIN             0       // analog pin used
#define TEMP_BOILER_HEATING_PIN 1       // analog pin used
#define TEMP_BOILER_BOTTOM_PIN  2       // analog pin used
#define TEMP_HEATING_FLOW_PIN   3       // analog pin used
#define TEMP_HEATING_RETURN_PIN 4       // analog pin used

#define TEMP_BOILER_ACS_PAD_RESISTANCE          9840 // measured
#define TEMP_BOILER_HEATING_PAD_RESISTANCE      9850 // measured
#define TEMP_BOILER_BOTTOM_PAD_RESISTANCE       9930 // measured
#define TEMP_HEATING_FLOW_PAD_RESISTANCE        9800 // measured
#define TEMP_HEATING_RETURN_PAD_RESISTANCE      9870 // measured


// DEFINE OUTPUT PINs
// ** DO NOT FORGET TO SET pinMode to OUTPUT **
#define HEATING_MIX_VALVE_SWITCH_PIN			28
#define HEATING_MIX_VALVE_DIRECTION_PIN			29
#define LIGHT_LOFT_1_PIN                        30
#define LIGHT_LOFT_2_PIN                        31
#define LIGHT_TERRACE_1_PIN                     32
#define LIGHT_TERRACE_2_PIN                     33
#define LIGHT_TERRACE_3_PIN                     34
#define LIGHT_TOILET_PIN                        35
#define HEATPUMP_REMOTE_SWITCH_PIN              36
#define HEATPUMP_CIRCULATION_PUMP_PIN   		37
#define HEATPUMP_ACS_REQUEST_PIN                38
#define HEATPUMP_COOL_PIN                       39
#define ZONE_SWITCH_BED_1_PIN                   40
#define ZONE_SWITCH_BED_2_PIN                   41
#define ZONE_SWITCH_BED_3_PIN                   42
#define ZONE_SWITCH_BATH_1_PIN                  43
#define ZONE_SWITCH_BATH_2_PIN                  44
#define ZONE_SWITCH_KITCHEN_PIN                 45
#define ZONE_SWITCH_LIVING_PIN                  46
#define ZONE_SWITCH_LOFT_PIN                    47
#define LIGHT_LOFT_1_PIN_IN                     48
#define LIGHT_LOFT_2_PIN_IN                     49
#define LIGHT_TERRACE_1_PIN_IN                  50
#define LIGHT_TERRACE_2_PIN_IN                  51
#define LIGHT_TERRACE_3_PIN_IN                  52
#define LIGHT_TOILET_PIN_IN                     53



#define SETPOINT_ACS            42 // °C
#define SETPOINT_HEATING        29 // °C
#define SETPOINT_DEADBAND       2


#define DEFINE_PIN_MODE		pinMode(LIGHT_LOFT_1_PIN_IN, INPUT_PULLUP);\
							pinMode(LIGHT_LOFT_2_PIN_IN, INPUT_PULLUP);\
							pinMode(LIGHT_TERRACE_1_PIN_IN, INPUT_PULLUP);\
							pinMode(LIGHT_TERRACE_2_PIN_IN, INPUT_PULLUP);\
							pinMode(LIGHT_TERRACE_3_PIN_IN, INPUT_PULLUP);\
							pinMode(LIGHT_TOILET_PIN_IN, INPUT_PULLUP);\
							digitalWrite(HEATING_MIX_VALVE_SWITCH_PIN, HIGH);\ 
							digitalWrite(HEATING_MIX_VALVE_DIRECTION_PIN, HIGH);\
							digitalWrite(LIGHT_LOFT_1_PIN, HIGH);\ 
							digitalWrite(LIGHT_LOFT_2_PIN, HIGH);\
							digitalWrite(LIGHT_TERRACE_1_PIN, HIGH);\
							digitalWrite(LIGHT_TERRACE_2_PIN, HIGH);\
							digitalWrite(LIGHT_TERRACE_3_PIN, HIGH);\
							digitalWrite(LIGHT_TOILET_PIN, HIGH);\
							digitalWrite(HEATPUMP_REMOTE_SWITCH_PIN, HIGH);\
							digitalWrite(HEATPUMP_CIRCULATION_PUMP_PIN, HIGH);\
							digitalWrite(HEATPUMP_ACS_REQUEST_PIN, HIGH);\
							digitalWrite(HEATPUMP_COOL_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_BED_1_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_BED_2_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_BED_3_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_BATH_1_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_BATH_2_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_KITCHEN_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_LIVING_PIN, HIGH);\
							digitalWrite(ZONE_SWITCH_LOFT_PIN, HIGH);\
							pinMode(HEATING_MIX_VALVE_SWITCH_PIN, OUTPUT);\
							pinMode(HEATING_MIX_VALVE_DIRECTION_PIN, OUTPUT);\
							pinMode(LIGHT_LOFT_1_PIN, OUTPUT);\
							pinMode(LIGHT_LOFT_2_PIN, OUTPUT);\
							pinMode(LIGHT_TERRACE_1_PIN, OUTPUT);\
							pinMode(LIGHT_TERRACE_2_PIN, OUTPUT);\
							pinMode(LIGHT_TERRACE_3_PIN, OUTPUT);\
							pinMode(LIGHT_TOILET_PIN, OUTPUT);\
							pinMode(HEATPUMP_REMOTE_SWITCH_PIN, OUTPUT);\
							pinMode(HEATPUMP_CIRCULATION_PUMP_PIN, OUTPUT);\
							pinMode(HEATPUMP_ACS_REQUEST_PIN, OUTPUT);\
							pinMode(HEATPUMP_COOL_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_BED_1_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_BED_2_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_BED_3_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_BATH_1_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_BATH_2_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_KITCHEN_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_LIVING_PIN, OUTPUT);\
							pinMode(ZONE_SWITCH_LOFT_PIN, OUTPUT);\

#define SOULISS_GLOBALS                 

#define SOULISS_DEFINE_TYPICALS Set_SimpleLight(LIGHT_LOFT_1);\
                                                                Set_SimpleLight(LIGHT_LOFT_2);\
                                                                Set_SimpleLight(LIGHT_TERRACE_1);\
                                                                Set_SimpleLight(LIGHT_TERRACE_2);\
                                                                Set_SimpleLight(LIGHT_TERRACE_3);\
                                                                Set_SimpleLight(LIGHT_TOILET);\
                                                                Souliss_SetT11(memory_map, HEATPUMP_AUTO);\
                                                                Souliss_SetT11(memory_map, HEATPUMP_REMOTE_SWITCH);\
                                                                Souliss_SetT11(memory_map, HEATPUMP_CIRCULATION_PUMP);\
                                                                Souliss_SetT11(memory_map, HEATPUMP_ACS_REQUEST);\
                                                                Souliss_SetT11(memory_map, HEATPUMP_COOL);\
                                                                Set_Temperature(TEMP_BOILER_ACS);\
                                                                Set_Temperature(TEMP_BOILER_HEATING);\
                                                                Set_Temperature(TEMP_BOILER_BOTTOM);\
                                                                Set_Temperature(TEMP_HEATING_FLOW);\
                                                                Set_Temperature(TEMP_HEATING_RETURN);\
                                                                Souliss_SetT11(memory_map, HEATING_KITCHEN);\
                                                                Souliss_SetT11(memory_map, HEATING_BATH2);\
                                                                Souliss_SetT11(memory_map, HEATING_BED3);\
                                                                Souliss_SetT11(memory_map, HEATING_LIVING);\
                                                                Souliss_SetT11(memory_map, HEATING_BED2);\
                                                                Souliss_SetT11(memory_map, HEATING_BATH1);\
                                                                Souliss_SetT11(memory_map, HEATING_BED1);\
                                                                Souliss_SetT11(memory_map, HEATING_LOFT);\
                                                                Souliss_SetT11(memory_map, HEATING_MIX_VALVE_SWITCH);\
																Souliss_SetT11(memory_map, HEATING_MIX_VALVE_DIRECTION);\

#define SOULISS_READ_INPUTS     Souliss_LowDigIn(LIGHT_LOFT_1_PIN_IN, Souliss_T1n_ToggleCmd, memory_map, LIGHT_LOFT_1, true);\
                                                                Souliss_LowDigIn(LIGHT_LOFT_2_PIN_IN, Souliss_T1n_ToggleCmd, memory_map, LIGHT_LOFT_2, true);\
                                                                Souliss_LowDigIn(LIGHT_TERRACE_1_PIN_IN, Souliss_T1n_ToggleCmd, memory_map, LIGHT_TERRACE_1, true);\
                                                                Souliss_LowDigIn(LIGHT_TERRACE_2_PIN_IN, Souliss_T1n_ToggleCmd, memory_map, LIGHT_TERRACE_2, true);\
                                                                Souliss_LowDigIn(LIGHT_TERRACE_3_PIN_IN, Souliss_T1n_ToggleCmd, memory_map, LIGHT_TERRACE_3, true);\
                                                                Souliss_LowDigIn(LIGHT_TOILET_PIN_IN, Souliss_T1n_ToggleCmd, memory_map, LIGHT_TOILET, true);\

#define SOULISS_PROCESS_LOGICS  Logic_SimpleLight(LIGHT_LOFT_1);\
                                                                Logic_SimpleLight(LIGHT_LOFT_2);\
                                                                Logic_SimpleLight(LIGHT_TERRACE_1);\
                                                                Logic_SimpleLight(LIGHT_TERRACE_2);\
                                                                Logic_SimpleLight(LIGHT_TERRACE_3);\
                                                                Logic_SimpleLight(LIGHT_TOILET);\
                                                                Souliss_Logic_T11(memory_map, HEATPUMP_AUTO, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATPUMP_REMOTE_SWITCH, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATPUMP_CIRCULATION_PUMP, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATPUMP_ACS_REQUEST, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATPUMP_COOL, &data_changed);\
                                                                Logic_Temperature(TEMP_BOILER_ACS);\
                                                                Logic_Temperature(TEMP_BOILER_HEATING);\
                                                                Logic_Temperature(TEMP_BOILER_BOTTOM);\
                                                                Logic_Temperature(TEMP_HEATING_FLOW);\
                                                                Logic_Temperature(TEMP_HEATING_RETURN);\
                                                                Souliss_Logic_T11(memory_map, HEATING_KITCHEN, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_BATH2, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_BED3, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_LIVING, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_BED2, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_BATH1, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_BED1, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_LOFT, &data_changed);\
                                                                Souliss_Logic_T11(memory_map, HEATING_MIX_VALVE_SWITCH, &data_changed);\
																Souliss_Logic_T11(memory_map, HEATING_MIX_VALVE_DIRECTION, &data_changed);\


#define SOULISS_SET_OUTPUTS		LowDigOut(LIGHT_LOFT_1_PIN, Souliss_T1n_Coil, LIGHT_LOFT_1);\
								LowDigOut(LIGHT_LOFT_2_PIN, Souliss_T1n_Coil, LIGHT_LOFT_2);\
								LowDigOut(LIGHT_TERRACE_1_PIN, Souliss_T1n_Coil, LIGHT_TERRACE_1);\
								LowDigOut(LIGHT_TERRACE_2_PIN, Souliss_T1n_Coil, LIGHT_TERRACE_2);\
								LowDigOut(LIGHT_TERRACE_3_PIN, Souliss_T1n_Coil, LIGHT_TERRACE_3);\
								LowDigOut(LIGHT_TOILET_PIN, Souliss_T1n_Coil, LIGHT_TOILET);\
								LowDigOut(HEATPUMP_REMOTE_SWITCH_PIN, Souliss_T1n_Coil, HEATPUMP_REMOTE_SWITCH);\
								LowDigOut(HEATPUMP_CIRCULATION_PUMP_PIN, Souliss_T1n_Coil, HEATPUMP_CIRCULATION_PUMP);\
								LowDigOut(HEATPUMP_ACS_REQUEST_PIN, Souliss_T1n_Coil, HEATPUMP_ACS_REQUEST);\
								LowDigOut(HEATPUMP_COOL_PIN, Souliss_T1n_Coil, HEATPUMP_COOL);\
								LowDigOut(ZONE_SWITCH_KITCHEN_PIN, Souliss_T1n_Coil, HEATING_KITCHEN);\
								LowDigOut(ZONE_SWITCH_BATH_2_PIN, Souliss_T1n_Coil, HEATING_BATH2);\
								LowDigOut(ZONE_SWITCH_BED_3_PIN, Souliss_T1n_Coil, HEATING_BED3);\
								LowDigOut(ZONE_SWITCH_LIVING_PIN, Souliss_T1n_Coil, HEATING_LIVING);\
								LowDigOut(ZONE_SWITCH_BED_2_PIN, Souliss_T1n_Coil, HEATING_BED2);\
								LowDigOut(ZONE_SWITCH_BATH_1_PIN, Souliss_T1n_Coil, HEATING_BATH1);\
								LowDigOut(ZONE_SWITCH_BED_1_PIN, Souliss_T1n_Coil, HEATING_BED1);\
								LowDigOut(ZONE_SWITCH_LOFT_PIN, Souliss_T1n_Coil, HEATING_LOFT);\
								LowDigOut(HEATING_MIX_VALVE_SWITCH_PIN, Souliss_T1n_Coil, HEATING_MIX_VALVE_SWITCH);\
								LowDigOut(HEATING_MIX_VALVE_DIRECTION_PIN, Souliss_T1n_Coil, HEATING_MIX_VALVE_DIRECTION);\


#define SOULISS_PROCESS_TIMERS	Timer_SimpleLight(LIGHT_LOFT_1);\
								Timer_SimpleLight(LIGHT_LOFT_2);\
								Timer_SimpleLight(LIGHT_TERRACE_1);\
								Timer_SimpleLight(LIGHT_TERRACE_2);\
								Timer_SimpleLight(LIGHT_TERRACE_3);\
								Timer_SimpleLight(LIGHT_TOILET);\
								Souliss_T11_Timer(memory_map, HEATPUMP_REMOTE_SWITCH);\
								Souliss_T11_Timer(memory_map, HEATPUMP_CIRCULATION_PUMP);\
								Souliss_T11_Timer(memory_map, HEATPUMP_ACS_REQUEST);\
								Souliss_T11_Timer(memory_map, HEATPUMP_COOL);\
								float temperature_acs = NTC_RawADCToCelsius( analogRead(TEMP_BOILER_ACS_PIN),TEMP_BOILER_ACS_PAD_RESISTANCE );\
								Souliss_ImportAnalog(memory_map, TEMP_BOILER_ACS, &temperature_acs);\
								float temperature_heating = NTC_RawADCToCelsius( analogRead(TEMP_BOILER_HEATING_PIN),TEMP_BOILER_HEATING_PAD_RESISTANCE );\
								Souliss_ImportAnalog(memory_map, TEMP_BOILER_HEATING, &temperature_heating);\
								float temperature_bottom = NTC_RawADCToCelsius( analogRead(TEMP_BOILER_BOTTOM_PIN),TEMP_BOILER_BOTTOM_PAD_RESISTANCE );\
								Souliss_ImportAnalog(memory_map, TEMP_BOILER_BOTTOM, &temperature_bottom);\
								float temperature_heating_flow = NTC_RawADCToCelsius( analogRead(TEMP_HEATING_FLOW_PIN),TEMP_HEATING_FLOW_PAD_RESISTANCE );\
								Souliss_ImportAnalog(memory_map, TEMP_HEATING_FLOW, &temperature_heating_flow);\
								float temperature_heating_return = NTC_RawADCToCelsius( analogRead(TEMP_HEATING_RETURN_PIN),TEMP_HEATING_RETURN_PAD_RESISTANCE );\
								Souliss_ImportAnalog(memory_map, TEMP_HEATING_RETURN, &temperature_heating_return);\                                                            
								if(mInput(HEATPUMP_AUTO)) {\
								        if (temperature_acs < SETPOINT_ACS - SETPOINT_DEADBAND)\
											mInput(HEATPUMP_ACS_REQUEST) = Souliss_T1n_OnCmd;\
										else if (temperature_acs > SETPOINT_ACS + SETPOINT_DEADBAND)\
											mInput(HEATPUMP_ACS_REQUEST) = Souliss_T1n_OffCmd;\
										if (temperature_heating < SETPOINT_HEATING - SETPOINT_DEADBAND)\
										{\
											mInput(HEATPUMP_CIRCULATION_PUMP) = Souliss_T1n_OnCmd;\
											mInput(HEATPUMP_COOL) = Souliss_T1n_OffCmd;\
										}\
										else if (temperature_heating > SETPOINT_HEATING + SETPOINT_DEADBAND)\
										{\
											mInput(HEATPUMP_CIRCULATION_PUMP) = Souliss_T1n_OffCmd;\
											mInput(HEATPUMP_COOL) = Souliss_T1n_OnCmd;\
										}\
								}\
								if( (mOutput(HEATING_KITCHEN)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_BATH2)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_BED3)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_LIVING)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_BED2)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_BATH1)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_BED1)==Souliss_T1n_Coil) ||\
									(mOutput(HEATING_LOFT)==Souliss_T1n_Coil) )\
								{\
									if (temperature_heating_flow > SETPOINT_HEATING + SETPOINT_DEADBAND)\
									{\
										mInput(HEATING_MIX_VALVE_SWITCH) = Souliss_T1n_OnCmd;\
										mInput(HEATING_MIX_VALVE_DIRECTION) = Souliss_T1n_OffCmd;\
									}\
									else if (temperature_heating_flow < SETPOINT_HEATING - SETPOINT_DEADBAND)\
									{\
										mInput(HEATING_MIX_VALVE_SWITCH) = Souliss_T1n_OnCmd;\
										mInput(HEATING_MIX_VALVE_DIRECTION) = Souliss_T1n_OnCmd;\
									}\	
									else\
									{\
										mInput(HEATING_MIX_VALVE_SWITCH) = Souliss_T1n_OffCmd;\
										mInput(HEATING_MIX_VALVE_DIRECTION) = Souliss_T1n_OffCmd;\
									}\																		
								}\
								else\
								{\
									mInput(HEATING_MIX_VALVE_SWITCH) = Souliss_T1n_OffCmd;\
									mInput(HEATING_MIX_VALVE_DIRECTION) = Souliss_T1n_OffCmd;\
								}\																
								Souliss_T11_Timer(memory_map, HEATING_KITCHEN);\
								Souliss_T11_Timer(memory_map, HEATING_BATH2);\
								Souliss_T11_Timer(memory_map, HEATING_BED3);\
								Souliss_T11_Timer(memory_map, HEATING_LIVING);\
								Souliss_T11_Timer(memory_map, HEATING_BED2);\
								Souliss_T11_Timer(memory_map, HEATING_BATH1);\
								Souliss_T11_Timer(memory_map, HEATING_BED1);\
								Souliss_T11_Timer(memory_map, HEATING_LOFT);\
								Souliss_T11_Timer(memory_map, HEATING_MIX_VALVE_SWITCH);\
								Souliss_T11_Timer(memory_map, HEATING_MIX_VALVE_DIRECTION);\