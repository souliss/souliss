/**************************************************************************
	BUFFER Tools 	
***************************************************************************/

// Buffer incoming commands
uint8_t	cmdbuff_header[CMDBUFF_HEADER];
uint8_t	cmdbuff_vals[CMDBUFF_VALS];
#define	cmdbuff ((CommandBufferHeader *)cmdbuff_header)

/**************************************************************************
/*!
	Store commands into the buffer
*/	
/**************************************************************************/
void command_buffer(uint16_t address, uint8_t slot, uint8_t* val)
{
	uint8_t i;

	// Look for free space
	for(i=0; i<CMDBUFF_NO; i++)
		if(!(cmdbuff[i].address))
		{	
			// Store address and slot
			cmdbuff[i].address = address;
			cmdbuff[i].slot = slot;
			
			// Store values
			memmove((cmdbuff_vals+(i*CMDBUFF_NO)), val, CMDBUFF_NO);
			
			break;
		}
	
	// If there is no longer free space, overwrite the oldest one
	if(i==CMDBUFF_NO)
	{
		// Store address and slot
		cmdbuff[0].address = address;
		cmdbuff[0].slot  = slot;
					
		// Store values
		memmove(cmdbuff_vals, val, CMDBUFF_NO);
			
	}
}

/**************************************************************************
/*!
	Send stored commands
*/	
/**************************************************************************/
void command_send()
{
	// Look for a command to send
	for(uint8_t i=0; i<CMDBUFF_NO; i++)
		if(cmdbuff[i].address)
		{
			MaCaco_send(cmdbuff[i].address, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + cmdbuff[i].slot, MAXVALUES, (cmdbuff_vals+(i*CMDBUFF_NO)));		
			
			// Reset the node
			cmdbuff[i].address=0;
			
			// Next one at next cycle
			break;
		}
}