/* vNet Example 6, Data Transmission

	Use of vNet libraries for standard Server/Client connections.
	
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0
	
	ethUsrCfg.h				#define ETH_ENC28J60		1	
	
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

uint8_t buf_idx, buf[BUFFER_SIZE];
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
	// Run the TCP/IP stack
	vNet_DataAvailable();
	
	// Look for available data on the listening port
	uint8_t sock = srvcln_getsocket(SERVER_PORT);
	
	// If the connection is estabilished
	if (srvcln_connected(SERVER_PORT, sock)) 
	{
		// If an HTML request was found
		if(htmlend)
        {	
			srvcln_discard();						// Discard whatever is coming in, now is time for the answer		
			
			// Define HTTP handshaking messages	
			char* str[] = {"HTTP/1.1 200 OK\n\r","Content-Type: text/html\n\r","\n\r"};
			
			// HTTP/1.1 200 OK\n\r
			srvcln_load((uint8_t*)str[0], strlen(str[0])); 
			srv_send(sock, SENDDATA); 
			srv_waitsend(sock);

			// Content-Type: text/html\n\r
			srvcln_load((uint8_t*)str[1], strlen(str[1])); 			
			srv_send(sock, SENDDATA); 
			srv_waitsend(sock);
		
			// \n\r
			srvcln_load((uint8_t*)str[2], strlen(str[2])); 				
			srv_send(sock, SENDDATA); 
			srv_waitsend(sock);	
						
			// Define HTTP Content	
			char* strcnt[] = {"analog input ", " is ", "\n\r"};

			// Print the value of each analog input pin
			for (uint8_t analogChannel = 0; analogChannel < 6; analogChannel++) 
			{
				// Print "analog input "
				srvcln_load((uint8_t*)strcnt[0], strlen(strcnt[0])); 
				srv_send(sock, SENDDATA); 
				srv_waitsend(sock);			

				// Send the current analog channel index
				buf_idx = 0;
				*(unsigned long*)buf = analogChannel;				
				srvcln_num2str(buf, DEC, &buf_idx);			// Convert to string
				srvcln_load(buf, buf_idx); 
				srv_send(sock, SENDDATA); 					// Send data
				srv_waitsend(sock);	
				
				// Print " is "
				srvcln_load((uint8_t*)strcnt[1], strlen(strcnt[1])); 				
				srv_send(sock, SENDDATA); 
				srv_waitsend(sock);
			
				buf_idx = 0;
				*(unsigned long*)(buf) = analogRead(analogChannel);				
				srvcln_num2str(buf, DEC, &buf_idx);			// Convert to string
				srvcln_load(buf, buf_idx); 
				srv_send(sock, SENDDATA); 					// Send data
				srv_waitsend(sock);	
			
				// Print "\n\r"
				srvcln_load((uint8_t*)strcnt[2], strlen(strcnt[2])); 				
				srv_send(sock, SENDDATA); 
				srv_waitsend(sock);				
			}
			
			htmlend=0;
			srvcln_stop(sock);
			srv_waitsend(sock);

		}
  
		// If there are available incoming data
		if (srvcln_dataavailable(SERVER_PORT))
		{
			// Retrieve data from the Ethernet controller
			srvcln_retrieve(buf, BUFFER_SIZE);						// Move forward until a return is found
			
			uint8_t i=0;
			while((buf[i] != '\n') && i < VNET_MAX_PAYLOAD)
                i++;
			
			// If a return was found, the HTML request is completed
			if (buf[i] == '\n')
				htmlend=1;
 
		}

	}

	delay(50);

}