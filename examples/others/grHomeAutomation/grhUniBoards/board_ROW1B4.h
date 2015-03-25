#define LIGHT_ENTRANCE_1	1			
#define LIGHT_ENTRANCE_2	2
#define LIGHT_BATH_1		3			
#define LIGHT_BATH_2		4
#define	DHT_SENSOR			5 // define DHT_SENSOR with highest index


#define SOULISS_GLOBALS			
#define SOULISS_DEFINE_TYPICALS Set_LightsGroup(LIGHT_ENTRANCE_1, LIGHT_ENTRANCE_2);\
  								Set_LightsGroup(LIGHT_BATH_1, LIGHT_BATH_2);\
								Souliss_SetT52(memory_map, TEMPERATURE);

#define SOULISS_READ_INPUTS 	ssLightsGroupIN(IN1, LIGHT_ENTRANCE_1, LIGHT_ENTRANCE_2);\
								ssLightsGroupIN(IN2, LIGHT_BATH_1, LIGHT_BATH_2);	

#define SOULISS_PROCESS_LOGICS 	Logic_LightsGroup(LIGHT_ENTRANCE_1, LIGHT_ENTRANCE_2);\
								Logic_LightsGroup(LIGHT_BATH_1, LIGHT_BATH_2);

#define SOULISS_SET_OUTPUTS 	CntRelay1(LIGHT_ENTRANCE_1);\
								CntRelay2(LIGHT_ENTRANCE_2);\
								CntRelay3(LIGHT_BATH_1);\
								CntRelay4(LIGHT_BATH_2);

#define SOULISS_PROCESS_TIMERS 	Timer_LightsGroup(LIGHT_ENTRANCE_1, LIGHT_ENTRANCE_2);\
								Timer_LightsGroup(LIGHT_BATH_1, LIGHT_BATH_2);