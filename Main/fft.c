//Modulo de FFT
//----------------------------
//Busqueda del maximo valor
//Prepearacion de estrucutra de FFT
//


#include "fft.h"


//Funcion para hallar la frecuancia de maxima amplitud
float find_max(kiss_fft_cpx * cx_out, int size /* BUFSIZE*/, int sampling_freq,int f) {

	float max = 0;
	int max_freq = 0;
	int f_min, f_max;
	float amplitud;
	if (f==1) {
		f_min=100;
		f_max=1999;
	}

	if (f==2){
		f_max=7000;
		f_min=1200;
	}

	// Omito la frecuencia cero
	for (int i = 1; i < size / 2; i++) {

		int freq = sampling_freq * i / size;
		if(freq>f_min && freq<f_max){
//considerar amplitud energia cx_out.r²+cx_out.i²
			amplitud = pow(cx_out[i].r,2) + pow(cx_out[i].i,2);
			if (amplitud >= max) {
				max = amplitud;
				max_freq = freq;
			}
		}
	}

	return max_freq;
}

//Estrucutra del FFT
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
