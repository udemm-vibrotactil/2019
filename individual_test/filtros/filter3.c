//Prueba de Filtro 3 (Pasa Banda 1200 a 8000 Hz)
//Tambien utilizo FILES para la guarda del archivo
//gcc -O3 filter3.c -o filter3 -lpulse -lstdc++ -lpulse-simple -lm
/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 44100 Hz

* 0 Hz - 850 Hz
  gain = 0
  desired attenuation = -50 dB
  actual attenuation = -49.74225651391637 dB

* 1200 Hz - 8000 Hz
  gain = 1
  desired ripple = 2 dB
  actual ripple = 1.5862850436819564 dB

* 8500 Hz - 22050 Hz
  gain = 0
  desired attenuation = -50 dB
  actual attenuation = -49.74225651391637 dB

*/

#include <stdio.h>
#include <stdlib.h> //Necesario para usar FILES
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <time.h>
#define BUFSIZE2 2048 //Cantidad de samples que gaurdo en buffer 

//Inici Filtro pasa altos
#define N 141
static const double h[N] = 
{
0.01032684474384226,
    0.012225172889283637,
    0.008760010997854435,
    -1.884617624506811E-4,
    -0.006769869901274298,
    -0.005196169519879638,
    0.0013883631241722023,
    0.004186967040043251,
    -8.088571480304196E-4,
    -0.007700308303938531,
    -0.007844341449046865,
    -9.695078809362413E-4,
    0.0038653053800295975,
    -3.5777931630084014E-4,
    -0.009118658654006333,
    -0.011398901377668686,
    -0.0038142498639614146,
    0.004206954256192015,
    0.0019581244012400363,
    -0.008370908772183093,
    -0.013518680824449155,
    -0.0059397289677794425,
    0.005691725121428115,
    0.006754093975741145,
    -0.004302197150252917,
    -0.012918817571828363,
    -0.006611722608892115,
    0.008272567145354579,
    0.013635185220664283,
    0.0028514985519416795,
    -0.009673249685238862,
    -0.006253605785003109,
    0.010772333874336768,
    0.02083796805074204,
    0.011286333142552176,
    -0.005358277268913912,
    -0.006523758291039455,
    0.010997597755413627,
    0.025618366069706106,
    0.018171438712754364,
    -0.002485079439500246,
    -0.009671354972118644,
    0.006591244391589482,
    0.02531982005692116,
    0.02077610086642148,
    -0.0034499106821601544,
    -0.017558455530498642,
    -0.0038985779015801056,
    0.018651962415680658,
    0.01784297663579459,
    -0.009366811676953541,
    -0.03080176674170918,
    -0.02019801929576428,
    0.006785692346148284,
    0.01078556203443162,
    -0.01934342232375387,
    -0.04878524206002147,
    -0.04042992824261526,
    -0.00599618552644363,
    0.004863690870700601,
    -0.030535049795598446,
    -0.07203834439110841,
    -0.06361534181641862,
    -0.010687748295740096,
    0.014438034876384667,
    -0.039338012306276475,
    -0.12060662875324989,
    -0.1105169133662793,
    0.04635489692871118,
    0.2585296798993385,
    0.3573319826663677,
    0.2585296798993385,
    0.04635489692871118,
    -0.1105169133662793,
    -0.12060662875324989,
    -0.039338012306276475,
    0.014438034876384667,
    -0.010687748295740096,
    -0.06361534181641862,
    -0.07203834439110841,
    -0.030535049795598446,
    0.004863690870700601,
    -0.00599618552644363,
    -0.04042992824261526,
    -0.04878524206002147,
    -0.01934342232375387,
    0.01078556203443162,
    0.006785692346148284,
    -0.02019801929576428,
    -0.03080176674170918,
    -0.009366811676953541,
    0.01784297663579459,
    0.018651962415680658,
    -0.0038985779015801056,
    -0.017558455530498642,
    -0.0034499106821601544,
    0.02077610086642148,
    0.02531982005692116,
    0.006591244391589482,
    -0.009671354972118644,
    -0.002485079439500246,
    0.018171438712754364,
    0.025618366069706106,
    0.010997597755413627,
    -0.006523758291039455,
    -0.005358277268913912,
    0.011286333142552176,
    0.02083796805074204,
    0.010772333874336768,
    -0.006253605785003109,
    -0.009673249685238862,
    0.0028514985519416795,
    0.013635185220664283,
    0.008272567145354579,
    -0.006611722608892115,
    -0.012918817571828363,
    -0.004302197150252917,
    0.006754093975741145,
    0.005691725121428115,
    -0.0059397289677794425,
    -0.013518680824449155,
    -0.008370908772183093,
    0.0019581244012400363,
    0.004206954256192015,
    -0.0038142498639614146,
    -0.011398901377668686,
    -0.009118658654006333,
    -3.5777931630084014E-4,
    0.0038653053800295975,
    -9.695078809362413E-4,
    -0.007844341449046865,
    -0.007700308303938531,
    -8.088571480304196E-4,
    0.004186967040043251,
    0.0013883631241722023,
    -0.005196169519879638,
    -0.006769869901274298,
    -1.884617624506811E-4,
    0.008760010997854435,
    0.012225172889283637,
    0.01032684474384226
};

short filter(const double x_in)
{
    static int n = 0;
    static double x[N];
    int i;
    double y = 0.0;

    // Store the current input, overwriting the oldest input
    x[n] = x_in;

    // Multiply the filter coefficients by the previous inputs and sum
    for (i=0; i<N; i++)
    {
        y += h[i] * x[((N - i) + n) % N];
    }

    // Increment the input buffer index to the next location
    n = (n + 1) % N;

    return y;
}

//Fin del filtro pasa altos

int main(int argc, char*argv[]) {
    /* Especifico las caracteristicas del SAMPLE */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 1
    };
    pa_simple *s = NULL;
   /* Fin de especificaciones */
    int ret = 1;
    int error;
	clock_t t;
//Creo el archivo, lo pongo en duro para no escribir despues
    FILE *write_ptr;
    write_ptr = fopen("test_filter.raw","wb");  // w for write, b for binary

    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
      //  goto finish;
    }
    /* Fin de creacion del stream */
	int16_t buf2[BUFSIZE2]; //Creo el array para le buffer
//Loop para leer
    for (;;) {
	int i;
        /* Record some data ... */
        if (pa_simple_read(s, buf2, sizeof(buf2), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
	
	t = clock();

	for(i=0;i<BUFSIZE2;i++){
		buf2[i] = filter(buf2[i]);
	}
	t = clock() - t;

        /* Vuleco el buffer en un archivo */
  	fwrite(buf2,sizeof(buf2),1,write_ptr); 

    printf ("Demora %f ms \n",(1000*(float)t)/CLOCKS_PER_SEC);	
    }

    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
    return ret;


pa_simple_free(s); //Libero el stream
fclose(write_ptr); //Cierro el archivo
}
