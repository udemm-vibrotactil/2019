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

echo -e "Modo debug [S/N]?"
read modo_debug

if [ modo_debug="S" ]
then
   debug="-DDEBUG"
fi

gcc -O3 $entrada1 $entrada2 $entrada3 $entrada4 $entrada5 $entrada6 -Wall -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
#gcc -O3 $entrada1 $entrada2 $entrada3 $entrada4 $entrada5 $entrada6 $debug -Wall -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
#gcc $entrada1 $entrada2 $entrada3 $entrada5 -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp

echo "Programa compilado - nombre del ejecutable: " $salida
