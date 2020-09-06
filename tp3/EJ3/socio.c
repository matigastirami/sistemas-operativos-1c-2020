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
#define SEM_MUTEX_1 "/sem-mutex-1"
#define SEM_MUTEX_PAYMENT "/sem-mutex-payment"
#define SEM_MUTEX_TOTAL "/sem-mutex-total"
//memory
#define SHARED_MEM_PAYMENTS "/payments-shared-mem"
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
void *threadPayment();
void *threadTotal();
void addPartnerStr(char *data);
void loadPartners();
float calculateAmout(Payment *payment);
void insertFirst(Partner *partner);
void partnersPaid();
//GLOBAL
#define SPORTS_SIZE 4
#define DAYS_SIZE 7
Node *head;
float amount = 0;
static char SPORTS[SPORTS_SIZE][10] = {"Futbol", "Voley", "Basquet", "Natacion"};
static char DAYS[DAYS_SIZE][10] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
static Info table[4] = {
    {0, {0, 2}, 1000},
    {1, {1, 3}, 1200},
    {2, {4}, 1300},
    {4, {5}, 1800}};
//

int main(int argc , char *argv[])
{
	
	if (argc <=1 )
	{// Si el consumidor no tiene parametros, quiere ser ejecutado.
		puts("Iniciando proceso.");
		head == NULL;
		loadPartners();
		partnerProcessConsumer();
		return 0;
	}

	if (strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0)
	{
		printf("----------------------------------------------------------------------------.\n");
		printf("El archivo SOCIO debe estar ejecutando antes de iniciar los PAGOS.\n");
		printf("Debe existir el archivo socios.txt en el directorio actual\n");
		printf("El servidor quedara  la escucha de los pagos e ira recalculando los totales usando memoria compartida\n Ej de uso: ./socios\n");
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
    pthread_t paymentProcess;
    pthread_t totalProcess;

    pthread_create(&paymentProcess, NULL, threadPayment, NULL);
    pthread_create(&totalProcess, NULL, threadTotal, NULL);
    pthread_join(paymentProcess, NULL);
    pthread_join(totalProcess, NULL);
}

void *threadPayment()
{
    int fd_shm;
    sem_t *mutex_sem_1, *mutex_sem_pay;
    char *shared_mem_ptr, *aux;
    char mybuf[256];
    Payment payment;

    //  mutual exclusion semaphore
    if ((mutex_sem_1 = sem_open(SEM_MUTEX_1, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_open");

    if ((mutex_sem_pay = sem_open(SEM_MUTEX_PAYMENT, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_open");

    // Get shared memory
    if ((fd_shm = shm_open(SHARED_MEM_PAYMENTS, O_RDWR | O_CREAT | O_EXCL, 0660)) == -1)
        error("shm_open");

    if (ftruncate(fd_shm, sizeof(shared_mem_ptr)) == -1)
        error("ftruncate");

    if ((shared_mem_ptr = mmap(NULL, sizeof(mybuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("mmap");

    while (1)
    { // forever
        //  P (mutex_sem_pay);
        if (sem_wait(mutex_sem_pay) == -1)
            error("sem_wait: mutex_sem_pay");

        strcpy(mybuf, shared_mem_ptr);

        aux = strrchr(mybuf, ';');
        *(aux + 11) = '\0';

        aux = strrchr(mybuf, ';');
        strcpy(payment.date, aux + 1);
        *aux = '\0';

        sscanf(mybuf, "%ld", &payment.dni);

        amount += calculateAmout(&payment);

        /*  V (mutex_sem_1);  */
        if (sem_post(mutex_sem_1) == -1)
            error("sem_post: buffer_count_sem");
    }
    close(fd_shm);
}

void *threadTotal()
{
    sem_t *mutex_sem_total, *mutex_sem_payed;
    //  mutual exclusion semaphore
    if ((mutex_sem_total = sem_open(SEM_MUTEX_TOTAL, O_CREAT, 0660, 0)) == SEM_FAILED)
        error("sem_open");

    while (1)
    {
        if (sem_wait(mutex_sem_total) == -1)
            error("sem_wait: mutex_sem_total");
        sleep(1);
        printf("Monto total[%.2f]\n", amount);
        partnersPaid();
    }
}

void partnersPaid()
{
    Node *partnerPtr = head;

    puts("Socios que no pagaron : ");

    while (partnerPtr)
    {
        if (!partnerPtr->data.paid)
            printf("[dni: %ld , apellido: %s, nombre: %s]\n", partnerPtr->data.dni, partnerPtr->data.lastName, partnerPtr->data.name);
        partnerPtr = partnerPtr->next;
    }

    puts("\n******************************\n");
}

float calculateAmout(Payment *payment)
{
    int i = 0;
    Node *partnerPtr = head;

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
