#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
/*
	 Grupo 5 Integrantes:
		MIRANDA SERGIO JAVIER - 35.634.266
		CARRIL ARANDA JOSE - 37.120.410
		LUNA SANCHEZ MATIAS LEONEL - 37.141.163
		RAMIREZ MATIAS GASTON - 39.505.838
		MASINI JOAN EZEQUIEL - 39.562.503

*/
typedef struct
{
	int nTotal;
	long* result;

}tThreadParameters;

typedef struct
{
	long* result;
	long* finalResult;
	int nTotal;

}tThreadResultParameters;

typedef struct 
{		
	long nThreads;
}tArg ;

//===========================================================================
//DEFINICION DE VARIABLES GLOBALES
//===========================================================================
pthread_t* threadsId;
// Puntero a parametros de threads comunes
tThreadParameters* threadParams;
// Puntero a parametros de threads resultados
tThreadResultParameters* threadResultParams;
long* resultVector;
long* finalResult;
tArg arg;

//===========================================================================
//FUNCIONES
//===========================================================================
void ayuda()		
{
	printf("\nGRUPO 5 EJERCICIO 2\n\n");
	printf("\nAYUDA PARA EJECUTAR EL PROGRAMA Ej2\n\n");
	printf("NOMBRE\n" );
	printf("\tEjercicio 2 \n");
	printf("DESCRIPCION\n");
	printf("\tEl programa recibe un parametro N, y luego realiza operaciones en paralelo de los N numeros \n\t correspondientes a la serie de Fibonacci\n\tmediante el uso de threads.\n\tPor ultimo mostrara en pantalla un resultado final utilizando el \n\tresultado de los threads en cuestion.\n"); 
	printf("SYNOPSIS\n");
	printf("\tEl ejercicio consta de ingresar un parametro N y que mediante una serie de calculos\n\tdevuelva un resultado.\n");
	printf("\tEl parametro 'N':\n\tN refiere a la cantidad de threads ejecutando en paralelo  \n\tdistintas operaciones para los primeros N numeros de la serie de Fibonacci\n\n");
	printf("EJEMPLO CON PARAMETRO N\n");
	printf("\tIngrese la ejecucion del ejercicio con el nombre y a continuacion el valor N a calcular.\n");
	printf("\tEj:\n");
	printf("\t./EjercicioDos 5 \n");
	printf("\tNOTA: Tener en cuenta que el valor max de N es el de un entero equivalente a 32767.\n");
	
	exit(2);
}

void* acumularFibonacciHastaN(void* args){
	tThreadParameters* params=(tThreadParameters*)args;
	
	int i;
	long t1 = 1, t2 = 1,
		nextTerm, 
		acum=0,
		num = params->nTotal;
		
	//printf("Serie de fibonacci con N: %d elementos\n",num);	
	
	for (i =1; i <= num; ++i) {
		//printf("%ld, ", t1);
		acum += t1;
		nextTerm = t1 + t2;
		t1 = t2;
		t2 = nextTerm;
	}	
	*(params->result) = acum;
	//printf("Se genera calculo con \tNtotal: %ld valores \tAcumulado: %ld\n", params->nTotal,*(params->result));
	
}

void* sumAcum(void* args){
	tThreadResultParameters* params=(tThreadResultParameters*)args;
	//printf("Thread para sumar los acumulados %ld\n",*(params->result + 0));
	int i;
	long acum = 0;
	
	for(i=0;i<params->nTotal;i++){
		acum += *(params->result + i);
	}
	*(params->finalResult) = acum;
	//printf("Sumados y acumulados %ld\n",*(params->finalResult));
}

void* mulAcum(void* args){
	tThreadResultParameters* params=(tThreadResultParameters*)args;
	// printf("Thread para multiplicar los acumulados %ld\n",*(params->result + 0));
	int i;
	long acum = 1;
	
	for(i=0;i<params->nTotal;i++){
		acum *= *(params->result + i);
	}
	*(params->finalResult) = acum;
	//printf("Multiplicados y acumulados %ld\n",*(params->finalResult));
}

