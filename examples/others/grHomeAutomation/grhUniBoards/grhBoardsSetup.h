#include "grhSoulissNetwork.h"

#define SHUTTER_SHORT_TIMEOUT		(Souliss_T2n_Timer_Off + 11)
#define SHUTTER_LONG_TIMEOUT		(Souliss_T2n_Timer_Off + 17)


////////////////////////////////////////////////////////////////////

#if defined(ROW1B1)	// Gateway and row bridge
	#define BOARD_PRODINO_V2
	#define	SOULISS_GATEWAY
	#define	IP_ADDRESS_4			IP_ADDRESS_ROW1B1
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW1B1
	#define RS485_SUPERNODE			0x0000
	#include "board_ROW1B1.h"

#elif defined(ROW1B2)
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW1B2
	#define RS485_SUPERNODE			RS485_ADDRESS_ROW1B1
	#include "board_ROW1B2.h"

#elif defined(ROW1B3)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW1B3
	#define RS485_SUPERNODE			RS485_ADDRESS_ROW1B1
	#include "board_ROW1B3.h"

#elif defined(ROW1B4)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW1B4
	#define RS485_SUPERNODE			RS485_ADDRESS_ROW1B1
	#include "board_ROW1B4.h"


////////////////////////////////////////////////////////////////////


#elif defined(ROW2B1)
	#define BOARD_PRODINO_V2	
	#define	IP_ADDRESS_4			IP_ADDRESS_ROW2B1
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW2B1
	#define RS485_SUPERNODE			0x0000
	#include "board_ROW2B1.h"

#elif defined(ROW2B2)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW2B2
	#define RS485_SUPERNODE			RS485_ADDRESS_ROW2B1
	#include "board_ROW2B2.h"

#elif defined(ROW2B3)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW2B3
	#define RS485_SUPERNODE			RS485_ADDRESS_ROW2B1
	#include "board_ROW2B3.h"

#elif defined(ROW2B4)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_ROW2B4
	#define RS485_SUPERNODE			RS485_ADDRESS_ROW2B1
	#include "board_ROW2B4.h"


////////////////////////////////////////////////////////////////////

#elif defined(BED1B1)
	#define BOARD_PRODINO_V2	
	#define	IP_ADDRESS_4			IP_ADDRESS_BED1B1
	#include "board_BED1B1.h"


////////////////////////////////////////////////////////////////////

#elif defined(BED2B1)
	#define BOARD_PRODINO_V2	
	#define	IP_ADDRESS_4			IP_ADDRESS_BED2B1
	#define	RS485_ADDRESS			RS485_ADDRESS_BED2B1
	#define RS485_SUPERNODE			0x0000
	#include "board_BED2B1.h"

#elif defined(BED2B2)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_BED2B2
	#define RS485_SUPERNODE			RS485_ADDRESS_BED2B1
	#include "board_BED2B2.h"

#elif defined(BED2B3)	
	#define BOARD_PRODINO_V2	
	#define	RS485_ADDRESS			RS485_ADDRESS_BED2B3
	#define RS485_SUPERNODE			RS485_ADDRESS_BED2B1
	#include "board_BED2B3.h"

////////////////////////////////////////////////////////////////////

#elif defined(LOFTB1)	
	#define BOARD_MEGA_ETH	
	#define	IP_ADDRESS_4			IP_ADDRESS_LOFTB1
	#include "board_LOFTB1.h"

////////////////////////////////////////////////////////////////////

#elif defined(TESTB1)
	#define	SOULISS_GATEWAY
	#define BOARD_MEGA_ETH	
	#define	IP_ADDRESS_4			IP_ADDRESS_TESTB1
	#include "board_TESTB1.h"

////////////////////////////////////////////////////////////////////
#endif

#if defined(RS485_SUPERNODE)
	#if (RS485_SUPERNODE == 0x0000)
		#define SOULISS_BRIDGE
	#endif
#endif