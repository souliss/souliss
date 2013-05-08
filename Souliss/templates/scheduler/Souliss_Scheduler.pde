/**************************************************************************
	
	Souliss - Base Scheduler
	
***************************************************************************/

#include "Souliss.h"
#include "Typicals.h"
#include "MaCaco.h"
#include "vNet.h"

#include <SPI.h>

#include "SoulissJSONServer.h"

#define time_base_fast		10				// Time cycle in milliseconds
#define time_base_slow		10000			// Time cycle in milliseconds
#define num_phases			255				// Number of phases

U8 phase_speedy=0, phase_fast=0, phase_slow=0;
unsigned long tmr_fast=0, tmr_slow=0;  

void setup()
{	
}

void loop()
{ 
  
	if(abs(millis()-tmr_fast) > time_base_fast)
	{	
		tmr_fast = millis();
		phase_fast = (phase_fast + 1) % num_phases;

		// Execute the code every time_base_fast
		if (!(phase_fast % 1))
		{                 
		} 
	  
		// Execute the code every 3 time_base_fast		
		if (!(phase_fast % 3))
		{
		} 
		
		// Execute the code every 5 time_base_fast		  
		if (!(phase_fast % 5))
		{   
		}

		// Execute the code every 7 time_base_fast		  
		if (!(phase_fast % 7))
		{   
		}		

		// Execute the code every 11 time_base_fast		  
		if (!(phase_fast % 11))
		{   
		}		
				
		// Execute the code every 23 time_base_fast		  
		if (!(phase_fast % 23))
		{   
		}		
		
		// Execute the code every 53 time_base_fast		  
		if (!(phase_fast % 53))
		{
		}		
		
		// Execute the code every 103 time_base_fast		 
		if (!(phase_fast % 103))
		{
		}
		
		// Execute the code every 251 time_base_fast		 
		if (!(phase_fast % 251))
		{
		}
		
	}
	else if(abs(millis()-tmr_slow) > time_base_slow)
	{	
		tmr_slow = millis();
		phase_slow = (phase_slow + 1) % num_phases;

		// Execute the code every time_base_slow
		if (!(phase_slow % 1))
		{                 
		} 
	  
		// Execute the code every 3 time_base_slow		
		if (!(phase_slow % 3))
		{
		} 
		
		// Execute the code every 5 time_base_slow		  
		if (!(phase_slow % 5))
		{   
		}

		// Execute the code every 7 time_base_slow		  
		if (!(phase_slow % 7))
		{   
		}		

		// Execute the code every 11 time_base_slow		  
		if (!(phase_slow % 11))
		{   
		}		
		
		// Execute the code every 23 time_base_slow		  
		if (!(phase_slow % 23))
		{   
		}		
		
		// Execute the code every 53 time_base_slow		  
		if (!(phase_slow % 53))
		{
		}		
		
		// Execute the code every 103 time_base_slow	 
		if (!(phase_slow % 103))
		{
		}
		
		// Execute the code every 251 time_base_slow		 
		if (!(phase_slow % 251))
		{
		}	
	}
	else
	{
		phase_speedy = (phase_speedy + 1) % num_phases;

		// Execute the code every time the MCU is free
		if (!(phase_speedy % 1))
		{                 
		} 
	  
		// Execute the code every 3 times of free MCU		
		if (!(phase_speedy % 3))
		{
		} 
		
		// Execute the code every 5 times of free MCU				  
		if (!(phase_speedy % 5))
		{   
		}

		// Execute the code every 7 times of free MCU		
		if (!(phase_speedy % 7))
		{   
		}		
	}	
	
} 