
#define LIGHT_DINING_1		1			
#define LIGHT_DINING_2		2
#define LIGHT_DINING_3		3			
#define LIGHT_STAIRS		4
#define	DHT_SENSOR			5 // define DHT_SENSOR with highest index



#define SOULISS_GLOBALS			

#define SOULISS_DEFINE_TYPICALS Set_LightsGroup(LIGHT_DINING_1, LIGHT_DINING_3);\
								Set_SimpleLight(LIGHT_STAIRS);

#define SOULISS_READ_INPUTS 	ssLightsGroupIN(IN1, LIGHT_DINING_1, LIGHT_DINING_3);\
								ReadInput2(LIGHT_STAIRS);

#define SOULISS_PROCESS_LOGICS 	Logic_LightsGroup(LIGHT_DINING_1, LIGHT_DINING_3);\
								Logic_SimpleLight(LIGHT_STAIRS);

#define SOULISS_SET_OUTPUTS 	CntRelay1(LIGHT_DINING_1);\
								CntRelay2(LIGHT_DINING_2);\
								CntRelay3(LIGHT_DINING_3);\
								CntRelay4(LIGHT_STAIRS);

#define SOULISS_PROCESS_TIMERS 	Timer_LightsGroup(LIGHT_DINING_1, LIGHT_DINING_3);\
								Timer_SimpleLight(LIGHT_STAIRS);