# Souliss

> Open source framework for Smart Home based on Arduino compatible devices (AVR, ESP8266) released as GPLv3.
> Copyright (C) 2011-2016

### Release note for revision v7.3.0,   on 06/01/2020

Preliminary support for ESP32 (WiFi and multi-core), migrating to latest ESP8266 cores (2.6.3) and Arduino IDE (1.8.1)
- The ESP32 is actually supported (preliminary) - Dario Cdj
- Multiple stability and functional bugfix by @fazioa, @ribico, @juanpintom, @dpeddi, @alvarolobato, @lucarui, @FulvioSpelta, @dariocdj, @loox, @marcolino7, @elminister83, @shineangelic, @plinioseniore


### Release note for revision v7.2.1,   on 30/10/2016

Completed support for KMP Electronics PRODINo WiFi-ESP WROOM-2
- Rewritten KMP Electronics PRODINo WiFi-ESP WROOM-2 examples for RS485
- Fixed support for ESP8266 and RS485
- Publish/Subscribe methods renamed to avoid conflict with MQTT library

### Release note for revision v7.2,   on 08/10/2016

The Gateway can now interact easily with event-based cloud services using the new LASTIN. Debug messages can now be printed on any interface and not only USART with a simple inSketch definition.

- Preliminary Support for KMP Electronics PRODINo WiFi-ESP WROOM-2
- Revamped LASTIN for simpler use with Cloud services
- Debug object #define is centralized
- Introduction of Pulsed Output functions
- Support for ESP8266 Core 2.3.0
- Resolved many compiler warnings

### Release note for revision v7.1.1, on 12/12/2015
    
Patch release, minor bugfix and improvements.

### Release note for revision v7.1, on 14/10/2015 (Maker Faire 2015 Edition)

Introduce the runtime switch between Gateway or Peer and an HTTP based WebConfig interface for ESP8266 with support for OTA.

- Support for Authometion LYT8266 WiFi Multicolor Bulb
- Support for IONO Relay Board
- Support for Industruino PLC
- Support for Controllino Maxi PLC
- Gateway behaviour can now be set at runtime
- Support for ESP8266 OTA (Over the Air) programming
- Added WebConfiguration for ESP8266
- Introduced a Lock-down for User Interfaces
- Added T6n Typicals, Analog Setpoint
- Bugfix for T18, T19 and T31
- Bugfix for Moteino support
- General bugfix (see issues)

### Release note for revision v7.0.6, on 16/08/2015

Introduce the support for the new LYT88 light bulbs from Authometion and include several bugfix and enhancements.

- Support for Authometion LYT88 LED RGB+W Light Bulbs
- Introduced Remote I/O
- Introduced vNet Hard Reset to force a reset via Watchdog
- Most used log messages are now located in the programming memory and no longer in RAM
- Minor bugfix for T12 and T22
- Standard deadband for analog value at 1.5% (was at 5%)
- Added example for standard Arduino with USART / RS485
- Added example for ESP8266 with USART / RS485


### Release note for revision v7.0.5, on 27/05/2014

Porting for 16-bit and 32-bit microcontrollers and support to run Souliss directly on Expressif ESP8266 SoC with WiFi
- Porting for 16-bit and 32-bit microcontrollers
- Support for Expressif ESP8266 SoC with WiFi
- vNet over USART extended to ESP8266
- Minor bugfix


### Release note for revision v7.0.3, on 07/05/2015

Revised configuration structure and dynamic addressing features, extended support for battery operated devices.
- Configuration is only available through inSketch or changing the configuration files
- Quick Configuration is deprecated.
- Dynamic Addressing works also in case of complex network
- EEPROM is used to retain network configuration for either Gateway and Peers
- Review of all examples
- Zeroconfig examples
- Introduction of Publish/Subscribe user methods
- Massive commands can be used between nodes
- Media 3 has been moved from MAC broadcast to IP broadcast
- General bugfix 


### Release note for revision Alpha 6.1.4, on 25/01/2015

Minor bugfix, last release code for branch Alpha 6.


### Release note for revision Alpha 6.1.3, on 18/12/2014

General improvement and minor bugfix
- Added software filtering for digital inputs
- Added support for T1A Digital Input
- Rollback autoaddressing code to Alpha 5.2
- Added support for "IoTuino Radio Only" using an external Ethernet transceiver
- nRF24L01 CS and EN pins are moved to inSketch 
- Fix on SPI CS for Atmega32U4
- Bugfix for case sensitive on DHCP code
- Bugfix on T11 and DigInHold
- Other minor bugfix  

Thanks to: Juan Luis, Juan Pinto, Fulvio Spelta, Marco Pozzuolo, Saverio Sbrana

### Release note for revision Alpha 6.1.2, on 07/11/2014

Modified T22 logic and general bugfixes


### Release note for revision Alpha 6.1.1, on 13/10/2014

