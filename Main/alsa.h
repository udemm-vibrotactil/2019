#ifndef ALSA_H
#define ALSA_H

#include <alsa/asoundlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int set_params(snd_pcm_t *capture_handle, unsigned int freq);

#endif
