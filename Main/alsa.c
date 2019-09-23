//Modulo de ALSA
//--------------
//Permite configurar la placa de sonido en formato y modo, con su respectivo control
//de errores

#include "alsa.h"

int set_params(snd_pcm_t *capture_handle, unsigned int freq)
{
  int err;
  snd_pcm_hw_params_t *hw_params;
  unsigned int rate = freq;

	/* Abro el stream de grabacion */
/*
        if ((err = snd_pcm_open (&capture_handle, "hw:0", SND_PCM_STREAM_CAPTURE, 0)) < 0){
            fprintf (stderr, "Error al abrir el dispositivo de audio [HW:0] (%s)\n",snd_strerror (err));
            exit (1);
          }
*/
        /* Allocate a hardware parameters object. */
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
            fprintf (stderr, "no puede asignar estructura de parámetros de hardware (%s)\n",snd_strerror (err));
            exit (1);
          }
	/* Fill it in with default values. */
        if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
            fprintf (stderr, "Error en la inicializacion de los parametros de estructura del hardware (%s)\n",snd_strerror (err));
            snd_pcm_close(capture_handle);
	    exit (1);
          }
        /* Set the desired hardware parameters. */
	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0){
            fprintf (stderr, "No se puede establecer el tipo de acceso (%s)\n",snd_strerror (err));
            snd_pcm_close(capture_handle);
	    exit (1);
          }
	//Utilizacion de datos de 32 bits Entero con Signo Little Endian
        if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S32_LE)) < 0) {
            fprintf (stderr, "Error en la configuracion del formato del Sample [Singned Int 32 LE] (%s)\n",snd_strerror (err));
            snd_pcm_close(capture_handle);
	    exit (1);
          }
        //Si SAMPLING_FREQ es 44100 Hz, entonces la frecuencia de Nyquist es 22050 Hz
         if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
            fprintf (stderr, "Error en al configuracion de la velocidad de muestreo [44100 Hz] (%s)\n",snd_strerror (err));
	    snd_pcm_close(capture_handle);
            exit (1);
          }
		//Configuracion del numero de canales (mono = 1)
	 if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
            fprintf (stderr, "Error en la configuracion de canales [Mono] (%s)\n",snd_strerror (err));
            snd_pcm_close(capture_handle);
	    exit (1);
          }
		/* Write the parameters to the driver */
         if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
            fprintf (stderr, "No puede establecer parámetros (%s)\n",snd_strerror (err));
            snd_pcm_close(capture_handle);
	    exit (1);
          }

 snd_pcm_hw_params_free (hw_params);
 return err;
}
