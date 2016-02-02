/**************************************************************************
    Souliss - Hello World for Expressif ESP8266-DEV with MOD-MPU6050
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.

    And code for libs from Olimex: 
    https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/resources/MOD-MPU6050_arduino.zip
        
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Olimex_ESP8266EVB.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"

// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "mywifi"
#define WiFi_Password           "mypassword"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
// https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/resources/MOD-MPU6050_arduino.zip
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az, cax, cay, caz;
int16_t gx, gy, gz, cgx, cgy, cgz;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO

// This identify the number GPIO0 of the Button
#define IN0                 0
// This identify the number GPIO5 for control Relay 
#define RELAY_PIN           5
// This identify the number of the LED logic
#define MYLEDLOGIC          0
// This identify the number GPIO2 for I2C SDA wire 
#define I2C_MASTER_SDA_GPIO 2
// This identify the number GPIO4 for I2C SCL wire
#define I2C_MASTER_SCL_GPIO 4

void setup()
{   
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin(I2C_MASTER_SDA_GPIO, I2C_MASTER_SCL_GPIO);
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    //Serial.begin(115200); //I don't have UEXTx5 
				// https://www.olimex.com/Products/Modules/Interface/UEXTx5/open-source-hardware, 
				// so I'm use UEXT only for sensor, but console work if you uncoment Serial... 

    // initialize device
    //Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    //Serial.println("Testing device connections...");
    //Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    accelgyro.setXGyroOffset(220);
    accelgyro.setYGyroOffset(76);
    accelgyro.setZGyroOffset(-85);

    Initialize();

    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This node will serve all the others in the network providing an address
    SetAddressingServer();
    
    Set_SimpleLight(MYLEDLOGIC);        // Define a simple LED light logic
    pinMode(IN0, INPUT);                        // Use pin 0 as input
    pinMode(RELAY_PIN, OUTPUT);                 // Use pin 5 as output 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds

            // read raw accel/gyro measurements from device
            accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

            DigIn(IN0, Souliss_T1n_ToggleCmd, MYLEDLOGIC);
            Logic_SimpleLight(MYLEDLOGIC);
            if ((0 != gx/1000) || (0 != gy/1000) || (0 != gz/1000)) {
                DigOut(RELAY_PIN, Souliss_T1n_Coil,MYLEDLOGIC);
            }

            cax = ax/1000;
            cay = ay/1000;
            caz = az/1000;
            cgx = gx/1000;
            cgy = gy/1000;
            cgz = gz/1000;

        } 
              
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
} 
