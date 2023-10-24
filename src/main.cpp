/*!
	@file SSD1306_OLED_RPI/examples/src/HELLO_WORLD/main.cpp
	@author Gavin Lyons
	@brief Test file for SSD1306_OLED library, showing  "hello world" basic use case
	
	Project Name: SSD1306_OLED_RPI
	
	@test
		1. Test 100 Hello World
*/

#include <bcm2835.h>
#include <time.h>
#include <stdio.h>

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <sstream>
#include <fstream>

#include "SSD1306_OLED.hpp"

#define myOLEDwidth  128
#define myOLEDheight 64
const uint16_t I2C_Speed = 626; //  bcm2835I2CClockDivider enum , see readme.
const uint8_t I2C_Address = 0x3C;
SSD1306 myOLED(myOLEDwidth ,myOLEDheight) ; // instantiate  an object 

// =============== Function prototype ================
int TestLoop(void);

// ======================= Main ===================
int main(int argc, char **argv) 
{
	if(!bcm2835_init())
	{
		printf("Error 1201: init bcm2835 library\r\n");
		return -1;
	}
	
	std::ofstream trigger("/sys/class/leds/ACT/trigger", std::ios::trunc);
	trigger << "none" << std::endl;
	trigger.close();

	bcm2835_delay(500);
	myOLED.OLEDbegin(I2C_Speed, I2C_Address);

	return TestLoop();
	
	return 0;
}
// ======================= End of main  ===================

// ===================== Function Space =====================

int TestLoop() 
{

	    std::ofstream act("/sys/class/leds/ACT/brightness", std::ios::trunc);
	    std::ofstream pwr("/sys/class/leds/PWR/brightness", std::ios::trunc);
	    bool state = true;

	    while (true) {
		// Define a buffer to cover whole screen 
		uint8_t  screenBuffer[myOLEDwidth * (myOLEDheight/8)+1]; 
		myOLED.buffer = (uint8_t*) &screenBuffer;  // set that to library buffer pointer

		myOLED.OLEDclearBuffer(); 
		myOLED.setTextColor(WHITE);
		myOLED.setCursor(0, 0);

		//get time
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm localtime;
		localtime_r(&time, &localtime);
		std::ostringstream ss;
		ss << std::put_time(&localtime, "%H:%M:%S") << std::endl;
		myOLED.print("Time: " + ss.str());


		//get ip address
		int sock = socket(PF_INET, SOCK_DGRAM, 0);
		sockaddr_in loopback;

		if (sock == -1) {
		    std::cerr << "Could not socket\n";
		    return 1;
		}

		std::memset(&loopback, 0, sizeof(loopback));
		loopback.sin_family = AF_INET;
		loopback.sin_addr.s_addr = 1337;   // can be any IP address
		loopback.sin_port = htons(9);      // using debug port

		if (connect(sock, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == -1) {
		    close(sock);
		    std::cerr << "Could not connect\n";
		    return 1;
		}

		socklen_t addrlen = sizeof(loopback);
		if (getsockname(sock, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == -1) {
		    close(sock);
		    std::cerr << "Could not getsockname\n";
		    return 1;
		}

		close(sock);

		char buf[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0) {
		    std::cerr << "Could not inet_ntop\n";
		    return 1;
		} else {
		    myOLED.print(std::string("IP: ") + buf);
		}

		myOLED.OLEDupdate();  

		//change board leds
		if (state) {
			act << "1" << std::endl;
			pwr << "1" << std::endl;
		}
		else {
			act << "0" << std::endl;
			pwr << "0" << std::endl;
		}
		state = !state;

		sleep(1);
	    }

	    act.close();
	    pwr.close();
	    return 0;

}

// ============== EOF =========
