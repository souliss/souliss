/**************************************************************************
	Souliss - vNet Virtualized Network
    
	Porting in vNet for Souliss project by Dario Di Maio
	
***************************************************************************/

#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include "src/types.h"

extern const prog_char ssid[];
extern U8 ssid_len;
extern const prog_char security_passphrase[];
extern U8 security_passphrase_len;
extern U8 security_type;
extern U8 wireless_mode;
extern prog_uchar wep_keys[];
extern unsigned char mfg_id[4];

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

#endif
