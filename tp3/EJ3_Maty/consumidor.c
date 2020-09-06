#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>
//semaphore
#define SEM_PROCESO "/sem-proceso"
#define SEM_ESCRITURA "/sem-escritura"
#define SEM_MUTEX "/sem-mutex"
#define SEM_LECTURA_PAGO "/sem-lecturaPago"
#define SEM_WHILE_PAGO "/sem-whilePago"
#define SEM_LECTURA_ASIST "/sem-lecturaAsist"
#define SEM_WHILE_ASIST "/sem-whileAsist"
//memory
#define SHARED_MEM "/shared-mem"
//file
#define MODE_READ_TXT "rt"
#define FILE_NAME_PARTNER "socios.txt"

typedef struct
{
    char name[20];
    char lastName[20];
    long dni;
    char sport[10];
    char day[10];
    int paid;
} Partner;

typedef struct _Node
{
    Partner data;
    struct _Node *next;
} Node;

typedef struct
{
    long dni;
    char date[11]; //YYYY-MM-DD
} Payment;

typedef struct
{
    long dni;
    char day[10];
} Attendance;

typedef struct
{
    int sport;
    int day[2];
    float value;
} Info;

Partner *create_partner(char *name, char *lastName, long dni, char *sport, char *day);
void error(char *msg);
int openFile(FILE **pf, char *name, char *mode);
void partnerProcessConsumer();
void *threadPago();
void *threadAsist();
void asistenciaNoCorresponde(Attendance *attend);
void addPartnerStr(char *data);
void loadPartners();
float calculateAmount(Payment *payment);
void insertFirst(Partner *partner);
void partnersPaid();
void liberarSemaforos();
//GLOBAL
#define SPORTS_SIZE 4
#define DAYS_SIZE 7
Node *head;
float amount = 0;
static char SPORTS[SPORTS_SIZE][10] = {"Futbol", "Voley", "Basquet", "Natacion"};
static char DAYS[DAYS_SIZE][10] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};

//{ID_DEPORTE, ID_FECHA, PRECIO}
static Info table[4] = {
    {0, {0, 2}, 1000},
    {1, {1, 3}, 1200},
    {2, {4}, 1300},
    {4, {5}, 1800}};
//

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0)
        {
            printf("----------------------------------------------------------------------------.\n");
            printf("El proceso CONSUMIDOR debe estar ejecutando antes de iniciar los procesos PRODUCTORES.\n");
            printf("Debe existir el archivo socios.txt en el directorio actual\n");
            printf("El proceso quedara a la escucha de los pagos e ira recalculando los totales usando memoria compartida\n Ej de uso: ./socios\n");
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

    puts("Esperando productores...\n");
    head == NULL;
    loadPartners();
    partnerProcessConsumer();
    return 0;
}

void loadPartners()
{
    FILE *pf;

    if (!openFile(&pf, FILE_NAME_PARTNER, MODE_READ_TXT))
        error("Error, the file could not be open.\n");

    char line[256];

    while (fgets(line, sizeof(line), pf))
    {
        addPartnerStr(line);
    }

    fclose(pf);
}

void partnerProcessConsumer()
{
    pthread_t pagoProcess;
    pthread_t asistProcess;

    pthread_create(&pagoProcess, NULL, threadPago, NULL);
    pthread_create(&asistProcess, NULL, threadAsist, NULL);
    pthread_join(pagoProcess, NULL);
    pthread_join(asistProcess, NULL);
    liberarSemaforos();
}

