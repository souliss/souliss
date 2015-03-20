/* vNet Example 6, Web Server

	Use of vNet libraries for standard Server/Client connections.
	
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0

	ethUsrCfg.h				#define ETH_W5100			1	
	
	ethUsrCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	ethUsrCfg.h				const uint8_t MAC_ADDRESS[]           = {...}	
	
	It's important define the proper MAC address for each board, two nodes
	that share the same MAC address will not communicate. You can found a
	proper MAC address on the sticker behind the board.

	As alternative to address the MAC for each board, the AUTO_MAC option can
	be used along with a Local MAC address. More details are available in
	ethUsrCfg.h configuration file.
	
	Only if debug on serial port is required,
	vNet_Config.h			#define VNET_DEBUG  		1
  
*/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define BUFFER_SIZE		200
#define	SERVER_PORT		80

uint8_t data_len, buf_idx, buf[BUFFER_SIZE];
uint8_t htmlend=0;

void setup()
{
  	// Set the network parameters
	eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
	eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
	eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);	
		
	vNet_Init();

	// The IP address is given by the byte-wise sum of "Base IP Address"
	// and the vNet address
	vNet_SetAddress(0x0013, vNet_GetMedia(0x0013));			// Set address
	vNet_SetSubnetMask(0xFF00, vNet_GetMedia(0x0013));		// Set subnet mask
	
	// Set a listening HTTP port
	srv_listen(SERVER_PORT); 
}

void loop()
{	
	// Look for available data on the listening port
	data_len = srvcln_dataavailable(SERVER_PORT);
	
	// If there are available incoming data
	if (data_len) 
	{  
		// If the socket is connected and the data size doesn't exceed the buffer
		if (srvcln_connected(SERVER_PORT) && (data_len < BUFFER_SIZE))										// If data are available
		{
			// Retrieve data from the Ethernet controller
			srvcln_retrieve(buf, data_len);
				
			uint8_t i=0;
			while((buf[i] != '\n') && i < data_len)			// Move forward until a return is found
                i++;
			
			// If a return was found, the HTML request is completed
			if (buf[i] == '\n')
				htmlend=1;

		}
		else
		{
			// Empty the controller buffer and discard the data
			srvcln_retrieve(buf, BUFFER_SIZE);
		}
		
		// Reply to the HTML request
		if(htmlend)
        {		
			// Define HTTP handshaking messages	
			char* str[] = {"HTTP/1.1 200 OK\n\r","Content-Type: text/html\n\r","\n\r"};
			
			// HTTP/1.1 200 OK\n\r
			srvcln_send((uint8_t*)str[0], strlen(str[0])); 

			// Content-Type: text/html\n\r
			srvcln_send((uint8_t*)str[1], strlen(str[1])); 
		
			// \n\r
			srvcln_send((uint8_t*)str[2], strlen(str[2])); 
						
			// Define HTTP Content	
			char* strcnt[] = {"analog input ", " is ", "\n\r"};

			// Print the value of each analog input pin
			for (uint8_t analogChannel = 0; analogChannel < 6; analogChannel++) 
			{
				// Print "analog input "
				srvcln_send((uint8_t*)strcnt[0], strlen(strcnt[0])); 		
			
				// Send the current analog channel index
				buf_idx = 0;
				*(unsigned long*)buf = analogChannel;				
				srvcln_num2str(buf, DEC, &buf_idx);			// Convert to string
				srvcln_send(buf, buf_idx); 					// Send data
				
				// Print " is "
				srvcln_send((uint8_t*)strcnt[1], strlen(strcnt[1])); 
			
				buf_idx = 0;
				*(unsigned long*)(buf) = analogRead(analogChannel);				
				srvcln_num2str(buf, DEC, &buf_idx);			// Convert to string
				srvcln_send(buf, buf_idx); 					// Send data
			
				// Print "\n\r"
				srvcln_send((uint8_t*)strcnt[2], strlen(strcnt[2])); 			
			}
			
			htmlend=0;
			srvcln_stop();									// Close the communication
		}

	}
	
	delay(50);
}