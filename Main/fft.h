#ifndef FFT_H
#define FFT_H

#include "kiss_fftr.h" //Para utilizar kissFFT - Analizador de FFT

float find_max_F1(kiss_fft_cpx * cx_out, int size /* BUFSIZE*/, int sampling_freq);
float find_max_F2(kiss_fft_cpx * cx_out, int size /* BUFSIZE*/, int sampling_freq);

#endif
