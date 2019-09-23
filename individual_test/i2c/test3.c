/*
=================================================================================
 Name        : i2c-arduino.c
 Version     : 0.1
 Copyright (C) 2012 by Andre Wussow, 2012, desk@binerry.de
 Description :
     Sample of controlling an Arduino connected to Raspberry Pi via I2C.
	 Recommended connection (http://www.raspberrypi.org/archives/384):
	 Arduino pins      I2C-Shifter      Raspberry Pi
	 GND                                P06  - GND
	 5V                5V
	 SDA               SDA2
	 SCL               SCL2
	                   3V3              P01 - 3.3V
					   SDA1             P03 - SDA
					   SCL1             P05 - SCL
 References  :
 http://binerry.de/post/27128825416/raspberry-pi-with-i2c-arduino-slave
 
================================================================================
This sample is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This sample is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
================================================================================
*/

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <unistd.h>

int deviceHandle;
int readBytes;
char buffer[7];

void testCommand(int command);
	
int main (void)
{
	// print infos
	printf("Raspberry Pi I2C Arduino Sample\n");
	printf("========================================\n");
	
	// initialize buffer
	buffer[0] = 0x00;
  
	// address of i2c Arduino device
	int deviceI2CAddress = 0x0A;  // (0x2A = 42)
	
	// open device on /dev/i2c-0
	if ((deviceHandle = open("/dev/i2c-0", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	
	// connect to arduino as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0) {
		printf("Error: Couldn't find arduino on address!\n");
		return 1;
	}  
	

	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle, buffer, 1);
/*
	if (readBytes != 1)
	{
		printf("Error: Received no ACK-Bit, couldn't established connection!");
	}
	else
	{
*/
		// drive some tests
		testCommand(5);
		usleep(200000); // 2s
		testCommand(0);
		usleep(200000); // 2s
		testCommand(125);
		usleep(200000); // 2s
		testCommand(0);
		usleep(200000); // 2s
/*
		testCommand("L31", "LED3 on");
		usleep(2000000); // 2s
		testCommand("L30", "LED3 off");
		usleep(2000000); // 2s
		testCommand("R11", "Relay on");
		usleep(2000000); // 2s
		testCommand("R10", "Relay off");
*/
//	}
	
	// close connection and return
//char pepe[3]="127";
//printf("%i",(int)pepe);
	close(deviceHandle);
	return 0;
}

// function for testing command
void testCommand(int command)
{
	// switch on
//	printf("Switching %s ... ", action);		

	  char str[4]; /* MUST be big enough to hold all 
                  the characters of your number!! */
//	  int i;

	  sprintf(str, "%d", command);
	  printf("con string %s\n", str);
	  printf("con int %d\n\n", command);


//	readBytes = write(deviceHandle, str, sizeof(str));
	readBytes = write(deviceHandle, xFF, 1);
		
	// give arduino some reaction time
	usleep(10000); // 10ms
		
	// read success
/*
	readBytes = read(deviceHandle, buffer, 1);
	if (readBytes != 1)
	{
		printf("Error: Received no data!");
	}
	else
	{
		// check response: 0 = error / 1 = success
		if (buffer[0] == 1)
		{
			printf("OK!\n");
		}
	}*/
}
