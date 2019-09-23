//Modulo de FFT
//----------------------------
//Busqueda del maximo valor
//Prepearacion de estrucutra de FFT
//


#include "fft.h"
#define INTENSIDAD 3

//Funcion para hallar la frecuancia de maxima amplitud
float find_max(kiss_fft_cpx * cx_out, int size /*AUDIO_ BUFSIZE*/, int sampling_freq,int f) {

	float max = 0;
	int max_freq = 0;
	int f_min = 0;
	int f_max = 0;
	float amplitud;


	if (f==1) {
		f_min=100;
		f_max=1999;
	}else if (f==2){
		f_max=7000;
		f_min=1200;
	}

	// Omito la frecuencia cero
	for (int i = 1; i < size ; i++) {
		//Calculo el bin (frecuencia) a analizar
		int freq = sampling_freq * i / size;
		if(freq>f_min && freq<f_max){
			//Normalizo la amplitud
			amplitud = 2*sqrt((cx_out[i].r*cx_out[i].r) + (cx_out[i].i*cx_out[i].i));
			//Transformo en veces (escala logaritmica) la amplitud
			//amplitud_db=20*log10(amplitud) 
			if (amplitud >= max && amplitud > INTENSIDAD) {
				max = amplitud;
				max_freq = freq;
			}
		}
	}
	//Puede devolver 0 que representa que no hubo coincidencia
	//printf("Amplitud %f \n",max);
	return max_freq;
}

