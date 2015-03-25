
#define LIGHT_DINING_1		1			
#define LIGHT_DINING_2		2
#define LIGHT_DINING_3		3			
#define LIGHT_STAIRS		4
#define	DHT_SENSOR			5 // define DHT_SENSOR with highest index



#define SOULISS_GLOBALS			

#define SOULISS_DEFINE_TYPICALS Set_LightsGroup(LIGHT_DINING_1, LIGHT_DINING_3);\
								Set_SimpleLight(LIGHT_STAIRS);

#define SOULISS_READ_INPUTS 	ssLightsGroupIN(IN1, LIGHT_DINING_1, LIGHT_DINING_3);\
								DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT_STAIRS);

#define SOULISS_PROCESS_LOGICS 	Logic_LightsGroup(LIGHT_DINING_1, LIGHT_DINING_3);\
								Logic_SimpleLight(LIGHT_STAIRS);

#define SOULISS_SET_OUTPUTS 	DigOut(RELAY1, Souliss_T1n_Coil, LIGHT_DINING_1);\
								DigOut(RELAY2, Souliss_T1n_Coil, LIGHT_DINING_2);\
								DigOut(RELAY3, Souliss_T1n_Coil, LIGHT_DINING_3);\
								DigOut(RELAY4, Souliss_T1n_Coil, LIGHT_STAIRS);

#define SOULISS_PROCESS_TIMERS 	Timer_LightsGroup(LIGHT_DINING_1, LIGHT_DINING_3);\
								Timer_SimpleLight(LIGHT_STAIRS);