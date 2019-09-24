//Modulo de seleccion de canal
//----------------------------
//Devuelve el del vibrador a activar como entero
//Frecuencia analisis de 100 Hz a 7000 Hz
//Conformado por 7 SubCanales (4 primeros al canal F1 y 3 ultimos al canal F2)

//DRV2605L is 0x5A (7-bit), or 1011010 in binary.

#include "selector.h"

//Selector de Canal para F1
char selector_F1 (int freq) {

	char result;

	switch (freq)
        {
    	case 100 ... 399:
      	/* SubCanal 1 - 100 Hz a 399 Hz */
	  #ifdef DEBUG
		printf ("F1 - SubCanal 1 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
      	  result = 0x1;
	  break;
    	case 400 ... 599:
      	/* SubCanal 2 - 400 Hz a 599 Hz */
      	  #ifdef DEBUG
		printf ("F1 - SubCanal 2 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x2;
	  break;
	case 600 ... 799:
      	/* SubCanal 3 - 600 Hz a 799 Hz */
      	  #ifdef DEBUG
		printf ("F1 - SubCanal 3 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x3;
	  break;
	case 800 ... 1199:
      	/* SubCanal 4 - 800 Hz a 1199 Hz */
      	  #ifdef DEBUG
		printf ("F1 - SubCanal 4 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x4;
	  break;
	default:
	  /* No hay coincidencia de frecuencia */
	  #ifdef DEBUG
		printf ("No hay coincidencia - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x0;
	  break;
	}

	return result;
}

//Selector de Canal para F2
char selector_F2 (int freq) {

	char result;

	switch (freq)
        {
	case 1200 ... 1599:
      	/* SubCanal 5 - 1200 Hz a 1599 Hz */
      	  #ifdef DEBUG
		printf ("F2 - SubCanal 5 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x5;
	  break;
	case 1600 ... 2999:
      	/* SubCanal 6 - 1600 Hz a 2999 Hz */
      	  #ifdef DEBUG
		printf ("F2 - SubCanal 6 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x6;
	  break;
	case 3000 ... 7000:
      	/* SubCanal 7 - 3000 Hz a 7000 Hz*/
      	  #ifdef DEBUG
		printf ("F2 - SubCanal 7 Detectado - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x7;
	  break;
	default:
	  /* No hay coincidencia de frecuencia */
	  #ifdef DEBUG
		printf ("No hay coincidencia - Frecuencia=%d Hz \n",freq);
	  #endif
	  result = 0x0;
	  break;
	}

	return result;
}

