// Select the Wiznet controller model
#if(ETH_W5100 || ETH_W5200)
#	include "frame/vNet/drivers/ethW5x00/src/w5x00.h"
#elif(ETH_W5500)
#	include "frame/vNet/drivers/ethW5x00/src/w5500.h"
#endif

#include "frame/vNet/drivers/ethW5x00/src/socket.h"
#include "Ethernet.h"
#include "Dhcp.h"
#include "conf/eth/ethUsrCfg.h"
#include "base/SoulissBase.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 
  0, 0, 0, 0 };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 
  0, 0, 0, 0 };

int EthernetClass::begin()
{
  static DhcpClass s_dhcp;
  _dhcp = &s_dhcp;


  // Initialise the basic info
  W5x00.init();
  W5x00.setMACAddress((uint8_t*)MAC_ADDRESS);
  W5x00.setIPAddress(IPAddress(0,0,0,0).raw_address());

  // Now try to get our config info from a DHCP server
  int ret = _dhcp->beginWithDHCP((uint8_t*)MAC_ADDRESS);
  if(ret == 1)
  {
    // We've successfully found a DHCP server and got our configuration info, so set things
    // accordingly
	Souliss_SetIPAddress(_dhcp->getLocalIp().raw_address(), _dhcp->getSubnetMask().raw_address(), _dhcp->getGatewayIp().raw_address());
	/*
    W5x00.setIPAddress(_dhcp->getLocalIp().raw_address());
    W5x00.setGatewayIp(_dhcp->getGatewayIp().raw_address());
    W5x00.setSubnetMask(_dhcp->getSubnetMask().raw_address());
	*/
    
	_dnsServerAddress = _dhcp->getDnsServerIp();
  }

  return ret;
}

void EthernetClass::begin(IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(local_ip, dns_server);
}

void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(local_ip, dns_server, gateway);
}

void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
  IPAddress subnet(255, 255, 255, 0);
  begin(local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
  Souliss_SetIPAddress(local_ip._address, subnet._address, gateway._address);

  /*
  W5x00.init();
  W5x00.setMACAddress(mac);
  W5x00.setIPAddress(local_ip._address);
  W5x00.setGatewayIp(gateway._address);
  W5x00.setSubnetMask(subnet._address);
  */
  _dnsServerAddress = dns_server;
}

int EthernetClass::maintain(){
  int rc = DHCP_CHECK_NONE;
  if(_dhcp != NULL){
    //we have a pointer to dhcp, use it
    rc = _dhcp->checkLease();
    switch ( rc ){
      case DHCP_CHECK_NONE:
        //nothing done
        break;
      case DHCP_CHECK_RENEW_OK:
      case DHCP_CHECK_REBIND_OK:
        //we might have got a new IP.
		Souliss_SetIPAddress(_dhcp->getLocalIp().raw_address(), _dhcp->getSubnetMask().raw_address(), _dhcp->getGatewayIp().raw_address());
		/*
        W5x00.setIPAddress(_dhcp->getLocalIp().raw_address());
        W5x00.setGatewayIp(_dhcp->getGatewayIp().raw_address());
        W5x00.setSubnetMask(_dhcp->getSubnetMask().raw_address());
		*/
        _dnsServerAddress = _dhcp->getDnsServerIp();
        break;
      default:
        //this is actually a error, it will retry though
        break;
    }
  }
  return rc;
}

IPAddress EthernetClass::localIP()
{
  IPAddress ret;
  W5x00.getIPAddress(ret.raw_address());
  return ret;
}

IPAddress EthernetClass::subnetMask()
{
  IPAddress ret;
  W5x00.getSubnetMask(ret.raw_address());
  return ret;
}

IPAddress EthernetClass::gatewayIP()
{
  IPAddress ret;
  W5x00.getGatewayIp(ret.raw_address());
  return ret;
}

IPAddress EthernetClass::dnsServerIP()
{
  return _dnsServerAddress;
}

EthernetClass Ethernet;
