/*
Proyecto Vibrotact 2019

Modulo principal
----------------
Version 1.0

Parametros de compilacion
gcc -O3 main.c kiss_fftr.c kiss_fft.c filtros.c selector.c otras_funciones.c -o vibrotact -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
*/

//Inclusion de librerias
#include <stdio.h> //Requerido por PulseAudio API
#include <unistd.h> //Requerido por PulseAudio API
#include <string.h> //Requerido por PulseAudio API
#include <stdint.h> //Requerido por pYin
#include <errno.h> //Requerido para control de errores por PulseAudio API
//#include <stdlib.h> //Para utilizar files
#include <pulse/simple.h> //Para utilizar el PulseAudio API - modo simple
#include <pulse/error.h> //Para utilizar el PulseAudio API - manejo de errores
#include <omp.h> //Para utilziar OpenMP - Multiprocesamiento
#include <math.h> //Requerido por kissFFT
#include "kiss_fftr.h" //Para utilizar kissFFT - Analizador de FFT
#include "Yin.h" //Para utilizar pYin (analisis F0)
#include <time.h> //Para el calculo del tiempo de procesamiento
#include <fcntl.h> //Requerido para la comunicacion I2C
#include <linux/i2c-dev.h> //Para utilizar comunicacion I2C (ioctl)
#include <linux/i2c.h> //Para utilizar comunicacion I2C (ioctl)
//#include <linux/ioctl.h>

//Inclusion de funciones
#include "selector.h"
#include "i2c.h"
#include "fft.h"

//Definicion de constantes
#define AUDIO_BUF_SIZE 1024 //Defino la cantidad de samples a procesar
#define BUFSIZE (AUDIO_BUF_SIZE*2) //Requerido para el kissFFT
#define SAMPLING_FREQ 44100  //Defino la frecuencia de muestreo

#define PI 3.14159265359 //Cte Para la ventana de Hamming
#define a0 0.53836 //Cte Para la ventana de Hamming
#define a1 0.46164 //Cte Para la ventana de Hamming


// C function showing how to do time delay 
//#include <stdio.h> 
// To use time library of C 
//#include <time.h> 

void delay(int number_of_seconds) 
{ 
	// Converting time into milli_seconds 
	int milli_seconds = 1000 * number_of_seconds; 

	// Stroing start time 
	clock_t start_time = clock(); 

	// looping till required time is not acheived 
	while (clock() < start_time + milli_seconds); 
} 


/* Funcion Principal */
int main() {

	//Declaro las variables locales
	pa_simple *s = NULL;

	float buf[AUDIO_BUF_SIZE]; //Para el procesamiento FFT
	float buf2[AUDIO_BUF_SIZE]; //Para el procesamiento del glotal
    	int ret = 1;
    	int error;
	char vibrador1,vibrador2;
        float pitch;
	float periodo;
	clock_t t1;

	int j =0;
	char spinner[3]={'/','-','|'};

	#ifdef DEBUG
	  clock_t t;
	#endif
	
	/* Caracteristicas del SAMPLE - Configuracion del servidor PulseAudio */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_FLOAT32LE, // 32 bits Flotante Little Endian
        .rate = SAMPLING_FREQ, // if SAMPLING_FREQ is 44100 Hz, then Nyquist frequency is 22050 Hz
        .channels = 1 // Canal Mono
    };

	int numeroHilos=8; //Cantidad de hilos paralelos
	omp_set_num_threads(numeroHilos); //Configuracion de la cantidad de hilos


	Yin yin;

	/* Inicializo fft data structure */
	kiss_fft_cpx cx_out[AUDIO_BUF_SIZE],*cpx_buf;
	kiss_fftr_cfg fft = kiss_fftr_alloc(BUFSIZE ,0 ,0,0);

	/* Creo el stream de grabacion - Conexion con servidor PulseAudio */
	if (!(s = pa_simple_new(NULL, "vibrotact", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		goto finish;
		}
	printf("---> Inicio del programa <--- \n");
	//Bucle infinito - Comienzo del procesamiento
	for (;;) {
		#ifdef DEBUG
			t = clock(); //Comienzo a medir tiempo del bucle
		#endif
	t1 = clock(); //Comienzo a medir tiempo del bucle
	int i;


    	/* Lectura de SAMPLES y los almaceno en el array buf */
			if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
				fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
				goto finish;
			}
	/* Fin de lectura de SAMPLES */

	/* Aplico una ventana de Hamminga los samples */
	for(i=0;i<AUDIO_BUF_SIZE;i++){
		buf2[i] = buf[i];
		buf[i] = (a0 - a1 * cos(2*PI*i/(AUDIO_BUF_SIZE-1))) * buf[i]; //Aplico una ventana de Hamming
	}

	cpx_buf = copycpx(buf,BUFSIZE); //Realizo la copia
    	kiss_fftr(fft,(kiss_fft_scalar*)cpx_buf, cx_out); //Calculo el FFT

    	/* Inicio procesamiento en paralelo (F1, F2 y Glotal)*/

		#pragma omp parallel sections
			{
			#pragma omp section
			{
				/* Canal F1 */
				vibrador1 = selector(find_max(cx_out, AUDIO_BUF_SIZE, SAMPLING_FREQ,1)); //Hallo la frecuencia de mayor amplitud y selecciono el vibrador a activar
			}
			#pragma omp section
			{
				/* Canal F2 */
				vibrador2 = selector (find_max(cx_out, AUDIO_BUF_SIZE, SAMPLING_FREQ,2));
			}
			//Glottal Rate Detector (algoritmo de correlacion Yin)
			 #pragma omp section
                        {
	       			Yin_init(&yin, AUDIO_BUF_SIZE /*tamano Audio a analizar*/, 0.05/*Threshold incerteza decimal*/);
		 	        pitch = Yin_getPitch(&yin, buf2); //Devuelve la frec fundamental
			}

		 } //Al finalizar , ya generar una barrera (espera) para que finalicen todos los hilos abiertos
		
		//Mido tiempo de proceso y veo si es necesario realizar mas espera de apagado
		t1 = clock() - t1;
		/*
		if ((periodo - t1) > 0) { 
			delay(periodo - t1);
		}
		*/
		//Evaluo si hay pulso glotal
		if (pitch!=-1) {
			periodo = (float) 1/pitch;
			printf("F0 %.2f Hz - F1 CH %c - F2 CH %c - T %.3f seg \n",pitch,vibrador1,vibrador2,periodo);
			
			if (vibrador1 != 0x0){
				//envio F1 ON
				i2c_send(vibrador1,0x0,0xFF,0x0);
			}

			if (vibrador2 != 0x0){
				//envio F2 ON
				i2c_send(vibrador2,0x0,0xFF,0x0);
			}
			
			delay (periodo);
			//envio OFF
			
			i2c_send(vibrador1,0x0,0x0,0x0);
			i2c_send(vibrador2,0x0,0x0,0x0);
			
		}

	//Animacion de ejcucion
	printf("%c %d  \r",spinner[j],j);
	j = (j<2) ? j+1 : 0;

	//Calculo el tiempo de procesamiento (modo debug)
	#ifdef DEBUG
		t = clock() - t;
		printf ("Demora Proceso %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);
	#endif

	}


//Liberacion de recursos
	ret = 0;

finish: 

	kiss_fft_free(fft);
	kiss_fft_cleanup();

	if (s)
	        pa_simple_free(s);

    return ret;
}
