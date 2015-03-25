#define LIGHT_BALCONY2		1			
#define LIGHT_BEDROOM3		2
#define WINDOW_DINING		3
#define	DHT_SENSOR			4 // define DHT_SENSOR with highest index

#define SOULISS_GLOBALS

#define SOULISS_DEFINE_TYPICALS Set_SimpleLight(LIGHT_BALCONY2);\
								Set_SimpleLight(LIGHT_BEDROOM3);\
								Souliss_SetT22(memory_map, WINDOW_DINING);

#define SOULISS_READ_INPUTS 	DigIn(IN1, Souliss_T1n_ToggleCmd, LIGHT_BALCONY2);\
								DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT_BEDROOM3);\
								DigInWindowToggle(IN3, WINDOW_DINING);	

#define SOULISS_PROCESS_LOGICS 	Logic_SimpleLight(LIGHT_BALCONY2);\
								Logic_SimpleLight(LIGHT_BEDROOM3);\
								Souliss_Logic_T22(memory_map, WINDOW_DINING, &data_changed, SHUTTER_SHORT_TIMEOUT);

#define SOULISS_SET_OUTPUTS 	DigOut(RELAY1, Souliss_T1n_Coil, LIGHT_BALCONY2);\
								DigOut(RELAY2, Souliss_T1n_Coil, LIGHT_BEDROOM3);\
								DigOut(RELAY3, Souliss_T2n_Coil_Open,  WINDOW_DINING);\
								DigOut(RELAY4, Souliss_T2n_Coil_Close, WINDOW_DINING);

#define SOULISS_PROCESS_TIMERS 	Timer_SimpleLight(LIGHT_BALCONY2);\
								Timer_SimpleLight(LIGHT_BEDROOM3);\
								Timer_Windows(WINDOW_DINING);
