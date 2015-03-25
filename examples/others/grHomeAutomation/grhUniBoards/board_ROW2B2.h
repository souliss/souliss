#define WINDOW_LIVING		1			
#define WINDOW_KITCHEN		2	
#define	DHT_SENSOR			3 // define DHT_SENSOR with highest index

#define SOULISS_GLOBALS

#define SOULISS_DEFINE_TYPICALS	Souliss_SetT22(memory_map, WINDOW_LIVING);\
								Souliss_SetT22(memory_map, WINDOW_KITCHEN);


#define SOULISS_READ_INPUTS 	DigInWindowToggle(IN1, WINDOW_LIVING);\
								DigInWindowToggle(IN2, WINDOW_KITCHEN);

#define SOULISS_PROCESS_LOGICS 	Souliss_Logic_T22(memory_map, WINDOW_LIVING, &data_changed, SHUTTER_SHORT_TIMEOUT);\
								Souliss_Logic_T22(memory_map, WINDOW_KITCHEN, &data_changed, SHUTTER_LONG_TIMEOUT);

#define SOULISS_SET_OUTPUTS 	DigOut(RELAY1, Souliss_T2n_Coil_Open,  WINDOW_LIVING);\
								DigOut(RELAY2, Souliss_T2n_Coil_Close, WINDOW_LIVING);\
								DigOut(RELAY3, Souliss_T2n_Coil_Open,  WINDOW_KITCHEN);\
								DigOut(RELAY4, Souliss_T2n_Coil_Close, WINDOW_KITCHEN);

#define SOULISS_PROCESS_TIMERS 	Timer_Windows(WINDOW_LIVING);\
								Timer_Windows(WINDOW_KITCHEN);
