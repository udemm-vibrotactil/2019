//Modulo Encoder
//-------------
//Configura el GPIO para la utilizacion de un Encoder
//Establece las funciones de interrupcion y configuracion
//de sensibilidad (volumen de MIC en ALSA)

#include "encoder.h"
#include "i2c.h"

// Define the Raspberry Pi IO Pins being used
#define ENCODER_A 12		// GPIO 23
#define ENCODER_B 13		// GPIO 24
#define ENCODER_SW 6		// GPIO 26

#define HW "hw:0"
#define MIC "Mic1 Boost"

#define TRUE	1
#define FALSE	0

//static  limitado al source file y volatile por la interrupcion
static volatile int encoderPos;
static volatile int lastEncoded;
static volatile int encoded;
volatile int inCriticalSection = FALSE;

volatile int set_mode=FALSE;
volatile int set_mode2=FALSE;

//Interrupcion del Switch del Encoder
void encoderSW (){
	if (set_mode == FALSE) {
		set_mode = TRUE;
	}
	else
		if (set_mode2 == FALSE){ 
			set_mode2 = TRUE;
		}
}

//Configuracion del GPIO
int config_encoder (){
//Modo configuracion de GPIO
  wiringPiSetup ();

//Configuracion del Switch Encoder
  pinMode (ENCODER_SW, INPUT);           // Modo entrada
  pullUpDnControl (ENCODER_SW, PUD_UP) ;  // Modo Pul-Upp n

//Configuracion pulsos Encoder
  pinMode (ENCODER_A, INPUT);
  pullUpDnControl (ENCODER_A, PUD_UP);
  pinMode (ENCODER_B, INPUT);
  pullUpDnControl (ENCODER_B, PUD_UP);

/* monitor encoder level changes */
  wiringPiISR (ENCODER_SW, INT_EDGE_FALLING, &encoderSW);
  wiringPiISR (ENCODER_A, INT_EDGE_BOTH, &encoderPulse);
  wiringPiISR (ENCODER_B, INT_EDGE_BOTH, &encoderPulse);

  return 0 ;
}

//Ajuste de sensibilidad (volumen) con Encoder
int sensibilidad(){
  double time_mode = 0;
  clock_t tmode_start;
  clock_t tmode_end;
  int pos = 125;
  long min, max;
  long gpiodelay_value = 100;
  //int max_indicador=7;
  //int min_indicador=0;
  //int indicador = 5;

  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;

  const char *card = HW;
  const char *selem_name = MIC;
  int x;
  long currentVolume;

  encoderPos = pos;

   // Setup ALSA access
   snd_mixer_open(&handle, 0);
   snd_mixer_attach(handle, card);
   snd_mixer_selem_register(handle, NULL, NULL);
   snd_mixer_load(handle);

   snd_mixer_selem_id_alloca(&sid);
   snd_mixer_selem_id_set_index(sid, 0);
   snd_mixer_selem_id_set_name(sid, selem_name);
   snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

   snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
   #ifdef DEBUG
	printf("Returned card VOLUME range - min: %ld, max: %ld\n", min, max);
   #endif

   // Minimum given is mute, we need the first real value
   min++;

   // Get current volume
   if ((x = snd_mixer_selem_get_capture_volume (elem, SND_MIXER_SCHN_FRONT_LEFT, &currentVolume))<0)
   {
        printf("%d %s\n", x, snd_strerror(x));
   }
   //else if (DEBUG_PRINT) printf("Current ALSA volume LEFT: %ld\n", currentVolume);

   if ((x = snd_mixer_selem_get_capture_volume (elem, SND_MIXER_SCHN_FRONT_RIGHT, &currentVolume))<0)
   {
        printf("%d %s\n", x, snd_strerror(x));
   }
   //else if (DEBUG_PRINT) printf("Current ALSA volume RIGHT: %ld\n", currentVolume);
   printf("Current ALSA volume RIGHT: %ld\n", currentVolume);

   i2c_luzoff();
   //Muestro en los leds en base al estado actual
   for(int j=0;j<currentVolume;j++){
   	   i2c_send(j,0x0,0x0,0xFF);
	}
   tmode_start = clock();

// Now sit and spin waiting for GPIO pins to go active...
// Ver de poner el tiempo de time out para salir
   while (time_mode < 1.5)
   {
	if (set_mode2 == TRUE){
		 intensidad();
		 break;
	}

        tmode_end = clock();
	time_mode = ((double)(tmode_end - tmode_start)/CLOCKS_PER_SEC)*1000; //+ time_mode;
	printf("time %f \n", time_mode);

	if (encoderPos != pos)
	      {
		      tmode_start = clock(); // Reseteo el contador de tiempo
		      time_mode=0;
		      snd_mixer_handle_events(handle); //handle external events such that volume is correct
              // get current volume
		      if ((x = snd_mixer_selem_get_capture_volume (elem, SND_MIXER_SCHN_FRONT_LEFT, &currentVolume))<0)
        	      {
        		printf("%d %s\n", x, snd_strerror(x));
	              }
	//	      else if (DEBUG_PRINT) printf(" Current ALSA volume: %ld\n", currentVolume);

              // Adjust for MUTE
		      if (currentVolume < min) 
        	      {
                        currentVolume = 0;
			#ifdef DEBUG
				printf(" Current ALSA volume set to min: %ld\n", currentVolume);
			#endif
		       // i2c_luzoff();
	   	   i2c_send(0,0x0,0x0,0x0);

	              }

              // What way did the encoder go?
		     if (encoderPos > pos)
		      {
		         pos = encoderPos;
			 currentVolume = currentVolume + 1;
			 //indicador=indicador+1;
			 // Adjust for MAX volume
			 if (currentVolume > max) currentVolume = max;
			// if (indicador > max_indicador) indicador = max_indicador;
			#ifdef DEBUG
				printf("Volume UP %d - %ld", pos, currentVolume);
			#endif
			for(int j=0;j<(currentVolume);j++){
			   	   i2c_send(j,0x0,0x0,0xFF);
			}

		        //i2c_send(indicador-1,0x0,0x0,0xFF);

		      }
			else if (encoderPos < pos)
		      {
		         pos = encoderPos;
                         currentVolume = currentVolume - 1;
			 //indicador=indicador - 1; 

			 // Adjust for MUTE
			 if (currentVolume < min) currentVolume = 0;
 			 //if (indicador < min_indicador) indicador = 0;
			 for(int j=7;j>currentVolume;j--){
			   	   i2c_send(j,0x0,0x0,0x0);
			 }



			 #ifdef DEBUG
				printf("Volume DOWN %d - %ld", pos, currentVolume);
 			 #endif
		      }

	              if ((x = snd_mixer_selem_set_capture_volume_all(elem, currentVolume))<0)
	              {
           	          printf(" ERROR %d %s\n", x, snd_strerror(x));
	              } //else if (DEBUG_PRINT) printf(" Volume successfully set to %ld using ALSA!\n", currentVolume);
		}

		// Check x times per second, MAY NEED TO ADJUST THS FREQUENCY FOR SOME ENCODERS */
		delay(gpiodelay_value); /* check pos x times per second */
	   }


   // We never get here but should close the sockets etc. on exit.
   snd_mixer_close(handle);
   set_mode=FALSE;
   i2c_luzoff();
   return 0;
}


