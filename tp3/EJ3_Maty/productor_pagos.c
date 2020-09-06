#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
//Semaforos
#define SEM_ESCRITURA "/sem-escritura"
#define SEM_MUTEX "/sem-mutex"
#define SEM_LECTURA "/sem-lecturaPago"
#define SEM_WHILE "/sem-whilePago"
#define SEM_PROCESO "/sem-proceso"
//Memoria compartida
#define SHARED_MEM_PAYMENTS "/payments-shared-mem"
//Archivo
#define FILE_NAME "pagos.txt"
#define MODE_READ_TXT "rt"

void error(char *msg);
int openFile(FILE **pf, char *name, char *mode);
void paymentsProcessSupplier();

int main(int argc , char *argv[])
{
    if(argc > 1){
	if (strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0)
	{
		printf("----------------------------------------------------------------------------.\n");
		printf("El archivo SOCIO debe estar ejecutando antes de iniciar los clientes.\n");
		printf("Pagos ira copiando a memoria compartida.\n");
		printf("Debe existir el archivo pagos.txt en el directorio actual\n");
		printf("Ejecute pagos cuantas veces quiera para recalcular el total. Se mostrara en SOCIOS.\n");
		printf("Para iniciar PAGOS, simplemente ejecute ./pagos\n");
		printf("----------------------------------------------------------------------------.\n");
		return 0;
	}
	else
	{
		printf("Error en los parametros ingresados\n");
		printf("Use como parametro [-help] o [-h] para obtener la ayuda.\n");
		return 0;
	}
    }

	
	puts("Iniciando proceso.");
    sem_t *sem_proceso;
    if ((sem_proceso = sem_open(SEM_PROCESO, 0, 0660, 1)) == SEM_FAILED)
        error("sem_proceso_open");
    paymentsProcessSupplier();

    return 0;
}

void paymentsProcessSupplier()
{
    int fd_shm;
    char mybuf[256];
    char *shared_mem_ptr;
    FILE *pf;
    sem_t *sem_escritura, *sem_lectura, *sem_mutex, *sem_while;

    if ((sem_escritura = sem_open(SEM_ESCRITURA, 0, 0660, 1)) == SEM_FAILED)
        error("sem_escritura_open");

    if ((sem_mutex = sem_open(SEM_MUTEX, 0, 0660, 1)) == SEM_FAILED)
        error("sem_mutex_open");

    if ((sem_lectura = sem_open(SEM_LECTURA, 0, 0660, 0)) == SEM_FAILED)
        error("sem_lectura_open");

    if ((sem_while = sem_open(SEM_WHILE, 0, 0660, 1)) == SEM_FAILED)
        error("sem_while_open");

    if (!openFile(&pf, FILE_NAME, MODE_READ_TXT))
        error("Error, the file could not be open.\n");

    //  open shared mem
    if ((fd_shm = shm_open(SHARED_MEM_PAYMENTS, O_RDWR, 0)) == -1)
        error("shm_open");

    if ((shared_mem_ptr = mmap(NULL, sizeof(mybuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    char line[256];

    while (fgets(line, sizeof(line), pf))
    {
        // P (mutex_sem);
        if (sem_wait(sem_escritura) == -1)
            error("sem_wait: sem_escritura");

        if (sem_wait(sem_mutex) == -1)
            error("sem_wait: sem_mutex");

        memcpy(shared_mem_ptr, line, sizeof(line));

        if (sem_post(sem_mutex) == -1)
            error("sem_post: sem_mutex");

        if (sem_post(sem_lectura) == -1)
            error("sem_post: sem_lectura");

        printf("Copy to shared Memory : %s", line);
        printf("%s \n", strchr(line, '\n'));
    }

    if (sem_wait(sem_while) == -1)
        error("sem_post: sem_while");
        
    fclose(pf);
    close(fd_shm);
}

int openFile(FILE **pf, char *name, char *mode)
{
    *pf = fopen(name, mode);
    return (!*pf) ? 0 : 1;
}

// Print system error and exit
void error(char *msg)
{
    perror(msg);
    exit(1);
}