Support for Moteino boards and fix the Alpha 6.0.3 this release include preliminary functionality that needs to be consolidated in future.  
- Introduction of Action Messages (preliminary)
- Massive commands can be used between nodes (preliminary)
- Preliminary support for Moteino and HopeRF RFM69CW/RFM69HCW
- Media 3 has been moved from MAC broadcast to IP broadcast (preliminary)
- Iotuino USART drivers are now based on SerialPort by William Greiman
- Improvement on broadcasting
- New typical retrieval approach bugfixed


### Release note for revision Alpha 6.0.3, on 23/09/2014

Fix a major bug while retrieving typicals for logics loaded in the network nodes        


### Release note for revision Alpha 6.0.2, on 14/09/2014

Major improvement on USART drivers for vNet
- vNet USART driver buffer handling improvement
- vNet USART max frame is now a parameter
- vNet USART nodex startup delay is introduced
- vNet USART retry in case of bus busy
- MaCaco TYP commands are now in broadcast (before unicast was used)


### Release note for revision Alpha 6.0.1, on 22/08/2014

Modbus TCP/RTU interface is back available. Introduced a buffer for the XML interface for either TCP and UDP cases to handle multiple commands in short time.
- Modbus TCP/RTU interface
- Modified the memory handling, save more RAM on Peers
- Nodes can dynamically join a Gateway even if has a fixed vNet address
- Support for Multicast
- Automatic update of the routing table in case of dynamic join of a gateway
- vNet Autoreset
- Command buffer for HTTP/XML and UDP/XML Interface (bugfix by Alexander Mathis)
- The openHAB Interfaces are now generic and called XMLServer Interfaces
- All Souliss DigIn can now returns the results without affect slots values
- Support of DINo over RS485
- Examples with Modbus TCP over DINo with plug&play
- Support for HF-LPT200 (preliminary)
- Bugfix

### Release note for revision Alpha 5.2, on 22/05/2014

A new XML data interface over UDP has been introduced, based on LASTIN allows event driven data retrieving in ASCII/XML format. Support for nRF24L01 and nRF24L01+ 2.4 GHz radio and Wiznet W5500 Ethernet controller.
- Extended openHAB HTTP/XML interface on uIP        - Juan Pinto
- Completed LASTIN mode
- UDP/XML Interface (Event based communication)     - Fulvio Spelta
- Battery friendly subscription mode introduced
- vNet drivers for Wiznet W5500                     - Philippe Peltier
- vNet drivers for nRF24L01 and nRF24L01+   
- New examples                                      - Flavio Piccolo
- Bugfix


### Release note for revision Alpha 5.1, on 09/03/2014

The introduction of PERSISTENCE and LASTIN modes makes a gateway node able to provide data to external sources using polling protocols. External protocols are now referred as Interfaces and are available either on gateway as peer nodes.
- Introduced openHAB interface based on HTTP/XML (thanks to Fulvio Spelta)
- The INTERFACES takes place of HTTP and Modbus GATEWAY
- The INTERFACES is now handled directly on MaCaco and has PERSISTENCE and LASTIN modes available
- Bugfix for USART over RS485 (thanks to Marco Pozzuolo and Juan Pinto) 


### Release note for revision Alpha 5.0.4, on 12/01/2014

Introduction of USART drivers in Quick Configuration and preliminary support for Arduino Ethernet Library methods, this ensure compatibility with third party libraries and doesn't affect Souliss/vNet communication.

### Release note for revision Alpha 5.0.3, on 18/12/2013

Support for DINo v2 completed with addition of Board Configuration files contained in bconf/ folder, that include automatically all the Quick Configuration parameters for that sketch, without need of user intervention of QuickConf.h file.
- Board Configuration files for inSketch mode added
- KMP Electronics DINo v2 full support completed
- Bugfix for IR-RGB Light, thanks to Juan Pinto
- Bugfix for Inputs method, thanks to Juan Luis Navarro.


### Release note for revision Alpha 5.0.2, on 01/12/2013

Introduction of inSketch mode, this move selected configuration parameters from the configuration file to the sketch. Removed legacy non-pass-through mode, from this time the gateway cannot longer serve polling protocols.
- Introduced inSketch mode,
- Modbus protocol deprecated,
- Bugfix for DINo v2 support (still work in progress).


### Release note for revision Alpha 5.0.1, on 13/11/2013

Introduced a revised memory handling for MaCaco in order reduce the RAM waste in case of non-gateway nodes, broadcasting features for configuration-less nodes
- Configuration-less network for simple architectures added
- Nodes can join a Souliss network in runtime
- Broadcast support for vNet    
- PassThrough mode for MaCaco communication with User Interfaces
- Support for KMP Electronics DINo II relay board
- Added Wiznet W5200 drivers (Ethernet controller)
- HTTP/JSON running on nodes is deprecated
- Revised memory structure for MaCaco, save RAM on non-gateway nodes
- Bandwith friendly approach for MaCaco retries mechanism
- Bug-fix for uIP/vNet IP source port handling (thanks to Antonino Fazio)
- Arduino IDE 1.0.x and 1.5.x supported as main releases


