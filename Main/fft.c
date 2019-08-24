/***
Ejemplo basado en FFT.c, usando 2 FFT con OpenMP
compilar con
gcc -O3 fft3.c kiss_fft.c -o fft -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
***/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <omp.h>

#include <math.h>
#include "kiss_fft.h"

#include <time.h>

#define PI 3.14159265359

#define AUDIO_BUF_SIZE 2048

#define BUFSIZE (AUDIO_BUF_SIZE*4)

#define DEBUG 0

#define SAMPLING_FREQ 44100



static float find_max(kiss_fft_cpx * cx_out, int size, int sampling_freq,int F) {

	float max = 0;
	int max_freq = 0;

	static int counter;

	// skip frequency zero
	for (int i = 1; i < size / 2; i++) {

		int freq = sampling_freq * i / size;

		if (cx_out[i].r >= max) {
			max = cx_out[i].r;
			max_freq = freq;
		}

	}

	// ANSI/VT100 Terminal Control Escape Sequences
	// http://www.termsys.demon.co.uk/vtansi.htm
//	printf("%5d#  \033[32m  MAX_FREQ=%5d \033[30m  VALUE=%.0f\r", counter++, max_freq, max);
	printf("%5d#  MAX_FREQ=%5d  AMPLITUD=%.0f F=%d \n", counter++, max_freq, max,F);



	return max_freq,max;
}


int main(int argc, char*argv[]) {
	/**
	 * very simple example which iterates these steps:
	 * from audio input, records BUFSIZE samples at sampling frequency SAMPLING_FREQ,
	 * then calculates fft of recorded samples,
	 * then looks up the frequency at which the spectrum has the absolute maximum (uses only the real component of spectrum).
	 */


    /* The sample type to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE, // Intel CPU architecture uses little endian memory representation
        .rate = SAMPLING_FREQ, // if SAMPLING_FREQ is 44100 Hz, then Nyquist frequency is 22050 Hz
        .channels = 1 // mono
    };

    pa_simple *s = NULL;

    int ret = 1;

    int error;

    int numeroHilos=4;
    int numeroProcesadores = omp_get_num_procs();

    omp_set_num_threads(numeroHilos);

	clock_t t;

    kiss_fft_cfg cfg;
    kiss_fft_cpx * cx_in;
    kiss_fft_cpx * cx_in2;
    kiss_fft_cpx * cx_out;
    kiss_fft_cpx * cx_out2;

    /* init fft data structure */
	cfg = kiss_fft_alloc( BUFSIZE ,/*is_inverse_fft*/ 0 ,0,0 );

	cx_in = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
	cx_in2 = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
	cx_out = calloc(BUFSIZE, sizeof(kiss_fft_cpx));
	cx_out2 = calloc(BUFSIZE, sizeof(kiss_fft_cpx));


	/* Create the recording stream */
	if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		goto finish;
		}


    uint16_t buf[AUDIO_BUF_SIZE];

    printf("buf size: %ld bytes\n",sizeof(buf));
    printf("Este computador usa %d procesador(es)\n", numeroProcesadores);
    printf("En este ejemplo se desea usar %d hilo(s)\n", omp_get_max_threads());

    for (;;) {
			t = clock();

    			/* Record some data ... */
			if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
				fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
				goto finish;
			}


    	memcpy(cx_in, &cx_in[AUDIO_BUF_SIZE], (BUFSIZE - AUDIO_BUF_SIZE) * sizeof(kiss_fft_cpx));
    	memcpy(cx_in2, &cx_in2[AUDIO_BUF_SIZE], (BUFSIZE - AUDIO_BUF_SIZE) * sizeof(kiss_fft_cpx));

//Aplico una ventana de Hann
	for (int i = 0; i < AUDIO_BUF_SIZE; i++) {
	    double multiplier = 0.5 * (1 - cos(2*PI*i/8191));
	    buf[i] = multiplier * buf[i];
	}
//Fin de la ventana

//Hago copia  del buffer de audio al buffer del FFT
    	for (int i = 0; i < AUDIO_BUF_SIZE; i++) {
    		cx_in[i + BUFSIZE - AUDIO_BUF_SIZE].r = buf[i];
    		cx_in2[i + BUFSIZE - AUDIO_BUF_SIZE].r = buf[i]*2;
    	}

    	/* calculo fft en paralelo*/

	 #pragma omp parallel sections 
		{
		#pragma omp section
			{
			kiss_fft( cfg , cx_in , cx_out );
//		    	printf("F1 salida %.0f!\n",cx_out[100].r);
			find_max(cx_out, BUFSIZE, SAMPLING_FREQ,1);
			}
		#pragma omp section
			{
			kiss_fft( cfg , cx_in2 , cx_out2 );
		    	//printf("F2 salida %.0f!\n",cx_out2[100].r);
			find_max(cx_out2, BUFSIZE, SAMPLING_FREQ,2);
			}
		 }

   t = clock() - t;
   printf ("Demora %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);

  }

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
