#define LIGHT_BATH1_1		1			
#define LIGHT_BATH1_2		2
#define LIGHT_BATH1_3		3
#define LIGHT_GW1			4
#define	DHT_SENSOR			5 // define DHT_SENSOR with highest index


#define SOULISS_GLOBALS			

#define SOULISS_DEFINE_TYPICALS	Set_LightsGroup(LIGHT_BATH1_1, LIGHT_BATH1_3);\
								Set_SimpleLight(LIGHT_GW1);

#define SOULISS_READ_INPUTS 	ssLightsGroupIN(IN1, LIGHT_BATH1_1, LIGHT_BATH1_3);\
								DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT_GW1);

#define SOULISS_PROCESS_LOGICS 	Logic_LightsGroup(LIGHT_BATH1_1, LIGHT_BATH1_3);\
								Logic_SimpleLight(LIGHT_GW1);

#define SOULISS_SET_OUTPUTS 	DigOut(RELAY1, Souliss_T1n_Coil, LIGHT_BATH1_1);\
								DigOut(RELAY2, Souliss_T1n_Coil, LIGHT_BATH1_2);\
								DigOut(RELAY3, Souliss_T1n_Coil, LIGHT_BATH1_3);\
								DigOut(RELAY4, Souliss_T1n_Coil, LIGHT_GW1);

#define SOULISS_PROCESS_TIMERS 	Timer_LightsGroup(LIGHT_BATH1_1, LIGHT_BATH1_3);\
								Timer_SimpleLight(LIGHT_GW1);