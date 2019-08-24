#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE 1024 //Cantidad de bytes que me traigo
#define BUFSIZE2 512 //Cantidad de samples BUFSIZE/2 , lo se , lo tengo que mejorar

/* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;
        if ((r = write(fd, data, size)) < 0)
            return r;
        if (r == 0)
            break;
        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }
    return ret;
}



int main(int argc, char*argv[]) {
    /* Especifico las caracteristicas del SAMPLE */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 1
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    float percent = 0.6f; //Factor de multiplicacion para el volumen, 0 = 0%  de volumen

    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    for (;;) {
        uint8_t buf[BUFSIZE]; //Es el buffer que leo de Pulse, me devuelve 1 byte
	uint16_t buf2[BUFSIZE2]; //este buffer tengo los samples de 2 bytes
	int j,i;
        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
	for(i=0,j=0;i<BUFSIZE2;i++,j+=2){
		buf2[i] = buf[j+1]<<8|buf[j]; //Armo sample en formato Little Endian
		buf2[i] = (uint16_t) (buf2[i] * percent); // amplifico
	}
        /* And write it to STDOUT */
        if (loop_write(STDOUT_FILENO, buf2, sizeof(buf2)) != sizeof(buf2)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }
    }
    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
    return ret;
}
