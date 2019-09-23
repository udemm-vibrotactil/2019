/**
 * alsa_capture.c
 */

/**
 * Paul Davis
 * http://equalarea.com/paul/alsa-audio.html#howto
 */

/**
 * Jan Newmarch
 */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <signal.h>

#define BUFSIZE 1024
#define RATE 44100

FILE *fout = NULL;

/*
 * quit on ctrl-c
 */
void sigint(int sig) {
  if (fout != NULL) {
    fclose(fout);
  }
  exit(1);
}
	      
main (int argc, char *argv[])
{
  int i;
  int err;
  int32_t buf[BUFSIZE];
  float buf2[BUFSIZE];
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t rate = RATE;	
  int nread;


  if (argc != 3) {
    fprintf(stderr, "Usage: %s cardname file\n", argv[0]);
    exit(1);
  }

  if ((fout = fopen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Can't open %s for writing\n", argv[2]);
    exit(1);
  }


  signal(SIGINT, sigint);
	
  if ((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
	     argv[1],
	     snd_strerror (err));
    exit (1);
  }
		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf (stderr, "cannot set access type (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
	

  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S32_LE)) < 0) {
    fprintf (stderr, "cannot set sample format (%s)\n",
	     snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    fprintf (stderr, "cannot set sample rate (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
  fprintf(stderr, "rate set to %d\n", rate);
	
  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
    fprintf (stderr, "cannot set channel count (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
	
  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
	
  snd_pcm_hw_params_free (hw_params);
	
  /*
  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
  */
int j;
float M = 2147483648;
  while (1) {
    if ((nread = snd_pcm_readi (capture_handle, buf, BUFSIZE)) < 0) {
      fprintf (stderr, "read from audio interface failed (%s)\n",
	       snd_strerror (err));
      /* recover */
      snd_pcm_prepare(capture_handle);
    } else {

for(j=0;j<BUFSIZE;j++) {
	buf2[j]=(float)buf[j]/M;
//	printf("%f \n",(float)buf[j]/M);
}
      fwrite(buf2, sizeof(float), nread, fout);
    }
  }
	
  snd_pcm_close (capture_handle);
  exit(0);
}
