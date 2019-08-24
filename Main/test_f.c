//Prueba Unitaria de  Filtro 
//Tambien utilizo FILES para la guarda del archivo
//gcc -O3 filter3.c -o filter3 -lpulse -lstdc++ -lpulse-simple -lm


#include <stdio.h>
#include <stdlib.h> //Necesario para usar FILES
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <time.h>
#define BUFSIZE2 2048 //Cantidad de samples que gaurdo en buffer 
#include "filtros.c"



int main(int argc, char*argv[]) {

    /* Especifico las caracteristicas del SAMPLE */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_FLOAT32LE,
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

    float buf2[BUFSIZE2]; //Creo el array para le buffer
    
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
		buf2[i] = filter_fpb(buf2[i]);
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
