//Compilar con
//gcc fft.c kiss_fft.c -o fft -lpulse -lstdc++ -lpulse-simple -lm

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include <math.h>

#include "kiss_fft.h"
#include "kiss_fftr.h"

#include <time.h>

//#define AUDIO_BUF_SIZE 1024

#define BUFSIZE 1024
//#define BUFSIZE (AUDIO_BUF_SIZE*4)

#define DEBUG 0

#define SAMPLING_FREQ 44100


kiss_fft_cpx* copycpx(float *mat, int nframe)
{
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



static float find_max(kiss_fft_cpx * cx_out, int size, int sampling_freq) {

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

	printf("%5d#  MAX_FREQ=%5d  VALUE=%.0f\n", counter++, max_freq, max);



	return max_freq;
}



int main(int argc, char*argv[]) {
    /* The sample type to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_FLOAT32LE, // Intel CPU architecture uses little endian memory representation
        .rate = SAMPLING_FREQ, // if SAMPLING_FREQ is 44100 Hz, then Nyquist frequency is 22050 Hz
        .channels = 1 // mono
    };

    pa_simple *s = NULL;

    int ret = 1;

    int error;

    kiss_fft_cpx out_cpx[BUFSIZE],/*out[size],*/*cpx_buf;

    kiss_fftr_cfg fft = kiss_fftr_alloc(BUFSIZE*2 ,0 ,0,0);


    /* init fft data structure */
//	cfg = kiss_fftr_alloc( BUFSIZE ,/*is_inverse_fft*/ 0 ,0,0 );


	if (DEBUG) {

	} else {
		/* Create the recording stream */
		if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
			fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
			goto finish;
		}
	}

    float buf[BUFSIZE];

    printf("buf size: %ld bytes\n",sizeof(buf));

    for (;;) {
	clock_t t;
	t=clock();
	float amplitud;

			/* Record some data ... */
			if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
				fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
				goto finish;
			}

	cpx_buf = copycpx(buf,BUFSIZE);
    	kiss_fftr(fft,(kiss_fft_scalar*)cpx_buf, out_cpx);
/*
    	for (int i = 0; i < AUDIO_BUF_SIZE; i++) {
    		cx_in[i + BUFSIZE - AUDIO_BUF_SIZE] = buf[i];
    		//cx_in[i].i = 0.f;
    	}
*/
    	/* calculate fft */
//    	kiss_fftr( cfg , cx_in , cx_out );
//    	find_max(out_cpx, BUFSIZE, SAMPLING_FREQ);

	for (int i = 1; i < BUFSIZE / 2; i++) {

                int freq = SAMPLING_FREQ * i / BUFSIZE;
		amplitud = pow(out_cpx[i].r,2) + pow(out_cpx[i].i,2);
		if (amplitud > 2){ 
		printf("FREQ=%5d  VALUE=%.0f\n",freq,amplitud);
			}

        }


	t = clock() - t;  //Fin de la medición y diferencia
	printf ("Demora %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);
        if (DEBUG)
        	break;
    }

    ret = 0;
finish:
/*
	free(cx_out);
	free(cx_in);

	kiss_fft_free(cfg);
*/
	    kiss_fft_cleanup();   
	    free(fft);

	if (s)
        pa_simple_free(s);

    return ret;
}
