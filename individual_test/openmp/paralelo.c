//Programa de ejemplo que utiliza el OpenMP APi para trabajar en paralelo
#include <stdio.h>
#include <omp.h>

int main(){

	int numeroHilos=10;
	int numeroProcesadores = omp_get_num_procs();
	omp_set_num_threads(numeroHilos);

	printf("Este computador usa %d procesador(es)\n", numeroProcesadores);
	printf("En este ejemplo se desea usar %d hilo(s)\n", omp_get_max_threads());

	printf("\n**Corrida normal**\n\n");
	printf("En este momento se esta(n) ejecutando %d hilo(s)\n", omp_get_num_threads());
//En forma lineal, unico proceso
	  for(int i=0;i<10;i++){
	    printf("%i\n",i);	
	  }

	printf("\n**Corrida paralelo**\n\n");

//Armo el bloque paralelo,puede  salir desordenado porque cada iteracion
// lo ejecuta un hilo distinto
	#pragma omp parallel for
	  for(int i=0;i<10;i++){
	    printf("%i\n",i);
	    int idHilo = omp_get_thread_num();
	    printf("Hola, soy el hilo %d, en este momento se esta(n) ejecutando %d hilo(s)\n", idHilo, omp_get_num_threads());
	  }
	  printf("Acabo de salir de la seccion paralela\n");
	  printf("\nEn este momento se esta(n) ejecutando %d hilo(s)\n", omp_get_num_threads());
	  return 0;

}
