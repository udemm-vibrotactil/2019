/*
Proyecto Vibrotact 2019

Modulo principal
----------------
Version 1.0

*/

//Inclusion de librerias
#include <stdio.h> //Para la utilziacion de printf y sprinft
#include <unistd.h> //Requerido por PulseAudio API - Ver requerido por delay y tipos de datos
#include <string.h> //Para realizar operaciones con Strings
#include <stdint.h> //Requerido por pYin
#include <errno.h> //Requerido para control de errores
//#include <stdlib.h> //Para utilizar files
#include <omp.h> //Para utilziar OpenMP - Multiprocesamiento
#include <math.h> //Requerido por kissFFT
#include "kiss_fftr.h" //Para utilizar kissFFT - Analizador de FFT
#include "Yin.h" //Para utilizar pYin - Aanalisis F0 por autocorrelacion
#include <time.h> //Para el calculo del tiempo de procesamiento
#include <fcntl.h> //Requerido para la comunicacion I2C
#include <linux/i2c-dev.h> //Para utilizar comunicacion I2C (ioctl)
#include <linux/i2c.h> //Para utilizar comunicacion I2C (ioctl)
#include <alsa/asoundlib.h> //Para la utilizacion de sonido por medio de ALSA

//Inclusion de funciones
#include "selector.h"
#include "i2c.h"
#include "fft.h"
#include "alsa.h"

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
	snd_pcm_t *capture_handle; //Datos de captura de audio (samples)
//	snd_pcm_hw_params_t *hw_params; //Parametros de conf de ALSA
	int err;
	int nread;

	int32_t buf0[AUDIO_BUF_SIZE]; //Guardo los samples de Audio
	float buf[AUDIO_BUF_SIZE]; //Para el procesamiento FFT
	float buf2[AUDIO_BUF_SIZE]; //Para el procesamiento del glotal
    	int ret = 1;
	float M = 2147483648; //Cte para conversionde int32 a float (2 elevado a bits-1)
	char vibrador1,vibrador2;
        float pitch;
	float periodo=0;
	clock_t t1;

	int j =0;
	char spinner[4]={'/','-','|','\0'};

	#ifdef DEBUG
	  clock_t t;
	#endif


	int numeroHilos=8; //Cantidad de hilos paralelos
	omp_set_num_threads(numeroHilos); //Configuracion de la cantidad de hilos

	/* Inicializo el pYin - Analizador de F0 */
	Yin yin;
   	Yin_init(&yin, AUDIO_BUF_SIZE /*tamano Audio a analizar*/, 0.05/*Threshold, incerteza decimal*/);

	/* Inicializo fft data structure */
	kiss_fft_cpx cx_out[AUDIO_BUF_SIZE],*cpx_buf;
	kiss_fftr_cfg fft = kiss_fftr_alloc(BUFSIZE ,0 ,0,0);
        cpx_buf=(kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*BUFSIZE);
        kiss_fft_scalar zero;
        memset(&zero,0,sizeof(zero) );

	for(int k=0; k<BUFSIZE ; k++)
        {
                cpx_buf[k].r = zero;
                cpx_buf[k].i = zero;
        }



	/* Configuro el stream de grabacion */
	/* Abro el stream de grabacion */
        if ((err = snd_pcm_open (&capture_handle, "hw:0", SND_PCM_STREAM_CAPTURE, 0)) < 0){
            fprintf (stderr, "Error al abrir el dispositivo de audio [HW:0] (%s)\n",snd_strerror (err));
            exit (1);
          }

	if ((err=set_params(capture_handle, SAMPLING_FREQ)) < 0){
		printf("Error en la configuracion del sistema de audio");
		exit(1);
	}

	//Apago leds para indicar que se enceuntra listo
	i2c_send(vibrador1,0x0,0x0,0x0);
	i2c_send(vibrador2,0x0,0x0,0x0);

	printf("---> Inicio del programa <--- \n");

	//Bucle infinito - Comienzo del procesamiento
	for (;;) {
		#ifdef DEBUG
			t = clock(); //Comienzo a medir tiempo del bucle
		#endif
	t1 = clock(); //Comienzo a medir tiempo del bucle

    	/* Lectura de SAMPLES y los almaceno en el array buf0 */

	if ((nread = snd_pcm_readi (capture_handle, buf0, AUDIO_BUF_SIZE)) < 0) {
	      fprintf (stderr, "Falla de lectura desde la interfaz de audio (%s)\n",snd_strerror (err));
	      /* recover */
	      snd_pcm_prepare(capture_handle);
	    } else {

	/* Fin de lectura de SAMPLES */

	/* Aplico una ventana de Hamminga los samples */
		for(int i=0;i<AUDIO_BUF_SIZE;i++){
			buf[i] = (float) buf0[i]/M; //Convierto de int32 a float
			buf2[i] = buf[i]; //Copio a un segundo buffer
			buf[i] = (a0 - a1 * cos(2*PI*i/(AUDIO_BUF_SIZE-1))) * buf[i]; //Aplico una ventana de Hamming
                	cpx_buf[i].r = buf[i]; //Copio para el input del FFT
		}


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
				vibrador2 = selector(find_max(cx_out, AUDIO_BUF_SIZE, SAMPLING_FREQ,2)); //Hallo la frecuencia de mayor amplitud y selecciono el vibrador a activar
			}
			//Glottal Rate Detector (algoritmo de correlacion Yin)
			 #pragma omp section
                        {
		 	        pitch = Yin_getPitch(&yin, buf2); //Devuelve la frec fundamental
			}

		 } //Al finalizar , ya genera una barrera (espera) para que finalicen todos los hilos abiertos

		//Mido tiempo de proceso y veo si es necesario realizar mas espera de apagado

		t1 = clock() - t1;

		if ((periodo - t1) > 0) { 
			sleep(periodo - (t1/CLOCKS_PER_SEC));
		}

		//Evaluo si hay pulso glotal

		if (pitch!=-1) {
			periodo = (float) 1/pitch;
			printf("F0 %.2f Hz - F1 CH %x - F2 CH %x - T %.3f seg \n",pitch,vibrador1,vibrador2,periodo);

			//Solo puede existir 2 vibradores activos al mismo tiempo
			if (vibrador1 != 0x0){
				//envio F1 ON
				i2c_send(vibrador1,0xFF,0xFF,0x0);
			}

			if (vibrador2 != 0x0){
				//envio F2 ON
				i2c_send(vibrador2,0x0,0xFF,0x0);
			}

			//Espero el tiempo glotal antes de desactivarlos
			sleep (periodo);

			//envio OFF
			i2c_send(vibrador1,0x0,0x0,0x0);
			i2c_send(vibrador2,0x0,0x0,0x0);

		}

	//Animacion de ejcucion mintras no se detecta pulso glotal
	printf("%c %d  \r",spinner[j],j);
	if (j<2) {
		j++;
	} else {
		j=0;
		}
	}


	//Calculo el tiempo de procesamiento (modo debug)
	#ifdef DEBUG
		t = clock() - t;
		printf ("Demora Proceso %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);
	#endif

	}


//Liberacion de recursos
ret = 0;

if (fft){
	kiss_fft_free(fft);
	kiss_fft_cleanup();
}

if (cpx_buf)
	free(cpx_buf);

snd_pcm_close (capture_handle);

   return ret;
}
