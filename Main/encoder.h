#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <wiringPi.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

extern volatile int set_mode;
extern volatile int mode;

int config_encoder (); //Configuracion del GPIO usado por el Encoder
void encoderSW(); //Interrupcion del Switch
void encoderPulse(); //Interrupcion por el movimiento ratativo del Encoder
int sensibilidad(); //Ajuste de sensibilidad (volumen) por MIC ALSA
int intensidad(); //Ajuste de la intensidad (nivel de vibracion)

#endif
