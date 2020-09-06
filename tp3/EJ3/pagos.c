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

#define MODE_READ_TXT "rt"
#define SEM_MUTEX_1 "/sem-mutex-1"
#define SEM_MUTEX_PAYMENT "/sem-mutex-payment"
#define SEM_MUTEX_TOTAL "/sem-mutex-total"
#define SHARED_MEM_PAYMENTS "/payments-shared-mem"
#define FILE_NAME "pagos.txt"

void error(char *msg);
int openFile(FILE **pf, char *name, char *mode);
void paymentsProcessSupplier();

int main(int argc , char *argv[])
{
	if (argc <=1 )
	{// Si el no tiene parametros, quiere ser ejecutado.
		puts("Iniciando proceso.");
		paymentsProcessSupplier();
		return 0;
	}

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

void paymentsProcessSupplier()
{
    int fd_shm;
    char mybuf[256];
    char *shared_mem_ptr;
    FILE *pf;
    sem_t *mutex_sem_1, *mutex_sem_payment, *mutex_sem_total;

    //  mutual exclusion semaphore, mutex_sem
    if ((mutex_sem_1 = sem_open(SEM_MUTEX_1, 0, 0, 0)) == SEM_FAILED)
        error("sem_open");

    if ((mutex_sem_payment = sem_open(SEM_MUTEX_PAYMENT, 0, 0, 0)) == SEM_FAILED)
        error("sem_open");

    if ((mutex_sem_total = sem_open(SEM_MUTEX_TOTAL, 0, 0, 0)) == SEM_FAILED)
        error("sem_open");

    if (!openFile(&pf, FILE_NAME, MODE_READ_TXT))
        error("Error, the file could not be open.\n");

    //  open shared mem
    if ((fd_shm = shm_open(SHARED_MEM_PAYMENTS, O_RDWR, 0)) == -1)
        error("shm_open");

    if ((shared_mem_ptr = mmap(NULL, sizeof(mybuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    char line[256];

    if (sem_post(mutex_sem_1) == -1)
        error("sem_post: mutex_sem_1");

    while (fgets(line, sizeof(line), pf))
    {
        // P (mutex_sem);
        if (sem_wait(mutex_sem_1) == -1)
            error("sem_wait: mutex_sem_1");

        memcpy(shared_mem_ptr, line, sizeof(line));

        printf("Copy to shared Memory : %s", line);

        // Release mutex sem: V (mutex_sem)
        if (sem_post(mutex_sem_payment) == -1)
            error("sem_post: mutex_sem_payment");

        printf("%s \n", strchr(line, '\n'));
    }

    if (sem_post(mutex_sem_total) == -1)
        error("sem_post: mutex_sem_total");

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