//Modulo I2C
//-------------
//Permite enviar ordenes por I2C a los diferentes perifereicos
//La OrangePi actua como Master y el resto como Esclavos (Slave)

/*
 References  :
 http://binerry.de/post/27128825416/raspberry-pi-with-i2c-arduino-slave
*/

#include "i2c.h"
#define ATTINY 0x0A 
//char buffer[7];

int i2c_send (char numled, char R, char G, char B)
{
	int deviceHandle;
	int readBytes;

	// initialize buffer
	//buffer[0] = 0x00;

	// address of i2c ATTiny device
	int deviceI2CAddress = ATTINY;  // (0x2A = 42)

	// open device on /dev/i2c-0
	if ((deviceHandle = open("/dev/i2c-0", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}

	// connect to ATTiny as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0) {
		printf("Error: Couldn't find ATTiny on address!\n");
		return 1;
	} 

	// begin transmission and request acknowledgement

	readBytes=write(deviceHandle, &numled,1);
	readBytes=write(deviceHandle, &R,1);
	readBytes=write(deviceHandle, &G,1);
	readBytes=write(deviceHandle, &B,1);

/*
	readBytes = write(deviceHandle, buffer, 1);
	if (readBytes != 1)
	{
		printf("Error: Received no ACK-Bit, couldn't established connection!");
	}
	else
	{
		//Datos a enviar 
		i2c_command(led);//Numero de LED (0 al 7)
		i2c_command(R);//Color Rojo (Red)
		i2c_command(G);//Color Verde(Green)
		i2c_command(B);//Color Azul (Blue)
	}
*/	

	// close connection and return
	close(deviceHandle);
	return 0;
}



