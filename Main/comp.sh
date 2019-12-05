#!/bin/bash
# Script de compilacion
# Proximamente usare Make

debug=""

salida="vibrotact"

entrada1="main.c"
entrada2="kiss_fftr.c"
entrada3="fft.c"
entrada4="Yin.c"
entrada5="selector.c"
entrada6="kiss_fft.c"
entrada7="i2c.c"
entrada8="alsa.c"
entrada9="encoder.c"

#echo -e "Modo debug [S/N]?"
read -p "Modo debug [S/N]?:" modo_debug

if [ $modo_debug == "S" ]; then
	debug1="-DDEBUG"
elif [ $modo_debug == "N" ]; then
	debug1=""
else
	echo "Error input"
	exit
fi

#gcc -O3 $entrada1 $entrada2 $entrada3 $entrada4 $entrada5 $entrada6 -Wall -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
#gcc -O3 $entrada1 $entrada2 $entrada3 $entrada4 $entrada5 $entrada6 $entrada7 $debug1 -Wall -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp 
gcc -mcpu=cortex-a53 -mtune=cortex-a53 -O3 $entrada1 $entrada2 $entrada3 $entrada4 $entrada5 $entrada6 $entrada7 $entrada8 $entrada9 $debug1 -Wall -o $salida -lstdc++ -lasound -lm -fopenmp -lwiringPi
#gcc $entrada1 $entrada2 $entrada3 $entrada5 $entrada6 $entrada7 $debug1 -Wall -o $salida -lpulse -lstdc++ -lpulse-simple -lm 
#-ffast-math
#gcc $entrada1 $entrada2 $entrada3 $entrada5 -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp

echo "Programa compilado - nombre del ejecutable: " $salida
