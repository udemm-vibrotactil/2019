#ifndef FFT_H
#define FFT_H

#include "kiss_fftr.h" //Para utilizar kissFFT - Analizador de FFT

static float find_max(kiss_fft_cpx * cx_out, int size /* BUFSIZE*/, int sampling_freq,int f);
kiss_fft_cpx* copycpx(float *mat, int nframe);

#endif