int intensidad(){
  double time_mode = 0;
  clock_t tmode_start;
  clock_t tmode_end;
  int pos = 125;
//  long min, max;
  long gpiodelay_value = 100;
  int max_indicador=4;
  int min_indicador=0;
  int indicador = 2;

//  int x;
//  long currentVolume;

  encoderPos = pos;


   // Minimum given is mute, we need the first real value
  // min++;


   //Muestro en los leds el estado por default 
   i2c_luzoff();
	for(int j=0;j<indicador;j++){
   	   i2c_send(j,0xFF,0x0,0x0);
	}

   tmode_start = clock();

// Now sit and spin waiting for GPIO pins to go active...
// Ver de poner el tiempo de time out para salir
   while (time_mode < 1.5)
   {
        tmode_end = clock();
	time_mode = ((double)(tmode_end - tmode_start)/CLOCKS_PER_SEC)*1000; //+ time_mode;
	printf("time %f \n", time_mode);

	if (encoderPos != pos)
	      {
		      tmode_start = clock(); // Reseteo el contador de tiempo
		      time_mode=0;


              // What way did the encoder go?
		      if (encoderPos > pos)
		      {
		         pos = encoderPos;
		//	 currentVibrator = currentVibrator + 1;
			 indicador=indicador+1;
			 // Adjust for MAX volume
		//	 if (currentVolume > max) currentVolume = max;
			 if (indicador > max_indicador) indicador = max_indicador;
			   for(int j=0;j<=indicador;j++){
   	   			i2c_send(j,0xFF,0x0,0x0);
				}

		      }
			else if (encoderPos < pos)
		      {
		         pos = encoderPos;
                  //       currentVolume = currentVolume - 1;
			 indicador=indicador - 1; 

			 // Adjust for MUTE
		//	 if (currentVolume < min) currentVolume = 0;
 			 if (indicador < min_indicador) indicador = 0;
			 for(int j=4;j>indicador;j--){
			   	   i2c_send(j,0x0,0x0,0x0);
			 }
		      }
		}

		// Check x times per second, MAY NEED TO ADJUST THS FREQUENCY FOR SOME ENCODERS 
		delay(gpiodelay_value); // check pos x times per second 
	   }


   set_mode2=FALSE;
//   set_mode=FALSE;
//   i2c_luzoff();
   return 0;
}




//Interrupcion por movimiento del Encoder
void encoderPulse(){
   /*
             +---------+         +---------+      0
             |         |         |         |
   A         |         |         |         |
             |         |         |         |
   +---------+         +---------+         +----- 1
       +---------+         +---------+            0
       |         |         |         |
   B   |         |         |         |
       |         |         |         |
   ----+         +---------+         +---------+  1
   */

	if (inCriticalSection==TRUE) return;

	inCriticalSection = TRUE;

	int MSB = digitalRead(ENCODER_A);
        int LSB = digitalRead(ENCODER_B);

        int encoded = (MSB << 1) | LSB;
        int sum = (lastEncoded << 2) | encoded;

        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderPos++;
	else if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderPos--;

        lastEncoded = encoded;

	inCriticalSection = FALSE;
}
