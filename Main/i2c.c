//Modulo I2C
//-------------
//Permite enviar ordenes por I2C a los diferentes perifereicos
//La OrangePi actua como Master y el resto como Esclavos (Slave)

/*
 References  :
 http://binerry.de/post/27128825416/raspberry-pi-with-i2c-arduino-slave
*/

#include "i2c.h"

int deviceHandle;
int readBytes;
char buffer[7];

int i2c_send (int led, int R, int G, int B)
{
	// initialize buffer
	buffer[0] = 0x00;

	// address of i2c ATTiny device
	int deviceI2CAddress = 0x0A;  // (0x2A = 42)

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
	
	// close connection and return
	close(deviceHandle);
	return 0;
}



// function for testing command
void i2c_command(int command_int)
{
	char command_str[4];
	snprintf(command_str, sizeof(command_str), "%d", command_int);
//	itoa(command_int,command_str,10); //Convierto de entero a string
	readBytes = write(deviceHandle, command_str , sizeof(command_str));
	// give arduino some reaction time
	usleep(1000); // 1ms

	// Lee ACK, comprueba que este ok lo enviado
	readBytes = read(deviceHandle, buffer, 1);
	if (readBytes != 1)
	{
		printf("Error: Received no data!");
	}
/*
	else
	{
		// check response: 0 = error / 1 = success
		if (buffer[0] == 1)
		{
			printf("OK!\n");
		}
	}*/
}
