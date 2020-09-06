#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define MEMORIA "Memoria"
#define CPU "CPU"
#define AMBOS "Ambos"
#define SEPARADOR "|"

void ayuda();

void handSignal(int signum);

//Registro de procesos ya analizados
struct regProceso
{
    char regPid[15];
    char regMotivo[10];
};
//Array de regProceso
struct regProcesoArray
{
    int numReg;
    struct regProceso *registros;
};

int main(int argc, char **argv)
{

    /*if (argc < 2 || argc > 3)
    {
        printf("\nError en los parametros, ingrese el parametro -help para mas informacion\nEj4 -help o Ej4 -h\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0)
    {
        ayuda();
    }*/

    int limiteCPU = 1;     //atoi(argv[1]);
    int limiteMem = 20000; //atoi(argv[2]);

    struct regProcesoArray registrados;
    registrados.numReg = 0;
    registrados.registros = malloc(1000 * sizeof(*registrados.registros));

    //Creacion de FIFO
    char *fifo = "./fifo";
    mkfifo(fifo, 0777);
    int wFifo, rFifo;

    pid_t control, registro;

    control = fork();

    //Codigo de hijo Control
    if (control == 0)
    {

        FILE *tmpProcess;
        char data[512];
        char *dataString;
        char *pid;
        char *memory;
        char *cpu;
        char *motivo;
        char *nameProcess;

        while (1)
        {
            //Comando que devuelve una lista con el PID, Memoria, CPU y nombre de cada proceso
            tmpProcess = popen("ps -eo pid=,rss=,pcpu=,cmd= | sort -k2 -rn | awk '$2 != 0'", "r");
            if (tmpProcess == NULL)
            {
                printf("Failed");
                exit(EXIT_FAILURE);
            }
            //Iteracion de cada linea
            while (fgets(data, sizeof(data), tmpProcess) != NULL)
            {
                //Se guarda en una variable
                dataString = strdup(data);
                //Se splitea toda la linea, el while es para reconocer cuando hay mas de un espacio entre palabras
                do
                {
                    pid = strsep(&dataString, " ");
                } while (strcmp(pid, "") == 0);
                do
                {
                    memory = strsep(&dataString, " ");
                } while (strcmp(memory, "") == 0);
                do
                {
                    cpu = strsep(&dataString, " ");
                } while (strcmp(cpu, "") == 0);
                do
                {
                    nameProcess = strsep(&dataString, "\n");
                } while (strcmp(nameProcess, "") == 0);

                motivo = "";

                //Verifica si la memoria y el CPU son superados por las del proceso recibidos por argumentos
                if (atoi(memory) >= limiteMem && atof(cpu) >= limiteCPU)
                {
                    motivo = AMBOS;
                }
                else if (atoi(memory) >= limiteMem)
                {
                    motivo = MEMORIA;
                }
                else if (atof(cpu) >= limiteCPU)
                {
                    motivo = CPU;
                }
                //Si supera, ejecuta las siguientes lineas
                if (strcmp(motivo, "") != 0)
                {
                    //Abre el FIFO para escritura
                    wFifo = open(fifo, O_WRONLY);
                    if (wFifo == -1)
                    {
                        perror("Error wFifo\n");
                        return EXIT_FAILURE;
                    }
                    //Arma los caracteres a enviar
                    char *process = malloc(strlen(pid) + strlen(nameProcess) + strlen(motivo) + 3);
                    strcpy(process, pid);
                    strcat(process, SEPARADOR);
                    strcat(process, nameProcess);
                    strcat(process, SEPARADOR);
                    strcat(process, motivo);
                    strcat(process, SEPARADOR);
                    //Envia la cadena
                    write(wFifo, process, strlen(process));
                    close(wFifo);
                    //free(process);
                }
            }
            pclose(tmpProcess);
            sleep(1);
        }
    }
    else
    {
        registro = fork();
        //Codigo de hijo Registro
        if (registro == 0)
        {
            FILE *file;
            time_t t;
            char *pid;
            char *nameProcess;
            char *motivo;
            char *process;

            file = fopen("./archivoProcesos.txt", "r+");
            if (file == NULL)
            {
                printf("No se puede crear el archivo");
                exit(EXIT_FAILURE);
            }
            while (1)
            {
                char strl[10000];
                memset(strl, '\0', 10000);
                //Abre el FIFO para lectura
                rFifo = open(fifo, O_RDONLY);
                if (rFifo == -1)
                {
                    perror("Error wFifo\n");
                    exit(EXIT_FAILURE);
                }
                //Lee el contenido del FIFO
                read(rFifo, strl, 10000);
                close(rFifo);
                int tam = strlen(strl);
                strl[tam] = '\0';
                process = strl;
                //Separa la cadena
                pid = strsep(&process, SEPARADOR);
                nameProcess = strsep(&process, SEPARADOR);
                motivo = strsep(&process, SEPARADOR);
                //Fecha actual
                t = time(NULL);
                struct tm tm = *localtime(&t);
                char currentTime[9];
                strftime(currentTime, 9, "%H:%M:%S", &tm);
                //Verifica si hay procesos ya registrados
                if (registrados.numReg == 0)
                {
                    fprintf(file, "%s %s %s %s %s %s %s\n", pid, SEPARADOR, nameProcess, SEPARADOR, motivo, SEPARADOR, currentTime);
                    fflush(file);
                    registrados.numReg += 1;
                    strcpy(registrados.registros[0].regPid, pid);
                    strcpy(registrados.registros[0].regMotivo, motivo);
                }
                else
                {
                    int i = 0;
                    int max = registrados.numReg;
                    bool found = false;
                    while (i < max && !found)
                    {
                        //Verifica si ya existia el PID
                        if (strcmp(registrados.registros[i].regPid, pid) == 0)
                        {
                            found = true;
                            //Verifica si el motivo fue por Ambos (CPU y Memoria)
                            if (strcmp(registrados.registros[i].regMotivo, AMBOS) != 0)
                            {
                                //Verifica si el motivo anterior era distinto
                                if (strcmp(registrados.registros[i].regMotivo, motivo) != 0)
                                {
                                    fprintf(file, "%s %s %s %s %s %s %s\n", pid, SEPARADOR, nameProcess, SEPARADOR, motivo, SEPARADOR, currentTime);
                                    fflush(file);
                                    registrados.numReg += 1;
                                    strcpy(registrados.registros[i].regPid, pid);
                                    strcpy(registrados.registros[i].regMotivo, AMBOS);
                                }
                            }
                        }
                        i++;
                    }
                    //Si no encontró el proceso, lo agrega
                    if (!found)
                    {
                        fprintf(file, "%s %s %s %s %s %s %s\n", pid, SEPARADOR, nameProcess, SEPARADOR, motivo, SEPARADOR, currentTime);
                        fflush(file);
                        registrados.numReg += 1;
                        strcpy(registrados.registros[i].regPid, pid);
                        strcpy(registrados.registros[i].regMotivo, motivo);
                    }
                }
            }
        }
        else
        {
            // Codigo de Principal

            printf("Procesos Control y Registro en ejecución.\n");
            printf("Para finalizar utilice: kill -SIGUSR1 %d.\n", getpid());
            //Crea la señal
            signal(SIGUSR1, handSignal);
            //Espera la señal
            pause();
        }
    }

    return 0;
}

