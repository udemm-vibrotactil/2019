/*
Proyecto Vibrotact 2019

Modulo principal
----------------
Version 1.0

Parametros de compilacion
gcc -O3 main.c kiss_fft.c filtros.c selector.c otras_funciones.c -o vibrotact -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
*/

//Inclusion de librerias
#include <stdio.h> //Requerido por PulseAudio API
#include <unistd.h> //Requerido por PulseAudio API
#include <string.h> //Requerido por PulseAudio API
#include <stdint.h> //Requerido por pYin
#include <errno.h>
#include <stdlib.h> //Para utilizar files - grabacion de archivos para debug
#include <pulse/simple.h> //Para utilizar el PulseAudio API - modo simple
#include <pulse/error.h> //Para utilizar el PulseAudio API - manejo de errores
#include <omp.h> //Para utilziar OpenMP - Multiprocesamiento
#include <math.h> //Requerido por kissFFT
//#include "kiss_fft.h" //Para utilizar kissFFT - Analizador de FFT
#include "kiss_fftr.h" //Para utilizar kissFFT - Analizador de FFT
#include "Yin.h" //Para utilizar pYin (analisis F0)
#include <time.h> //Para el calculo del tiempo de procesamiento

//Inclucion de funciones

#include "selector.h"
#include "filtros.h"
//#include "otras_funciones.h"


//Definicion de constantes
//#define AUDIO_BUF_SIZE 2048 //Defino la cantidad de samples a procesar
#define AUDIO_BUF_SIZE 1024 //Defino la cantidad de samples a procesar
//#define BUFSIZE (AUDIO_BUF_SIZE*4) //Requerido para el kissFFT
#define BUFSIZE (AUDIO_BUF_SIZE*2) //Requerido para el kissFFT
#define SAMPLING_FREQ 44100  //Defino la frecuencia de muestreo

#define PI 3.14159265359 //Cte Para la ventana de Hamming
#define a0 0.53836 //Cte Para la ventana de Hamming
#define a1 0.46164 //Cte Para la ventana de Hamming

//Funcion para hallar la frecuancia de maxima amplitud
static float find_max(kiss_fft_cpx * cx_out, int size /* BUFSIZE*/, int sampling_freq,int f) {

	float max = 0;
	int max_freq = 0;

	// Omito la frecuencia cero
	for (int i = 1; i < size / 2; i++) {

		int freq = sampling_freq * i / size;

		if (cx_out[i].r >= max) {
			max = cx_out[i].r;
			max_freq = freq;
		}

	}

	return max_freq;
}

kiss_fft_cpx* copycpx(float *mat, int nframe) {
	int i;
	kiss_fft_cpx *mat2;
	mat2=(kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*nframe);
        kiss_fft_scalar zero;
        memset(&zero,0,sizeof(zero) );
	for(i=0; i<nframe ; i++)
	{
		mat2[i].r = mat[i];
		mat2[i].i = zero;
	}
	return mat2;
}

