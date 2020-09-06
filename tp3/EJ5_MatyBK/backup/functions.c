/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<time.h>

typedef struct {
    char nombre[20];
    char contrasena[20];
    char rol;
    int cod_comision;
} t_usuario;

typedef struct {
    char nombre[20];
    char presente;
} t_asistencia;

/*
    @name: generar_archivo_asistencias
    @description: Crea un archivo de asistencias en una comisión y fecha determinadas, nombre template "Asistencia_[FECHA]_[COMISION].txt"
    @params: *usuario, puntero a una variable de tipo t_usuario
    @returns:
        - 0 Proceso OK
        - 1 Error al procesar
*/
int generar_archivo_asistencias(t_asistencia* lista, int cod_comision);

/*
    @name: split_login_line
    @description: convierte una linea del archivo de usuarios a tipo t_usuario, el cual lo retorna dentro de la referencia *usr
    @params: *linea, puntero al string, *usr puntero a una variable t_usuario
*/
void split_login_line(char * linea, t_usuario * usr);

/*
    @name: validar_login
    @description: Verifica que una combinación de usuario y contraseña sea válida
    @params: *usuario, puntero a una variable de tipo t_usuario
    @returns:
        - 0 el usuario es válido
        - 1 credenciales inválidas
        - 2 usuario no existe
        - 3 Error al leer base de datos
*/

int validar_login(t_usuario *usuario);
/*
    @name: registrar_ingreso
    @description: guarda en un archivo .log un login de usuario
    @params: *usuario, puntero a una variable de tipo t_usuario
    @returns:
        - 0 en caso de que se haya guardado con éxito
        - 1 en caso de no poder guardar el registro
        - 2 en caso de no poder abrir el archivo
*/
int registrar_ingreso(t_usuario *usuario);
/*
    @name: listar_ingresos
    @description: Imprime la lista de ingresos al sistema
    @params: fecha: string en formato yyyy-mm-dd
*/
void listar_ingresos(char * fecha);

void error(const char *msg);

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer);
    n = write(newsockfd, "I got your message", 18);
    if (n < 0)
        error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);

    /*t_usuario user = { "LUCA", "x5sqsa" };


    //Prueba login


    int existe = validar_login(&user);

    printf("%d\n", existe);

    registrar_ingreso(&user);*/

    return 0;
}


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void split_login_line(char * linea, t_usuario * usr){
    strcpy(usr->nombre, strtok(linea,"|"));
    strcpy(usr->contrasena, strtok( NULL, "|" ));
    char * ptr = strtok( NULL, "|" );
    usr->rol = *ptr;
    usr->cod_comision = atoi(strtok( NULL, "\n" ));
}

int validar_login(t_usuario *usuario){

    t_usuario aux;
    char reg[50];
    char * str;

    FILE* pf = fopen("alumnos.txt", "r");
    if(!pf) return 3;

    while(str = fgets(reg, 50, pf)){

        split_login_line(reg, &aux);
        printf("%s %s %c %d\n", aux.nombre, aux.contrasena, aux.rol, aux.cod_comision);
        if(strcmp(aux.nombre, usuario->nombre) == 0) break;

    }

    if(!str) return 2;

    if(strcmp(aux.contrasena, usuario->contrasena) != 0) return 1;

    fclose(pf);

    return 0;
}

/*
    @name: registrar_ingreso
    @description: guarda en un archivo .log un login de usuario
    @params: *usuario, puntero a una variable de tipo t_usuario
    @returns:
        - 0 en caso de que se haya guardado con éxito
        - 1 en caso de no poder guardar el registro
        - 2 en caso de no poder abrir el archivo
*/
int registrar_ingreso(t_usuario *usuario){
    FILE * pf;
    pf = fopen("ingresos.log", "a+");
    if(!pf){
        return 2;
    }

    time_t t;   // not a primitive datatype
    time(&t);
    char * current_time = ctime(&t);

    printf("\nThis program has been writeen at (date and time): %s", current_time);

    fseek(pf, 0, SEEK_END);

    fprintf(pf, "%s - %s - %d", current_time);

    fclose(pf);

    return 0;
}
/*
    @name: listar_ingresos
    @description: Imprime la lista de ingresos al sistema
    @params: fecha: string en formato yyyy-mm-dd
*/
void listar_ingresos(char * fecha);