void *threadPago()
{
    int fd_shm;
    sem_t *sem_escritura, *sem_lectura, *sem_mutex, *sem_while;
    char *shared_mem_ptr, *aux;
    char mybuf[256];
    Payment payment;

    //  mutual exclusion semaphore
    if ((sem_escritura = sem_open(SEM_ESCRITURA, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_escritura_open");

    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_mutex_open");

    if ((sem_lectura = sem_open(SEM_LECTURA_PAGO, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_lectura_pago_open");

    if ((sem_while = sem_open(SEM_WHILE_PAGO, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_while_pago_open");

    // Get shared memory
    if ((fd_shm = shm_open(SHARED_MEM, O_RDWR | O_CREAT | O_EXCL, 0660)) == -1)
    {
        error("shm_pagos_open");
    }

    if (ftruncate(fd_shm, sizeof(shared_mem_ptr)) == -1)
        error("ftruncate");

    if ((shared_mem_ptr = mmap(NULL, sizeof(mybuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    int val;
    sem_getvalue(sem_while, &val);

    while (val == 1)
    {
        //  P (sem_lectura);
        if (sem_wait(sem_lectura) == -1)
            error("sem_wait: sem_lectura");

        if (sem_wait(sem_mutex) == -1)
            error("sem_wait: sem_mutex");

        strcpy(mybuf, shared_mem_ptr);

        if (sem_post(sem_mutex) == -1)
            error("sem_post: sem_mutex");

        if (sem_post(sem_escritura) == -1)
            error("sem_post: sem_escritura");

        aux = strrchr(mybuf, ';');
        *(aux + 11) = '\0';

        //aux = strrchr(mybuf, ';');

        strcpy(payment.date, aux + 1);
        *aux = '\0';

        sscanf(mybuf, "%ld", &payment.dni);

        amount += calculateAmount(&payment);
        sleep(0.1);

        sem_getvalue(sem_while, &val);
    }

    if (sem_post(sem_while) == -1)
        error("sem_wait: sem_while");
    close(fd_shm);

    printf("Monto total[%.2f]\n", amount);
    partnersPaid();

    sem_t *sem_proceso;
    if ((sem_proceso = sem_open(SEM_PROCESO, 0, 0660, 1)) == SEM_FAILED)
        error("sem_proceso_open");
}

void *threadAsist()
{
    int fd_shm;
    sem_t *sem_escritura, *sem_lectura, *sem_mutex, *sem_while;
    char *shared_mem_ptr, *aux;
    char mybuf[256];
    Attendance attend;

    //  mutual exclusion semaphore
    if ((sem_escritura = sem_open(SEM_ESCRITURA, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_escritura_open");

    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_mutex_open");

    if ((sem_lectura = sem_open(SEM_LECTURA_ASIST, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_lectura_pago_open");

    if ((sem_while = sem_open(SEM_WHILE_ASIST, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_while_pago_open");

    // Get shared memory
    if ((fd_shm = shm_open(SHARED_MEM, O_RDWR | O_CREAT | O_EXCL, 0660)) == -1)
    {
        error("shm_asistencia_open");
    }

    if (ftruncate(fd_shm, sizeof(shared_mem_ptr)) == -1)
        error("ftruncate");

    if ((shared_mem_ptr = mmap(NULL, sizeof(mybuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    int val;
    sem_getvalue(sem_while, &val);

    while (val == 1)
    {
        printf("esperando...");
        //  P (sem_lectura);
        if (sem_wait(sem_lectura) == -1)
            error("sem_wait: sem_lectura");

        if (sem_wait(sem_mutex) == -1)
            error("sem_wait: sem_mutex");

        strcpy(mybuf, shared_mem_ptr);

        if (sem_post(sem_mutex) == -1)
            error("sem_post: sem_mutex");

        if (sem_post(sem_escritura) == -1)
            error("sem_post: sem_escritura");

        aux = strrchr(mybuf, ';');
        *(aux + 10) = '\0';

        //aux = strrchr(mybuf, ';');

        strcpy(attend.day, aux + 1);
        *aux = '\0';

        sscanf(mybuf, "%ld", &attend.dni);

        asistenciaNoCorresponde(&attend);

        sleep(0.1);
        sem_getvalue(sem_while, &val);
    }

    if (sem_post(sem_while) == -1)
        error("sem_wait: sem_while");
    close(fd_shm);

    sem_t *sem_proceso;
    if ((sem_proceso = sem_open(SEM_PROCESO, 0, 0660, 1)) == SEM_FAILED)
        error("sem_proceso_open");
}

void asistenciaNoCorresponde(Attendance *attend)
{

    Node *partnerPtr = head;
    //Busca el DNI que recuperó del archivo pagos y corrobora que esté en el archivo socios.txt
    //Si existe opera y sino retorna 0.
    while (partnerPtr)
    {
        if (partnerPtr->data.dni != attend->dni)
        {
            partnerPtr = partnerPtr->next;
        }
        else
        {
            //Busco el deporte del socio
            for (int i; i < SPORTS_SIZE; i++)
            {
                if (strcmp(SPORTS[i], partnerPtr->data.sport) == 0)
                {
                    int cantDias = (int)sizeof(table[i].day) / sizeof(table[i].day[0]);
                    for (int j; j < cantDias; j++)
                    {
                        if (strcmp(DAYS[table[i].day[j]], attend->day) != 0)
                        {
                            printf("\t Socio: %ld\n", attend->dni);
                            return;
                        }
                    }
                }
            }
        }
    }
    if (partnerPtr == NULL)
        printf("No existe el DNI: %ld\n", attend->dni);
}

void partnersPaid()
{
    Node *partnerPtr = head;

    puts("Socios que no pagaron: ");

    while (partnerPtr)
    {
        if (!partnerPtr->data.paid)
            printf("[dni: %ld , apellido: %s, nombre: %s]\n", partnerPtr->data.dni, partnerPtr->data.lastName, partnerPtr->data.name);
        partnerPtr = partnerPtr->next;
    }

    puts("\n******************************\n");
}

float calculateAmount(Payment *payment)
{
    int i = 0;
    Node *partnerPtr = head;
    //Busca el DNI que recuperó del archivo pagos y corrobora que esté en el archivo socios.txt
    //Si existe opera y sino retorna 0.
    while (partnerPtr)
    {
        if (partnerPtr->data.dni == payment->dni)
            break;
        partnerPtr = partnerPtr->next;
    }

    if (partnerPtr == NULL)
        return 0L;

    //payed
    partnerPtr->data.paid = 1;

    for (i = 0; i < SPORTS_SIZE; i++)
    {
        if (strcmp(SPORTS[i], partnerPtr->data.sport) == 0)
        {
            //Si el pago se realiza del 1 al 10 se realiza un descuento del 10%.
            char *day = strrchr(payment->date, '-');
            int numberDay = atoi(day + 1);
            return (numberDay >= 1 && numberDay <= 10) ? table[i].value * 0.9 : table[i].value;
        }
    }

    return 0;
}

void addPartnerStr(char *data)
{
    Partner partner;
    char *aux;

    aux = strrchr(data, '\n');

    if (!aux)
        return;

    *aux = '\0';

    aux = strrchr(data, ';');
    strcpy(partner.day, aux + 1);
    *aux = '\0';

    aux = strrchr(data, ';');
    strcpy(partner.sport, aux + 1);
    *aux = '\0';

    aux = strrchr(data, ';');
    sscanf(aux + 1, "%ld", &partner.dni);
    *aux = '\0';

    aux = strrchr(data, ';');
    strcpy(partner.lastName, aux + 1);
    *aux = '\0';

    strcpy(partner.name, data);

    insertFirst(&partner);
}

void insertFirst(Partner *partner)
{
    //create a link
    Node *link = (Node *)malloc(sizeof(Node));

    strcpy(link->data.name, partner->name);
    strcpy(link->data.lastName, partner->lastName);
    link->data.dni = partner->dni;
    link->data.paid = 0;
    strcpy(link->data.sport, partner->sport);
    strcpy(link->data.day, partner->day);

    //point it to old first node
    link->next = head;

    //point first to new first node
    head = link;
}

void liberarSemaforos()
{
    sem_t *sem_escritura, *sem_lectura, *sem_mutex, *sem_while;

    if ((sem_escritura = sem_open(SEM_ESCRITURA, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_open");

    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0660, 1)) == SEM_FAILED)
        error("sem_open");

    if ((sem_lectura = sem_open(SEM_LECTURA_PAGO, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_open");

    if ((sem_while = sem_open(SEM_WHILE_PAGO, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_open");

    if(!sem_close(sem_mutex) && !sem_close(sem_while) && !sem_close(sem_escritura) && !sem_close(sem_lectura))
    {
        puts("Los semaforos se cerraron correctamente.");
    }else
    {
        puts("Ocurrió un error al cerrar los semaforos.");
    }
    
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
