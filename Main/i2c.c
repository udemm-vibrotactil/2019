//Modulo I2C
//-------------
//Permite enviar ordenes por I2C a los diferentes perifereicos
//La OrangePi actua como Master y el resto como Esclavos (Slave)

/*
 References  :
 http://binerry.de/post/27128825416/raspberry-pi-with-i2c-arduino-slave
*/

#include "i2c.h"
#define ATTINY 0x0A  //ATTiny85 
#define DRV2605 0x5A //Vibrador
#define TCAADDR 0x70 // Multiplexor
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

int DRVwriteRegister8 (char reg, char val)
{
	int deviceHandle;
	int readBytes;
	char buffer_drv[2];

	// address of i2c DRV2605 device
	int deviceI2CAddress = DRV2605;  // (0x5A)

	// open device on /dev/i2c-0
	if ((deviceHandle = open("/dev/i2c-0", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}

	// connect to DRV2605 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0) {
		printf("Error: Couldn't find DRV2605 on address!\n");
		return 1;
	} 

	// begin transmission and request acknowledgement

	buffer_drv[0] = reg;
	buffer_drv[1] = val;
	//readBytes=write(deviceHandle, &reg,1);
	//readBytes=write(deviceHandle, &val,1);
	readBytes=write(deviceHandle, buffer_drv,2);	

	// close connection and return
	close(deviceHandle);
	printf("DRV reg: %x val: %x\n", reg, val); 
	return 0;
}

int tcaselect (char i)
{
	if (i > 7) return 1;
	int deviceHandle;
	int readBytes;

	// address of i2c MUX device
	int deviceI2CAddress = TCAADDR;  // (0x70)

	// open device on /dev/i2c-0
	if ((deviceHandle = open("/dev/i2c-0", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}

	// connect to MUX as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0) {
		printf("Error: Couldn't find MUX on address!\n");
		return 1;
	} 
	
	i = 1 << i;
	// begin transmission and request acknowledgement

	readBytes=write(deviceHandle, &i,1);
	

	// close connection and return
	close(deviceHandle);
	printf("MUX %x\n",i);
	return 0;
}

int i2c_vibrador (char vibrador, char modo) {
	tcaselect(vibrador);
	sleep(0.01);
	DRVwriteRegister8(0x0C, modo);      //0x0C => drv.go() ---> Manejo ARRANQUE 1 PARADA 0
	return 0;
}

void init_DRV2605 () {

for (int t=0; t<7; t++) {
      	tcaselect(t);
	DRVwriteRegister8(0x01, 0x00);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x02, 0x00);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x03, 0x06);      //drv.begin()   useLibrary(6)
	sleep(0.01);
	DRVwriteRegister8(0x04, 0x01);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x05, 0x00);      //drv.begin()
	sleep(0.01);
      	DRVwriteRegister8(0x0D, 0x00);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x0E, 0x00);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x0F, 0x00);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x10, 0x00);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x13, 0x64);      //drv.begin()
	sleep(0.01);
	DRVwriteRegister8(0x1A, 0xB6);      //drv.begin()   useLRA()
	sleep(0.01);
	DRVwriteRegister8(0x1D, 0xA0);      //drv.begin()
      	//BUZZ 100% waveform  47  0x2F
      	//BUZZ 80% waveform   48  0x30
      	//BUZZ 60% waveform   49  0x31
      	//BUZZ 40% waveform   50  0x32
      	//BUZZ 20% waveform   51  0x33
	//STRONG CLICK 1 100% 17  0x11
	//STRONG CLICK 2 80%  18  0x12
	//STRONG CLICK 3 60%  19  0x13
	//STRONG CLICK 4 30%  20  0x14
	sleep(0.01);
	DRVwriteRegister8(0x04, 0x12);      //drv.setWaveform:0,x
	sleep(0.01);
	DRVwriteRegister8(0x05, 0x00);      //drv.setWaveform:1,0
	} 
}
