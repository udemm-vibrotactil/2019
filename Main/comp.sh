#!/bin/bash
# Script de compilacion
# Proximamente usare Make

debug=""

salida="vibrotact"

entrada1="main.c"
entrada2="kiss_fft.c"
entrada3="filtros.c"
entrada4="Yin.c"
entrada5="selector.c"

echo -e "Modo debug [S/N]?"
read modo_debug

if [ modo_debug="S" ]
then
   debug="-DDEBUG"
fi

gcc -O3 $entrada1 $entrada2 $entrada3 $entrada4 $entrada5 -Wall -o $salida $debug -lpulse -lstdc++ -lpulse-simple -lm -fopenmp
#gcc $entrada1 $entrada2 $entrada3 $entrada5 -o $salida -lpulse -lstdc++ -lpulse-simple -lm -fopenmp

echo "Programa compilado - nombre del ejecutable: " $salida
