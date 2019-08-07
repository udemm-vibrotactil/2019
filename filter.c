//Ejemplo de aplicacion de un filtro pasa Bajos con Fc a 5kHz
//Tambien utilizo FILES para la guarda del archivo

#include <stdio.h>
#include <stdlib.h> //Necesario para usar FILES
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE2 512 //Cantidad de samples que gaurdo en buffer 

//Inici Filtro pasa bajos
#define N 25
static const double h[N] = 
{
    8.047440128986703E-4,
    0.00234802526789238,
    0.005345421801512643,
    0.01025533664728186,
    0.01744056333116492,
    0.02700184501541485,
    0.038672365753250955,
    0.05177163284964986,
    0.06523994444357964,
    0.07776428537820018,
    0.0879695552416836,
    0.09464778671976695,
    0.09697172136912907,
    0.09464778671976695,
    0.0879695552416836,
    0.07776428537820018,
    0.06523994444357964,
    0.05177163284964986,
    0.038672365753250955,
    0.02700184501541485,
    0.01744056333116492,
    0.01025533664728186,
    0.005345421801512643,
    0.00234802526789238,
    8.047440128986703E-4,
};

short filter(const double x_in) //devuelvo short que es 16bits signed
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


//Fin del filtro pasa bajos

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
//Creo el archivo, lo pongo en duro para no escribir despues
    FILE *write_ptr;
    write_ptr = fopen("test_filter.raw","wb");  // w for write, b for binary

    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
      //  goto finish;
    }
    /* Fin de creacion del stream */

//ÃLoop para leer
    for (;;) {
	int16_t buf2[BUFSIZE2]; //Creo el array para le buffer
	int i;
        /* Record some data ... */
        if (pa_simple_read(s, buf2, sizeof(buf2), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
	for(i=0;i<BUFSIZE2;i++){
		buf2[i] = filter(buf2[i]);
	}
        /* Vuleco el buffer en un archivo */
  	fwrite(buf2,sizeof(buf2),1,write_ptr); 

    }

    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
    return ret;


pa_simple_free(s); //Libero el stream
fclose(write_ptr); //Cierro el archivo
}
