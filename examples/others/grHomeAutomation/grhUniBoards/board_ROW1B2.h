
#define LIGHT_KITCHEN_1		1			
#define LIGHT_KITCHEN_2		2
#define LIGHT_KITCHEN_3		3			
#define LIGHT_GW2			4
#define	DHT_SENSOR			5 // define DHT_SENSOR with highest index





#define SOULISS_GLOBALS			

#define SOULISS_DEFINE_TYPICALS Set_LightsGroup(LIGHT_KITCHEN_1, LIGHT_KITCHEN_3);\
								Set_SimpleLight(LIGHT_GW2);

#define SOULISS_READ_INPUTS 	ssLightsGroupIN(IN1, LIGHT_KITCHEN_1, LIGHT_KITCHEN_3);\
								DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT_GW2);	

#define SOULISS_PROCESS_LOGICS 	Logic_LightsGroup(LIGHT_KITCHEN_1, LIGHT_KITCHEN_3);\
								Logic_SimpleLight(LIGHT_GW2);

#define SOULISS_SET_OUTPUTS 	DigOut(RELAY1, Souliss_T1n_Coil, LIGHT_KITCHEN_1);\
								DigOut(RELAY2, Souliss_T1n_Coil, LIGHT_KITCHEN_2);\
								DigOut(RELAY3, Souliss_T1n_Coil, LIGHT_KITCHEN_3);\
								DigOut(RELAY4, Souliss_T1n_Coil, LIGHT_GW2);

#define SOULISS_PROCESS_TIMERS 	Timer_LightsGroup(LIGHT_KITCHEN_1, LIGHT_KITCHEN_3);\
								Timer_SimpleLight(LIGHT_GW2);