### Release note for revision Alpha 4.5.1, on 10/09/2013

Introducted a macro based easy coding feature referred as Speak Easy
- Introduced Speak Easy
- Introduced an USART vNet Driver for RS485 communication (preliminary)
- Native support for DFRobots XBoard
- Support for ENC28J60 + AT86RF230 SuperNodes (thanks to Antonino Fazio)
- Bug-fix for interrupt handler of chibiduino stack
- Bug-fix on ENC28J60 vNet Driver


### Release note for revision Alpha 4.5, on 18/07/2013

Review of analog values managment and relevant logics (typicals), support for XBoard Relay from DFRobots and DHT temperature and humidity sensors
- Introduced half-precision floating and methods
- Native support for DHT sensors
- Native support for DFRobots XBoard Relay
- Bug-fix as per chibiduino stack v.1.00a
- General bug-fix for typicals (T16, T42)


### Release note for revision Alpha 4.4.2, on 04/06/2013

Bugfix in vNet over IP drivers, now source port is handled properly also in case of source port masquering by NAT (thanks to Juan Luis).

### Release note for revision Alpha 4.4.1, on 13/05/2013

Included updated documentation for
- MaCaco Light Data Protocol "User and Developers Guide"
- vNet Network Virtualization Layer "User Guide"
- Wireshark capture for MaCaco over vNet/UDP-IP


### Release note for revision Alpha 4.4, on 07/05/2013

Extended support of Olimex boards
- Support for Olimex MOD-RGB
- Support for Olimex MOD-IO
- Modbus RemoteInputs fix (thanks to Giorgio Zoppetti)


### Release note for revision Alpha 4.3, on 12/04/2013

Support for AirQ Networks wireless 433 MHz boards (needs download of Air sNET library)
- Support for AirQ Shield
- Support for AirQ 305
- Bug fix


### Release note for revision Alpha 4.2, on 25/03/2013

Support for Open Electronics boards and improvement on multicast commands.
- Support for Open Electronics RGB Shield
- Support for Open Electronics ENC28J60 Ethernet Shield
- Reduced number of frame sent for multicast commands
- Common commands tuned for Modbus coil use.

### Release note for revision Alpha 4.1, on 08/02/2013

Support Microchip MRF24WB0MA WiFi module and Olimex boards, introduction of Quick Configuration mode.
- Support for Olimex AVR-T32U4
- Support for Olimex OLIMEXINO-32U4
- Support for Olimex OLIMEXINO-328
- Support for Olimex MOD-ENC28J60
- Support for Olimex MOD-WIFI
- Support for Olimex MOD-IO2            
- Support for Microchip MRF24WB0MA (up to firmware relase 0x1207)
- Quick Configuration mode


### Release note for revision Alpha 4, on 22/12/2012

Support for Modbus and extended MaCaco for user interfaces, all communication based on binary protocol to increase the performances. Improvement on RAM handling.
- Introduction of oFrame for data buffer handling,
- Extended MaCaco for user interfaces,
- Use of UDP for vNet based communication instead of TCP,
- Modbus RTU and TCP Slave,
- HTTP Server for remote commands,
- New Server/Client TCP APIs,
- New Examples,
- Bug fix.
    
### Release note for revision Alpha 3.1, on 30/07/2012

Bugfix for ENC28J60 and first release of plinio drivers
- Binary FSK using ATmega328P (preliminary),
- New JSON Server with support for one and two commands at same time,
- Bug fix,
- Enhanced examples.


### Release note for revision Alpha 3, on 18/07/2012

Support for the Android Client application, integration with anti-theft system and tools for communication debugging:
- Support for ENC28J60,
- Introduction of Raw Ethernet communication,
- Watchdog chain and typicals for anti-theft integration,
- Debug mode for vNet and MaCaco using serial port,
- Enhanced scheduler and examples,
- Friendly names for Souliss examples,      
- Arduino IDE 1.    


### Release note for revision Alpha 2.3, on 11/04/2012

Support for the Android Client application:
- Integration with dedicated Android Cleint adding a new JSON array structure,


### Release note for revision Alpha 2.2, on 02/02/2012

Bugfix


### Release note for revision Alpha 2.1, on 01/02/2012

Support for nodes dedicated as JSON Server.


### Release note for revision Alpha 2, on 07/01/2012

Support for user customizable interfaces is added, following additional features are provided:
- Compatibility with standard Ethernet class (2 sockets),
- JSON Server for User Interfaces,
- Browser based User Interface,
- Support for new typicals (ready to use logics for Home Automation).


### Release note for revision Alpha, on 23/11/2011 

An smart home framework with Ethernet and 2.4 Ghz Point to Point Wireless support.

Supported boards are: 
- Freaklabs Chibiduino (2.4 GHz Wireless)
- Arduino 2009/UNO with Ethernet Shield (Wiznet W5100),
- Arduino Ethernet. 

Development enviroment Arduino IDE 0022.    
        