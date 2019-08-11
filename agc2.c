//Compilar con
// gcc agc2.c -o agc -lliquid -lm -lc -lpulse-simple -lpulse -lstdc++

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE 1024 //Cantidad de bytes que me traigo
# include "liquid-dsp/include/liquid.h"

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
        data = (const float*) data + r;
        size -= (size_t) r;
    }
    return ret;
}



int main(int argc, char*argv[]) {
    /* Especifico las caracteristicas del SAMPLE */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_FLOAT32LE,
        .rate = 44100,
        .channels = 1
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;

//Configuro el AGC

	agc_rrrf q = agc_rrrf_create(); // create object
	agc_rrrf_set_bandwidth(q, 0.01f); // set loop filter BW
	agc_rrrf_set_scale(q,0.01f);
//	agc_rrrf_set_signal_level(q,1e-3f); 
//	agc_rrrf_set_gain_limits(q,1,1);

    // initialize squelch functionality
    agc_rrrf_squelch_enable(q);             // enable squelch
    agc_rrrf_squelch_set_threshold(q, -110); // threshold for detection [dB]
    agc_rrrf_squelch_set_timeout  (q, 100); // timeout for hysteresis

//Termina el AGC

    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    for (;;) {
        float buf[BUFSIZE]; //Es el buffer que leo de Pulse, me devuelve 1 byte
	int i;

        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
	

	for(i=0;i<BUFSIZE;i++){
		agc_rrrf_execute(q, buf[i], &buf[i]);
	}

        /* And write it to STDOUT */
        if (loop_write(STDOUT_FILENO, buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }
    }
    ret = 0;
finish:
agc_rrrf_destroy(q);
    if (s)
        pa_simple_free(s);
    return ret;
}
