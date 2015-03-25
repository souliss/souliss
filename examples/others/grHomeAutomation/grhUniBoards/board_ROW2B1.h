#define LIGHT_BALCONY2		1			
#define LIGHT_BEDROOM3		2
#define WINDOW_DINING		3
#define	DHT_SENSOR			4 // define DHT_SENSOR with highest index

#define SOULISS_GLOBALS

#define SOULISS_DEFINE_TYPICALS Set_SimpleLight(LIGHT_BALCONY2);\
								Set_SimpleLight(LIGHT_BEDROOM3);\
								Souliss_SetT22(memory_map, WINDOW_DINING);

#define SOULISS_READ_INPUTS 	ReadInput1(LIGHT_BALCONY2);\
								ReadInput2(LIGHT_BEDROOM3);\
								DigInWindowToggle(IN3, WINDOW_DINING);	

#define SOULISS_PROCESS_LOGICS 	Logic_SimpleLight(LIGHT_BALCONY2);\
								Logic_SimpleLight(LIGHT_BEDROOM3);\
								Souliss_Logic_T22(memory_map, WINDOW_DINING, &data_changed, SHUTTER_SHORT_TIMEOUT);

#define SOULISS_SET_OUTPUTS 	CntRelay1(LIGHT_BALCONY2);\
								CntRelay2(LIGHT_BEDROOM3);\
								ssDigOut(OUT3, Souliss_T2n_Coil_Open,  WINDOW_DINING);\
								ssDigOut(OUT4, Souliss_T2n_Coil_Close, WINDOW_DINING);

#define SOULISS_PROCESS_TIMERS 	Timer_SimpleLight(LIGHT_BALCONY2);\
								Timer_SimpleLight(LIGHT_BEDROOM3);\
								Timer_Windows(WINDOW_DINING);