/* Funcion Principal */
int main() {

	//Declaro las variables locales
	pa_simple *s = NULL;

	float buf[AUDIO_BUF_SIZE];
	float buf2[AUDIO_BUF_SIZE];
	float buf3[AUDIO_BUF_SIZE];
	#ifdef DEBUG
	  clock_t t;
	#endif
    	int ret = 1;
    	int error;
	int vibrador1,vibrador2;

	/* Caracteristicas del SAMPLE - Configuracion del servidor PulseAudio */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_FLOAT32LE, // 32 bits Flotante Little Endian
        .rate = SAMPLING_FREQ, // if SAMPLING_FREQ is 44100 Hz, then Nyquist frequency is 22050 Hz
        .channels = 1 // Canal Mono
    };

	int numeroHilos=8; //Cantidad de hilos paralelos
	omp_set_num_threads(numeroHilos); //Configuracion de la cantidad de hilos

	kiss_fft_cpx out_cpx[AUDIO_BUF_SIZE],/*out[size],*/*cpx_buf;
	kiss_fftr_cfg fft = kiss_fftr_alloc(BUFSIZE ,0 ,0,0);
	
	/*
	kiss_fft_cfg cfg;
    	kiss_fft_cpx * cx_in;
    	kiss_fft_cpx * cx_in2;
    	kiss_fft_cpx * cx_out;
    	kiss_fft_cpx * cx_out2;
	*/
	
	Yin yin;
	
	/* Inicializo fft data structure */
	//cfg = kiss_fft_alloc( BUFSIZE ,/*is_inverse_fft*/ 0 ,0,0 );

	//cx_in = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
	//cx_in2 = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
	//cx_out = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
	//cx_out2 = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
		
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

    	/* Lectura de SAMPLES y los almaceno en el array buf */
			if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
				fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
				goto finish;
			}
	/* Fin de lectura de SAMPLES */
	
	cpx_buf = copycpx(buf,BUFSIZE); //Realizo la copia
    	kiss_fftr(fft,(kiss_fft_scalar*)cpx_buf, out_cpx); //Calculo el FFT
	for(i=0;i<AUDIO_BUF_SIZE;i++){
		buf[i] = (a0 - a1 * cos(2*PI*i/(AUDIO_BUF_SIZE-1))) * buf[i]; //Aplico una ventana de Hamming
	}
//    	memcpy(cx_in, &cx_in[AUDIO_BUF_SIZE], (BUFSIZE - AUDIO_BUF_SIZE) * sizeof(kiss_fft_cpx));
//    	memcpy(cx_in2, &cx_in2[AUDIO_BUF_SIZE], (BUFSIZE - AUDIO_BUF_SIZE) * sizeof(kiss_fft_cpx));

    	/* Inicio procesamiento en paralelo (F1 y F2)*/

		#pragma omp parallel sections
			{
			#pragma omp section
			{
				/* Canal F1 */
				/*
				int i;
				for(i=0;i<AUDIO_BUF_SIZE;i++){
					buf2[i] = filter_fpb(buf[i]); //Aplico filtro pasa bajos
					
					cx_in[i + BUFSIZE - AUDIO_BUF_SIZE].r = buf2[i]; //Hago copia  del buffer de audio al buffer del FFT
				}
				kiss_fft( cfg , cx_in , cx_out ); //Aplicto FFT 
				*/
				vibrador1 = selector(find_max(cx_out, BUFSIZE, SAMPLING_FREQ,1)); //Hallo la frecuencia de mayor amplitud y selecciono el vibrador a activar
			}
			#pragma omp section
			{
				/* Canal F2 */
				/*
				int i;
				for(i=0;i<AUDIO_BUF_SIZE;i++){
					buf3[i] = filter_fpa(buf[i]); //Aplico filtro pasa altos
					buf3[i] = (a0 - a1 * cos(2*PI*i/(AUDIO_BUF_SIZE-1))) * buf3[i]; //Aplico una ventana de Hamming
					cx_in2[i + BUFSIZE - AUDIO_BUF_SIZE].r = buf3[i]; //Hago copia  del buffer de audio al buffer del FFT
				}
				kiss_fft( cfg , cx_in2 , cx_out2 );
				*/
				vibrador2 = selector (find_max(cx_out2, BUFSIZE, SAMPLING_FREQ,2));
			}
			//Glottal Rate Detector (algoritmo de correlacion Yin)
			 #pragma omp section
                        {
			        float pitch;
	       			Yin_init(&yin, AUDIO_BUF_SIZE /*tamano Audio a analizar*/, 0.05/*Threshold incerteza decimal*/);
		 	        pitch = Yin_getPitch(&yin, buf); //Devuelve la frec fundamental
			        printf("Pitch is found to be %f Hz and probabiity %f\n",pitch, Yin_getProbability(&yin) );
			}
		 }


	//Calculo el tiempo de procesamiento (modo debug)
	#ifdef DEBUG
		t = clock() - t;
		printf ("Demora %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);
	#endif

	}


//Liberacion de recursos
	ret = 0;

finish: 

	free(cx_out);
	free(cx_out2);
	free(cx_in);
	free(cx_in2);

	kiss_fft_free(cfg);

	if (s)
        pa_simple_free(s);

    return ret;
}
