#ifndef ethernet_h
#define ethernet_h

#include <inttypes.h>
#include "IPAddress.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "Dhcp.h"
#include "conf/eth/ethUsrCfg.h"
#include "base/SoulissBase.h"

#define MAX_SOCK_NUM 4

class EthernetClass {
private:
  IPAddress _dnsServerAddress;
  DhcpClass* _dhcp;
public:
  static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];
  // Initialise the Ethernet shield to use the provided MAC address and gain the rest of the
  // configuration through DHCP.
  // Returns 0 if the DHCP configuration failed, and 1 if it succeeded
  int begin();
  void begin(IPAddress local_ip);
  void begin(IPAddress local_ip, IPAddress dns_server);
  void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
  void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
  int maintain();

  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsServerIP();

  friend class EthernetClient;
  friend class EthernetServer;
};

extern EthernetClass Ethernet;

#endif
