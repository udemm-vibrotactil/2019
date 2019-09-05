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
//#include <stdlib.h> //Para utilizar files - grabacion de archivos para debug
#include <pulse/simple.h> //Para utilizar el PulseAudio API - modo simple
#include <pulse/error.h> //Para utilizar el PulseAudio API - manejo de errores
#include <omp.h> //Para utilziar OpenMP - Multiprocesamiento
#include <math.h> //Requerido por kissFFT
#include "kiss_fftr.h" //Para utilizar kissFFT - Analizador de FFT
#include "Yin.h" //Para utilizar pYin (analisis F0)
#ifdef DEBUG
	#include <time.h> //Para el calculo del tiempo de procesamiento
#endif

//Inclusion de funciones
#include "selector.h"
//#include "filtros.h"
#include "fft.h"

//Definicion de constantes
#define AUDIO_BUF_SIZE 1024 //Defino la cantidad de samples a procesar
#define BUFSIZE (AUDIO_BUF_SIZE*2) //Requerido para el kissFFT
#define SAMPLING_FREQ 44100  //Defino la frecuencia de muestreo

#define PI 3.14159265359 //Cte Para la ventana de Hamming
#define a0 0.53836 //Cte Para la ventana de Hamming
#define a1 0.46164 //Cte Para la ventana de Hamming


/* Funcion Principal */
int main() {

	//Declaro las variables locales
	pa_simple *s = NULL;

	float buf[AUDIO_BUF_SIZE];
	float buf2[AUDIO_BUF_SIZE];
    	int ret = 1;
    	int error;
	int vibrador1,vibrador2;
        float pitch;


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

	//Bucle infinito - Comienzo del procesamiento
	for (;;) {
		#ifdef DEBUG
			t = clock();
		#endif
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
				vibrador1 = selector(find_max(cx_out, BUFSIZE, SAMPLING_FREQ,1)); //Hallo la frecuencia de mayor amplitud y selecciono el vibrador a activar
			}
			#pragma omp section
			{
				/* Canal F2 */
				vibrador2 = selector (find_max(cx_out, BUFSIZE, SAMPLING_FREQ,2));
			}
			//Glottal Rate Detector (algoritmo de correlacion Yin)
			 #pragma omp section
                        {
	       			Yin_init(&yin, AUDIO_BUF_SIZE /*tamano Audio a analizar*/, 0.05/*Threshold incerteza decimal*/);
		 	        pitch = Yin_getPitch(&yin, buf2); //Devuelve la frec fundamental
			}

		 }

	//	#pragma omp barrier //espero que finalice los threads anteriores
	//		{
				if (pitch!=-1) {
					printf("F0 %f Hz \t",pitch);
					printf("F1 CH %d \t",vibrador1);
					printf("F2 CH %d \n",vibrador2);
				}
	//		}

	//Calculo el tiempo de procesamiento (modo debug)
	#ifdef DEBUG
		t = clock() - t;
		printf ("Demora %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);
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
