//Ejemplo de aplicacion de un filtro pasa altos con Fc a 7kHz
//Tambien utilizo FILES para la guarda del archivo

#include <stdio.h>
#include <stdlib.h> //Necesario para usar FILES
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE2 512 //Cantidad de samples que gaurdo en buffer 

//Inici Filtro pasa altos
#define N 27
static const double h[N] = 
{
    -0.009123678620765853,
    0.03382711107244206,
    -0.033443655471315975,
    -0.012061853619367117,
    0.02266499260223626,
    0.026110800489631488,
    -0.013076237941850788,
    -0.04582001420755287,
    -0.011063375652899457,
    0.06642297115846281,
    0.0690020993767205,
    -0.08216210124423179,
    -0.30487070186848236,
    0.5880590792522308,
    -0.30487070186848236,
    -0.08216210124423179,
    0.0690020993767205,
    0.06642297115846281,
    -0.011063375652899457,
    -0.04582001420755287,
    -0.013076237941850788,
    0.026110800489631488,
    0.02266499260223626,
    -0.012061853619367117,
    -0.033443655471315975,
    0.03382711107244206,
    -0.009123678620765853,
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
