#ifndef I2C_H
#define I2C_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int i2c_send(char led, char R, char G, char B); //Utilizada en la funcion Main
void i2c_command(char command); //Utilizada por el i2c_send

#endif
