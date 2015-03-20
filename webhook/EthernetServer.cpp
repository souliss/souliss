/**************************************************************************
	This code is part of the original Arduino Ethernet library with relevant
	license, has been modified for the integration with vNet drivers.
	
	All modification are tracked in the comments.
***************************************************************************/

// Select the Wiznet controller model
#if(ETH_W5100 || ETH_W5200)
#	include "frame/vNet/drivers/ethW5x00/src/w5x00.h"
#elif(ETH_W5500)
#	include "frame/vNet/drivers/ethW5x00/src/w5500.h"
#endif

#include "frame/vNet/drivers/ethW5x00/src/socket.h"
extern "C" {
#include "string.h"
}

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "conf/eth/ethUsrCfg.h"

EthernetServer::EthernetServer(uint16_t port)
{
  _port = port;
}

void EthernetServer::begin()
{
  //for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {					/***/
  for (int sock = ST_SOCK; sock < MAX_SOCK_NUM; sock++) {		/***/
    EthernetClient client(sock);
    if (client.status() == SnSR::CLOSED) {
      socket(sock, SnMR::TCP, _port, 0);
      listen(sock);
      EthernetClass::_server_port[sock] = _port;
      break;
    }
  }  
}

void EthernetServer::accept()
{
  int listening = 0;

  //for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {					/***/  
  for (int sock = ST_SOCK; sock < MAX_SOCK_NUM; sock++) {		/***/  
    EthernetClient client(sock);

    if (EthernetClass::_server_port[sock] == _port) {
      if (client.status() == SnSR::LISTEN) {
        listening = 1;
      } 
      else if (client.status() == SnSR::CLOSE_WAIT && !client.available()) {
        client.stop();
      }
    } 
  }

  if (!listening) {
    begin();
  }
}

EthernetClient EthernetServer::available()
{
  accept();

  //for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {					/***/   
  for (int sock = ST_SOCK; sock < MAX_SOCK_NUM; sock++) {		/***/   
    EthernetClient client(sock);
    if (EthernetClass::_server_port[sock] == _port &&
        (client.status() == SnSR::ESTABLISHED ||
         client.status() == SnSR::CLOSE_WAIT)) {
      if (client.available()) {
        // XXX: don't always pick the lowest numbered socket.
        return client;
      }
    }
  }

  return EthernetClient(MAX_SOCK_NUM);
}

size_t EthernetServer::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t EthernetServer::write(const uint8_t *buffer, size_t size) 
{
  size_t n = 0;
  
  accept();

  //for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {					/***/ 
  for (int sock = ST_SOCK; sock < MAX_SOCK_NUM; sock++) {		/***/     
    EthernetClient client(sock);

    if (EthernetClass::_server_port[sock] == _port &&
      client.status() == SnSR::ESTABLISHED) {
      n += client.write(buffer, size);
    }
  }
  
  return n;
}