//===========================================================================
//MAIN
//===========================================================================
int main(int argc, char const *argv[])
{    
	int nValue,i;

	/*Verifico argumento*/
	if(argv[0] == NULL || argc != 2) {
		printf("\nError en los parametros, ingrese el parametro -help para mas informacion\nej2 -help o ej2.c -h\n");
		exit(EXIT_FAILURE);
	}else{
		if(strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0)  {
			ayuda();
		}
		
		nValue= atoi(argv[1]);
		if(nValue<=0)
		{
			printf("\nError en los parametros, ingrese un valor de N mayor a 0.\nUse -help para mas informacion\nej2 -help o ej2.c -h\n");
			exit(EXIT_FAILURE);
		}
	}
	
	// Guardo parametros que recibo
	arg.nThreads = nValue;

	// Estoy reservando memoria secuencial de N * tamanio de threads.
	// ThreadsId es el puntero inicial a la 1er posicion reservada para cada thread.
	
	// Puntero a pos de mem que necesita mi vector de parmetros para cad thread.
	threadParams = (tThreadParameters*)malloc(arg.nThreads * sizeof(tThreadParameters));
	
	// Puntero a posiciones de memoria donde stan los threads.
	threadsId = (pthread_t*)malloc((arg.nThreads+2) * sizeof(pthread_t));
	
	// Vector con una posicion para que cada thread deposite su resultado al finalizar.
	resultVector = (long*)malloc(arg.nThreads * sizeof(long));
	
	// Quiero solo un vector con dos posiciones para meter en uno todo lo multiplicado y en el otro todo lo sumado.
	finalResult = (long*)malloc(2 * sizeof(long));
	
	// Puntero a pos de mem que necesita mi vector de parmetros para cad thread.
	threadResultParams = (tThreadResultParameters*)malloc(2*sizeof(tThreadResultParameters));
	
	// Creo los threads y espero a que terminen
	for(i = 0; i < arg.nThreads; i++) {
	
		threadParams[i].nTotal = (arg.nThreads - i); // Le voy pasando el total decrementado.
		threadParams[i].result = (resultVector + i); // Esto es un puntero, y lo hago apuntar adonde reserve memoria.
		
		//printf("Parametro de hilo: %d \tNtotal: %ld \tResultDir: %u\tResultVal: %u\n", i+1, threadParams[i].nTotal, threadParams[i].result,*(threadParams[i].result));

		pthread_create(&threadsId[i],NULL,acumularFibonacciHastaN,&threadParams[i]);
	}
	for(i = 0; i < arg.nThreads; i++) {
		pthread_join(threadsId[i],NULL);	
	}
	

	
	// Segunda creacion de threads para sumar y multiplicar acumulados.
	int subIndexTHR_sum = arg.nThreads,
		subIndexTHR_mul = arg.nThreads+1;
	
	// Parametros para sumar
	threadResultParams[0].nTotal = arg.nThreads;
	threadResultParams[0].result = resultVector;
	threadResultParams[0].finalResult = finalResult;
	
	// Parametros para multiplicar
	threadResultParams[1].nTotal = arg.nThreads;
	threadResultParams[1].result = resultVector;
	threadResultParams[1].finalResult = (finalResult+1);
	
	pthread_create(&threadsId[subIndexTHR_sum],NULL,sumAcum,&threadResultParams[0]);
	pthread_create(&threadsId[subIndexTHR_mul],NULL,mulAcum,&threadResultParams[1]);
	pthread_join(threadsId[subIndexTHR_sum],NULL);	
	pthread_join(threadsId[subIndexTHR_mul],NULL);
	
	// Se obtiene el resultado final restando la productoria - sumatoria.
	long resFinal = *(finalResult+1)-*finalResult;
	// Ver los valores del vector de resultados de threads separados.

	//printf("Threads N: %ld\n\n",arg.nThreads);
		
	// Visualizar la serie
	// long t1 = 1, t2 = 1,
	// nextTerm, 
	// num = arg.nThreads;
	// for (i =1; i <= num; ++i) {
		// printf("%ld, ", t1);
		// nextTerm = t1 + t2;
		// t1 = t2;
		// t2 = nextTerm;
	// }	
	
	// printf("\nVector de resultados acumulados POR THREAD INDEPENDIENTE.\n");
	// for(i=0;i<arg.nThreads;i++){
		// printf("Thread %d \tValor: %ld\n",i+1,(resultVector[i]));
	// }
	// printf("\nSumatoria de los resultados: %ld\n",*finalResult);
	// printf("Productoria de los resultados: %ld\n",*(finalResult+1));
	printf("Resultado final: %ld\n",resFinal);
	printf("==================== FIN DE EJERCICIO 2 ====================\n");
	return 0;
}
