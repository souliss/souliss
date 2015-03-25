#define LIGHT_BALCONY1		1			
#define LIGHT_BEDROOM1		2
#define WINDOW_BEDROOM1		3
#define	DHT_SENSOR			4 // define DHT_SENSOR with highest index

#define SOULISS_GLOBALS			

#define SOULISS_DEFINE_TYPICALS	Set_SimpleLight(LIGHT_BALCONY1);\
								Set_SimpleLight(LIGHT_BEDROOM1);\
								Souliss_SetT22(memory_map, WINDOW_BEDROOM1);

#define SOULISS_READ_INPUTS 	ReadInput1(LIGHT_BALCONY1);\
								ReadInput2(LIGHT_BEDROOM1);\
								DigInWindowToggle(IN3, WINDOW_BEDROOM1);

#define SOULISS_PROCESS_LOGICS 	Logic_SimpleLight(LIGHT_BALCONY1);\
								Logic_SimpleLight(LIGHT_BEDROOM1);\
								Souliss_Logic_T22(memory_map, WINDOW_BEDROOM1, &data_changed, SHUTTER_LONG_TIMEOUT);

#define SOULISS_SET_OUTPUTS 	CntRelay1(LIGHT_BALCONY1);\
								CntRelay2(LIGHT_BEDROOM1);\
								ssDigOut(OUT3, Souliss_T2n_Coil_Open,  WINDOW_BEDROOM1);\
								ssDigOut(OUT4, Souliss_T2n_Coil_Close, WINDOW_BEDROOM1);

#define SOULISS_PROCESS_TIMERS 	Timer_SimpleLight(LIGHT_BALCONY1);\
								Timer_SimpleLight(LIGHT_BEDROOM1);\
								Timer_Windows(WINDOW_BEDROOM1);