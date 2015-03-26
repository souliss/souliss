#define WINDOW_BATH1		1			
#define WINDOW_BEDROOM2		2	
#define	DHT_SENSOR			3 // define DHT_SENSOR with highest index

#define SOULISS_GLOBALS

#define SOULISS_DEFINE_TYPICALS	Souliss_SetT22(memory_map, WINDOW_BATH1);\
								Souliss_SetT22(memory_map, WINDOW_BEDROOM2);

#define SOULISS_READ_INPUTS 	DigInWindowToggle(IN1, WINDOW_BATH1);\
								DigInWindowToggle(IN2, WINDOW_BEDROOM2);

#define SOULISS_PROCESS_LOGICS 	Souliss_Logic_T22(memory_map, WINDOW_BATH1, &data_changed, SHUTTER_SHORT_TIMEOUT);\
								Souliss_Logic_T22(memory_map, WINDOW_BEDROOM2, &data_changed, SHUTTER_SHORT_TIMEOUT);

#define SOULISS_SET_OUTPUTS 	DigOut(RELAY1, Souliss_T2n_Coil_Open,  WINDOW_BATH1);\
								DigOut(RELAY2, Souliss_T2n_Coil_Close, WINDOW_BATH1);\
								DigOut(RELAY3, Souliss_T2n_Coil_Open,  WINDOW_BEDROOM2);\
								DigOut(RELAY4, Souliss_T2n_Coil_Close, WINDOW_BEDROOM2);

#define SOULISS_PROCESS_TIMERS 	Timer_Windows(WINDOW_BATH1);\
								Timer_Windows(WINDOW_BEDROOM2);