void ayuda()
{
    printf("\nGRUPO 5 EJERCICIO 4\n\n");
    printf("\nAYUDA PARA EJECUTAR EL PROGRAMA EJ4\n\n");
    printf("NOMBRE\n");
    printf("Ejercicio 2\n\n");
    printf("DESCRIPCION\n");
    printf("\t El programa recibe dos parametros numericos, el primero es el limite de uso del CPU y el segundo el limite de uso de la Memoria. ");
    printf("Funcionará en segundo plano analizando el uso del sistema y guardará en un archivo los procesos que superen esos limites.\n\n");
    printf("EJECUCION\n");
    printf("./Ej4 CPU MEMORIA\n");
    printf("EJEMPLO\n");
    printf("./Ej4 3000 22000\n");
    exit(2);
}

void handSignal(int signum)
{
    if (signum == SIGUSR1)
    {
        unlink("./fifo");

        FILE *childs;
        char command[30];
        char data[10];
        int pid = getpid();
        char *mypid = (char *)malloc(sizeof(int));
        sprintf(mypid, "%d", pid);
        strcpy(command, "pgrep -P ");
        strcat(command, mypid);
        childs = popen(command, "r");
        if (childs == NULL)
        {
            printf("Failed");
            exit(1);
        }
        char killP[20];
        while (fgets(data, sizeof(data), childs) != NULL)
        {
            strcpy(killP, "kill -9 ");
            strcat(killP, data);
            system(killP);
        }
        printf("El proceso %d finalizó correctamente.\n", getpid());
        exit(EXIT_SUCCESS);
    }
}