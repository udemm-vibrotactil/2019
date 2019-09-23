#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "i2c.h"
#include "lcd.h"

#define I2C_FILE_NAME "/dev/i2c-0"

char* txt = "be happy"; 

int main(){
    int i2c_dev;
    unsigned int i, j, dir;
    // 0x27 is the address of the i2c device
    i2c_dev = open_i2c(I2C_FILE_NAME, 0x09);

    if(i2c_dev <0){
       printf("Errore: %d\n", i2c_dev);
       return 1;
    }

set_i2c_dat(i2c_dev, 0x133);
set_i2c_dat(i2c_dev, 0x134);
set_i2c_dat(i2c_dev, 0x135);
set_i2c_dat(i2c_dev, 0x136);

    close_i2c(i2c_dev);
    return 0;
}
