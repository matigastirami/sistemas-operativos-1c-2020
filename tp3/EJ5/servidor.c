/*
Sistemas Operativos
	====================
    Trabajo Práctico N°3
    Ejercicio 5  
    ====================
	 Grupo 5 Integrantes:
		MIRANDA SERGIO JAVIER - 35.634.266
		CARRIL ARANDA JOSE - 37.120.410
		LUNA SANCHEZ MATIAS LEONEL - 37.141.163
		RAMIREZ MATIAS GASTON - 39.505.838
		MASINI JOAN EZEQUIEL - 39.562.503
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <dirent.h> 

#define BUFFER_RESP 500
#define REG_SIZE 50
#define MAX_CMD 5000
#define MAX_RES 20000

void *connection_handler(void *);
void close_socket(int signum);
void *turn_off_svr ();
int registrar_conexion(int mod,int socket);
int registrar_mensaje(int mod, int socket, char* msje);
void replace_newline(char* word);
int has_user_in_file(FILE* fich, char* name);
struct t_present_counter file_searcher_present_counter(char* username);
int starts_with(const char *totalName, const char *startWith);
int was_student_present(char* username,char* date_class);
int accumulative_students_lines(char* resultado, char* date_class, char* code_class);
int make_classes_assist(char* resultado,char* date_class,char* code_class, char* student_to_write,char* student_state_to_write);
int get_lines_of_file(FILE* fp);
int socket_desc;
pid_t p;

struct t_present_counter
{
	int presents;
	int totalFiles;
};

/* Signal Handler for SIGINT */
int main(int argc , char *argv[])
{
    int client_sock , c , *new_sock, puerto_servidor;
    struct sockaddr_in server , client;
    
	if (argc <=1 )
	{
		printf("Error en los parametros ingresados\n");
		printf("Use como parametro [-help] o [-h] para obtener la ayuda.\n");
		return 0;
	}

	if (strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0)
	{
		printf("----------------------------------------------------------------------------.\n");
		printf("El Servidor debe estar ejecutando antes de iniciar los clientes.\n");
		printf("Para iniciar el servidor, simplemente ejecute ./servidor [PUERTO].\n");
		printf("----------------------------------------------------------------------------.\n");
		return 0;
	}
	
	if( !(access( "Usuario.txt", F_OK ) != -1) ) 
	{
		printf("No se encontro el archivo [Usuario.txt].\n");
		printf("Agreguelo en el directorio de ejecucion para poder continuar.\n");
		return 1;
	}
	
	puerto_servidor = atoi(argv[1]);
	
	if (puerto_servidor<=1 && puerto_servidor>=65000)
	{
		printf("Error en el puerto ingresado.\n");
		printf("El puerto debe ser entre 1 y 65000.\n");
		return 0;
	}

    pthread_t idHilo;
    signal(SIGTERM,close_socket);

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No se pudo crear el socket");
    }
	puts("Socket creado");

    int new_size = MAX_RES ;
    setsockopt(socket_desc, SOL_SOCKET, SO_SNDBUF , &new_size, sizeof(new_size));

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( puerto_servidor );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Bind fallido");
        return 1;
    }
    puts("Bind exitoso");

    //Listen
    listen(socket_desc , 3);

    //Me preparo para aceptar conexiones
    
    p = fork(); 
		  
	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} 
	else if (p > 0) 
	{ 	// Parent process 
		printf("\nServidor demonio creado.\n");
		printf("Para CERRAR EL SERVIDOR Y LIBERAR LA MEMORIA utilice [kill -sigterm %d].\n",p);
	} 
	else
	{	// child process 
		puts("Todo listo, esperando conexiones...");
		c = sizeof(struct sockaddr_in);
		// c = sizeof(struct sockaddr_in);
		while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
		{
			puts("\nConexion aceptada");

			pthread_t sniffer_thread;
			new_sock = (int *) malloc (sizeof (int));

			*new_sock = client_sock;

			if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
			{
				perror("No se puedo iniciar el thread");
				return 1;
			}
			printf("Cliente atendido: %d\n", client_sock);
		}

		if (client_sock < 0)
		{
			perror("Fallo en aceptar conexion");
			return 1;
		}
		else if(registrar_conexion(1,client_sock)==2)
			puts("Hubo un error al registrar la conexion.");

		return 0;
	}
}


// This will handle connection for each client
void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc,
		n;
	
	int opt = 0;
    
	char client_message[MAX_CMD],
		 resultado[MAX_RES],
		 quit_sentence[]="QUIT\0";

	do{
		puts("\nSERVIDOR ESCUCHANDO...");
		
		fflush(stdin);
		memset(resultado,'\0',MAX_RES);
		memset(client_message,'\0',MAX_CMD);
		opt = 0;
		
		recv(sock,client_message,MAX_CMD,0);
		puts("SERVIDOR RECIBE MENSAJE DE CLIENTE");
		///printf("SERVIDOR RECIBE:%s\n",client_message);
		registrar_mensaje(0,sock,client_message);
		
		if(strcmp(client_message,quit_sentence)==0)
		{// Si se recibio QUIT del cliente.
			registrar_conexion(0,sock);
			printf("Cerrando conexion de socket %d...\n",sock);
			close(sock);
			return 0;
		}
	
		char del[] = "|";
		char *zeroParam_client = strtok(client_message, del),
		*firstParam_client,
		*secondParam_client,
		*thirdParam_client,
		*fourthParam_client_op,
		*fifthParam_client_op;
		
		// Desglozo mensaje recibido en parmetros
		if(zeroParam_client != NULL)
		{
			firstParam_client = strtok(NULL, del);
			
			if(firstParam_client != NULL)
				secondParam_client = strtok(NULL, del);
			
			if(secondParam_client != NULL)
				thirdParam_client = strtok(NULL, del);
			
			if(thirdParam_client != NULL)
				fourthParam_client_op = strtok(NULL, del);
			
			if(fourthParam_client_op != NULL)
				fifthParam_client_op = strtok(NULL, del);
		}
		
		// El primer parametro siempre es el tipo de usuario
		int userRequest = atoi(zeroParam_client),
			notLoggedUser = (userRequest == 0)?1:0,
			isTeacherUser = (userRequest == 1)?1:0,
			isStudentUser = (userRequest == 2)?1:0;
		
		if(notLoggedUser)
		{// Si no esta logeado, debo verificar existencia y responder al logeo.
			//Info a recibir [LOG_OFF]|[USER]|[PASSWORD]

			puts("SERVIDOR LOGEA USUARIO");
			//printf("Ingresado Campo: %s\n", firstParam_client);
			//printf("Ingresado Pass: %s\n", secondParam_client);

			FILE *fich;
			char linea[MAX_RES],
				 delimitador[] = "|";
				 
			int f_busqueda=0,
				lineNumber=0;

			fich = fopen("Usuario.txt", "r");
			
			if(fich == NULL)
			{// Si no pudo abrir el archivo.
				puts("\n\nNo se pudo abrir el archivo Usuarios.txt");
				puts("Verifique la existencia del mismo, o los permisos para poder utilizarlo e intente nuevamente.");
				puts("Cerrando servidor y liberando recursos...\n");
				kill(getpid(),SIGUSR1);
			}
			
			if(firstParam_client == NULL || secondParam_client == NULL)
			{
				puts("\n\nFaltan campos para poder logerse.\nIntente nuevamete.\n");
				continue;
			}
			
			//Lee línea a línea y escribe en pantalla hasta el fin de fichero
			//fscanf (f, "%s", cadena);

			
			while(fgets(linea, MAX_RES, (FILE*) fich)) 
			{
				if(lineNumber!=0)
				{
					
					//int n = strlen(linea);
					//linea[n-1] = 0;
					
					char *username_reg = strtok(linea, delimitador),
						 *password_reg,
						 *role_reg,
						 *code_reg;
						 
					if(username_reg != NULL)
					{
						password_reg = strtok(NULL, delimitador);
						role_reg = strtok(NULL, delimitador);
						code_reg = strtok(NULL, delimitador);
					}

					///printf("Registro leido: %s | %s | %s | %s \n",username_reg,password_reg,role_reg,code_reg);

					if ((strcmp(firstParam_client,username_reg)==0)&&(strcmp(secondParam_client,password_reg)==0))
					{
						f_busqueda=1;
						strcpy(resultado, "1");
						if(strcmp(role_reg,"D")==0)
							strcat(resultado, "|1|");
						else
							strcat(resultado, "|2|");
						strcat(resultado, code_reg);
						strcat(resultado, "\0");
						//Esto devuelve [LOGEADO_OK]|[TIPO_USER]|[COMISION]
					}
				}
				
				if(f_busqueda==1)
					break;
				
				lineNumber++;
			}

			if (f_busqueda==0)
			{
				strcpy(resultado, "0|0");
			}

			fclose(fich);
			///puts("Cerrando archivo");
			
		}
		else
		{// Si esta logeado.
			puts("SERVIDOR IDENTIFICA USUARIO");
			opt = atoi(firstParam_client);
			replace_newline(secondParam_client); // Fijar si no dejar esto en alumno
			if(isTeacherUser)
			{// Es DOCENTE
				puts("SERVIDOR RECIBE CONEXION DE DOCENTE");
				// RECIBE: [Logeado-DOCENTE]|[OPCION-SELEC]|[FECHA]|[COMISION]
				// Ambas opciones posibles requieren solo de estos parametros, AL MENOS LA PRIMERA VEZ..
				if(opt==1)
				{// Cliente DOCENTE quiere consultar las asistencias de una fecha.
					// Buscar entre los archivos del directorio la existencia de Asistencia_COMISION_FECHA.txt
					// Si no existe, retorno 0.
					// Si existe, devuelvo solo los del tipo alumno para la comision del docente para una fecha.
					if(!accumulative_students_lines(resultado,secondParam_client,thirdParam_client))
					{
						puts("Ocurrio un error al querer obtener la lista de asistencia.");
						memset(resultado,'\0',MAX_RES);
						strcpy(resultado, "-1");
					}
						
					strcat(resultado, "\0");
					//Esto devuelve [LISTADO_ALUMNOS_SEPARADOS_XLINEA] ó [VALOR_INVALIDO=-1]
				}
				else
				{// Cliente DOCENTE quiere cargar asistencias para una fecha
					// Se deben obtener los solo los del tipo alumno desde Usuarios.txt para la comision del docente para una fecha.
					// Se deberian devolver separados por \n solo los nombres.
					// Lo ideal es ver como manejamos esto, por ahora devolver el listado.
					///////////////////////////////////////////////////////////////////////
					// De alguna manera el cliente debe enviar una respuesta para cada item del archivo de Usuarios.txt
					// Se debe crear o ir escribiendo en archivo nuevo Asistencia_COMISION_FECHA.txt
					// Una vez finalizada la lista de alumnos, cerrar archivo.
					// PROCESO DE CARGA --------------------------
					// Si esta en medio de una carga de asistencia, va a recibir tambien el ultimo alumno cargado y su [P]o[A] - Presente o Ausente.
					// RECIBE: [Logeado-DOCENTE]|[OPCION-SELEC]|[FECHA]|[COMISION]|[ALUMNO]|[ASISTENCIA]
					// Escribir en archivo lo recibido para el archivo Asistencia_COMISION_FECHA.txt, y escribir [ALUMNO]|[ASISTENCIA]
					// Cerrar archivo Asistencia. 
					// Abrir Usuarios.txt buscar el ALUMNO recibido, leer el siguiente y retornarlo.
					// Si se llego a fin de archivo retornar 0 para avisar que finalizo.

					int responseMethod;

					
					memset(resultado,'\0',MAX_RES);
					///puts("Se solicito cargar asistencia.");
					responseMethod = make_classes_assist(resultado,secondParam_client, thirdParam_client, fourthParam_client_op, fifthParam_client_op);
					///printf("Sale con esto %s\n",resultado);
					
					if(responseMethod != 1)
					{
						memset(resultado,'\0',MAX_RES);
						sprintf(resultado, "%d",responseMethod);
					}
					
					
					strcat(resultado, "\0");
					///printf("Va a devolver %s con cero\n",resultado);
					//Esto devuelve [LISTADO_ALUMNOS_SEPARADOS_XLINEA] ó [VALOR_INVALIDO=-1]
				}
			}
			else
			{// Es ALUMNO
				puts("SERVIDOR RECIBE CONEXION DE ALUMNO");
				// OP1 RECIBE: [Logeado-ALUMNO]|[OPCION-SELEC]|[USUARIO]|[FECHA]
				// OP2 RECIBE: [Logeado-ALUMNO]|[OPCION-SELEC]|[USUARIO]
				
				if(opt==1)
				{// Cliente ALUMNO quiere consultar si esta presente en una fecha determinada.
					// Buscar entre los archivos del directorio la existencia de Asistencia_COMISION_FECHA.txt
					// Si no existe, retorno 0.
					// Si existe, busco alumno. Si tiene presente retorno 1, si tiene ausente retorno 0.
					
					memset(resultado,'\0',MAX_RES);
					char response[BUFFER_RESP];
					memset(response,'\0',BUFFER_RESP);
					
					int foundIt = was_student_present(secondParam_client,thirdParam_client);
					sprintf(response, "%d", foundIt);
					strcpy(resultado, response);
					strcat(resultado, "\0");
					//Esto devuelve [ASISTIO?=1o0] ó [VALOR_INVALIDO=-1]
				}
				else
				{// Cliente ALUMNO quiere su porcentaje de asistencias
					// Traer TODOS los archivos de la comision del alumno Asistencia_COMISION_FECHA.txt
					// Si no existe ninguno, retorno 0.
					// Si existe al menos 1, voy contando los Presentes. Tambien cuento el total de archivos de esa COMISION.
					// Total de archivos - Presente = Ausente.
					// Total de archivos -- 100%  ->   Retornar (Presente*100)/TotalArchivos
					char response_percentage[BUFFER_RESP];
					memset(response_percentage,'\0',BUFFER_RESP);
					
					struct t_present_counter newObject = file_searcher_present_counter(secondParam_client);
					///printf("Total de archivos-> %d\n", newObject.totalFiles);
					///printf("Total de presentes-> %d\n", newObject.presents); 
					
					sprintf(response_percentage, "%d", ((newObject.presents*100)/newObject.totalFiles));
					strcpy(resultado, response_percentage);
					strcat(resultado, "\0");
					
					//Esto devuelve [PORCENTAJE_ASISTENCIAS=0a100] ó [VALOR_INVALIDO=-1]
				}
			}
		}
		
		int nmu = strlen(client_message);
		client_message[nmu-1]= '\0';
		
		puts("SERVIDOR EMITE RESPUESTA");
		registrar_mensaje(1,sock,resultado);
		send(sock,resultado,MAX_RES,0);
		
		memset(resultado,'\0',MAX_RES);
		
	}while(1);

}

void close_socket(int signum)
{
    close(socket_desc);
	printf("Se liberaron los recursos utilizados.\n");
	kill(p,SIGKILL);
	exit(1);
}

void * turn_off_svr ()
{// Funcion para matar el server.
	kill(getpid(),SIGUSR1);
}

int registrar_conexion(int mod,int socket)
{// Logea conexion conexion realizada o finalizada.
    FILE * pf;
    pf = fopen("Log_Servidor.log", "a+");
    if(!pf)
        return 2;

    time_t t;   // not a primitive datatype
    time(&t);
    char * current_time = ctime(&t);

    replace_newline(current_time);

    fseek(pf, 0, SEEK_END);
	if(mod==1)
		fprintf(pf, "[Fecha %s] - [Socket %d] - [Nueva conexion]\n", current_time, socket);
	else
		fprintf(pf, "[Fecha %s] - [Socket %d] - [Cierra conexion]\n", current_time, socket);
	
    fclose(pf);

    return 0;
}

int registrar_mensaje(int mod, int socket, char* msje)
{// Logea mensaje enviado 1 o recibido 0.
    FILE * pf;
    pf = fopen("Log_Servidor.log", "a+");
    if(!pf)
	{
		puts("Hubo un error al querer crear o abrir el archivo de Log_Servidor.log\nVerifique permisos y vuelva a ejecutar.");
		return 2;
	}
        
    time_t t; 
    time(&t);
	
    char * current_time = ctime(&t);
	replace_newline(current_time);
    replace_newline(msje);

    fseek(pf, 0, SEEK_END);
	
	if(mod == 1)
		fprintf(pf, "[Fecha %s] - [Socket %d] - [Msje enviado: %s]\n",current_time ,socket ,msje);
    else
		fprintf(pf, "[Fecha %s] - [Socket %d] - [Msje recibido: %s]\n",current_time ,socket, msje);

    fclose(pf);

    return 0;
}

void replace_newline(char* word)
{// Remplaza el ultimo caracter suponiendo que es el salto de linea por fin de linea.
	char auxi[MAX_CMD];
	strcpy(auxi, word);
	int nm = strlen(auxi);
	if(word[nm-1]=='\n')
		word[nm-1]= '\0';
	return;
}

struct t_present_counter file_searcher_present_counter(char* username)
{// Busco los archivos, busco las veces que aparece el usuario y cuento su presente.
	
    struct dirent *de; // Puntero directorio 
	struct t_present_counter finalElement = { 0,0 };
	
    // Retorna puntero a directorio actual.
    DIR *dr = opendir("."); 
  
    if (dr == NULL)
    { // No se puedo abrir el directorio.
        puts("No se pudo abrir el directorio actual.\nVerifique permisos e intente nuevamente el proceso.\n" ); 
		finalElement.totalFiles = -1;
		return finalElement; 
    } 
  
    while ((de = readdir(dr)) != NULL)
	{
		FILE * pf;
		///printf("Archivo leido-> %s\n", de->d_name); 
		if(starts_with(de->d_name, "Asistencia_"))
		{
			finalElement.totalFiles++;
			
			pf = fopen(de->d_name, "r");
			if(pf != NULL)
			{//Abrir archivo, buscar 1 x 1 si encuentro el alumno e incrementar alumno.
				if(has_user_in_file(pf,username)==1)
					finalElement.presents++;
				fclose(pf);
			}
			else
			{
				puts("Error al abrir archivo.");
				continue;
			}	
		}
	}
    closedir(dr); 
    return finalElement; 
}

int has_user_in_file(FILE* fich, char* name)
{// Busco la existencia del usuario en el archivo de asistencia, y lo cuento solo si esta presente.
	char linea[MAX_RES],
		 delimitador[] = "|";
				 
	int f_busqueda=0,
		lineNumber=0;
	//printf("Usuario recibido: %s\n",name);
	
	while(fgets(linea, MAX_RES, (FILE*) fich)) 
	{
		if(lineNumber!=0)
		{
			char *username_reg = strtok(linea, delimitador),
				 *asist_reg;
				 
			if(username_reg != NULL)
				asist_reg = strtok(NULL, delimitador);

			//printf("Registro leido: %s | %s\n",username_reg,asist_reg);
			//printf("Cantidad 1: %d, y la 2 tiene %d\n",strlen(username_reg),strlen(name));
			
			//printf("Igualdad 1: %d\n",(strcmp(name,username_reg)==0));
			//printf("Igualdad 2: %d\n",(strstr(asist_reg, "P") != NULL));
			if ((strcmp(name,username_reg)==0)&&(strstr(asist_reg, "P") != NULL))
				return 1;
			
		}
		lineNumber++;
	}
	return 0;
}

int starts_with(const char *totalName, const char *startWith)
{// Funcion auxiliar para saber si un string comienza con un substring.
   if(strncmp(totalName, startWith, strlen(startWith)) == 0) return 1;
   return 0;
}

int was_student_present(char* username,char* date_class)
{// Busca un estudiante dentro de un archivo de asistencia dependiendo de la fecha ingresada, y devuelve 1 o 0 dependiendo de la presencia.
	struct dirent *de;
	int response = -1;
	char complete_name[BUFFER_RESP];
	memset(complete_name,'\0',BUFFER_RESP);
	
	sprintf(complete_name, "Asistencia_%s_",date_class);
	///printf("\nArchivo a buscar> %s\n",complete_name);
    DIR *dr = opendir("."); 
  
    if (dr == NULL)
    { // No se puedo abrir el directorio.
        puts("No se pudo abrir el directorio actual.\nVerifique permisos e intente nuevamente el proceso.\n" ); ;
		return 0; 
    } 
  
    while ((de = readdir(dr)) != NULL)
	{
		FILE * pf;
		
		///printf("Archivo leido-> %s\n", de->d_name); 
		if(starts_with(de->d_name, complete_name))
		{
			pf = fopen(de->d_name, "r");
			if(pf != NULL)
			{//Abrir archivo, buscar 1 x 1 si encuentro el alumno devuelvo 1.
				response = has_user_in_file(pf,username);
				fclose(pf);
			}
			else
			{
				puts("Error al abrir archivo.");
				continue;
			}	
		}
	}
    closedir(dr); 
    return response; 
}

int accumulative_students_lines(char* resultado, char* date_class, char* code_class)
{// Devuelve en resultado las lineas acumuladas del archivo de asistencia que machee con la fecha y la comision de la clase.
	// Busca formato: Asistencia_[FECHA]_[COMISION].txt
	struct dirent *de;
	int f_reg=0;
	char complete_name[BUFFER_RESP];
	memset(complete_name,'\0',BUFFER_RESP);
	
	sprintf(complete_name, "Asistencia_%s_%s.txt",date_class,code_class);
	///printf("\nArchivo a buscar> %s\n",complete_name);
    DIR *dr = opendir("."); 
  
    if (dr == NULL)
    { // No se puedo abrir el directorio.
        puts("No se pudo abrir el directorio actual.\nVerifique permisos e intente nuevamente el proceso.\n" ); ;
		return 0; 
    } 
  
	while ((de = readdir(dr)) != NULL)
	{
		FILE * pf;
		///printf("Archivo leido-> %s\n", de->d_name); 
		if(starts_with(de->d_name, complete_name))
		{// Si encontre el directorio.
			pf = fopen(de->d_name, "r");
			if(pf != NULL)
			{//Abrir archivo, buscar 1 x 1 si encuentro el alumno devuelvo 1.
				
				char linea[MAX_RES],
					 delimitador[] = "|";
							 
				int lineNumber=0;

				while(fgets(linea, MAX_RES, (FILE*) pf)) 
				{
					///printf("Linea leida: %s\n",linea);
					if(lineNumber!=0)
					{
						if(lineNumber==1)
						{
							f_reg++;
							strcpy(resultado, linea);
						}
						else
							strcat(resultado, linea);
					}
					lineNumber++;
				}
				fclose(pf);
			}
			else
			{
				puts("Error al abrir archivo.");
				continue;
			}	
			break;
		}
	}
    closedir(dr); 
    return f_reg; 
}

int make_classes_assist(char* resultado, char* date_class, char* code_class, char* student_to_write, char* student_state_to_write)
{// Funcion para generar el archivo de asistencia
	// Para comenzar la carga, la 1era vez recibe [Logeado-DOCENTE]|[OPCION-SELEC]|[FECHA]|[COMISION]
	// Si no es la 1era vez, recibe [Logeado-DOCENTE]|[OPCION-SELEC]|[FECHA]|[COMISION]|[ALUMNO]|[ASISTENCIA]
	FILE* fichUsers, 
		*fichStudents;

	char complete_name_file[REG_SIZE],
		 reg_write[REG_SIZE];
	
	int passedLimit = -1; // Si passed limit es -1, ni se cargo el 1ero. Si es 0, en esta vuelta se cargo el 1er reg. Si es 1, viene cargando.
	
	// Primero intentamos abrir el archivo de usuarios p/ lectura. Si hay error, abortamos todo.
	fichUsers = fopen("Usuario.txt", "r");
	if(fichUsers == NULL)
	{// Si no pudo abrir el archivo.
		puts("\n\nNo se pudo abrir el archivo Usuarios.txt");
		puts("Verifique la existencia del mismo, o los permisos para poder utilizarlo e intente nuevamente.");
		return -2;
	}
	
	// Vaciamos el buffer x las dudas.
	memset(complete_name_file,'\0',BUFFER_RESP);
	// Armamos el nombre del archivo a partir de la fecha y la comision.
	// Este archivo vamos a intentar abrirlo y sino a crearlo.
	sprintf(complete_name_file, "Asistencia_%s_%s.txt",date_class,code_class);
	
	fichStudents = fopen(complete_name_file, "a+");
	if(fichStudents == NULL)
	{// Si no pudo abrir el archivo.
		puts("\n\nNo se pudo abrir el archivo de asistencia.");
		return -2;
	}
	
	if(fichStudents)
	{
		fseek(fichStudents, 0L, SEEK_END);
		if(ftell(fichStudents)==0)
		{
			memset(reg_write,'\0',REG_SIZE);
			strcpy(reg_write,"NOMBRE|PRESENTE\n");
			int number = strlen(reg_write);
			fwrite(&reg_write, sizeof(char)*number, 1, fichStudents);
			memset(reg_write,'\0',REG_SIZE);
			rewind(fichStudents);
			passedLimit = 0;
		}
	}
	
	// Aseguramos los punteros al inicio de ambos archivos.
	fseek(fichUsers, 0L, SEEK_SET);
	fseek(fichStudents, 0L, SEEK_SET);
	
	if(student_to_write!=NULL || student_state_to_write != NULL)
	{// Asumo que NO es la 1era vez que viene la solicitud.
		// Por ende, viene con la respuesta del profesor, para ser escrita en el archivo.
		// Quita el caracter de salto de linea de ambos campos.
		replace_newline(student_to_write);
		replace_newline(student_state_to_write);
		
		//Preparo el buffer que se va a escribir con el nombre del estudiante.
		strcpy(reg_write,student_to_write);
		strcat(reg_write,"|");
		strcat(reg_write,student_state_to_write);
		strcat(reg_write,"\n\0.");
		
		// Insertar al final, el alumno cargado.
		fseek(fichStudents, 0, SEEK_END);
		int number = strlen(reg_write);
		fwrite(&reg_write, sizeof(char)*number, 1, fichStudents);
		fseek(fichStudents, 0, SEEK_SET);
	}
	
	char linea[MAX_RES],
		 delimitador[] = "|";
	char lineaAs[MAX_RES],
		 delimitadorAs[] = "|";	 
	int lineNumber=0, change=0;


	while(fgets(linea, MAX_RES, (FILE*) fichUsers)) 
	{
		///printf("Linea leida: %s\n",linea);
		if(lineNumber!=0)
		{
			char *username_reg = strtok(linea, delimitador),
				 *password_reg,
				 *role_reg,
				 *code_reg;
			
			// Si existe un usuario delimitado y leido.
			if(username_reg != NULL)
			{
				password_reg = strtok(NULL, delimitador);
				role_reg = strtok(NULL, delimitador);
				code_reg = strtok(NULL, delimitador);
				
				///printf("Registro leido: %s | %s | %s | %s \n", username_reg, password_reg, role_reg, code_reg);
				
				if((starts_with(role_reg,"A")==1) && (starts_with(code_reg,code_class)))
				{// Si el que estoy leyendo es un alumno de la misma comision que el docente...
					///puts("Encontro un alumno.");
					if(student_to_write == NULL)
					{
						///puts("Devuelve alumno para que carguen. Va a ser el 1ero.");
						change++;
						//Devuelvo el 1er usuario de la lista de usuarios para que le pongan presente.
						strcpy(resultado,username_reg);
						break;
					}
					else if (passedLimit>-1)
					{
						memset(resultado,'\0',BUFFER_RESP);
						//El siguiente de donde estoy parado lo devuelvo.
						///puts("Tengo al fichero de usuarios apuntando al mismo que acabo de escribir.");
						///puts("Devuelvo el siguiente.");
						change++;
						strcpy(resultado,strtok(linea, delimitador));
						///printf("Devuelvo el siguiente:%s\n",resultado);
						break;
					}
					else if(strcmp(student_to_write,username_reg)==0)
					{
						passedLimit = 1;
					}
				}
				//Si es docente, ignoro.

			}else // Si no hay usuario para comparar desde el usuario base, termino la busqueda.
				break;
			
			
		}
		lineNumber++;
	}
	fclose(fichStudents);
	fclose(fichUsers);
	
	if(change==0){
		if(student_to_write == NULL)
			return -1; // Error -1, el archivo de asistencia ya fue previamente cargado.
		else
			return 0; // El ciclo anterior completo la asistnecia, entonces no hubo cambio. Termino la carga.
	}
	else
		return 1;// Se realizo una carga de asistencia.
}

int get_lines_of_file(FILE* fp)
{// Funcion auxiliar
	int lines=0;
	char ch;
	
	while(!feof(fp))
	{
	  ch = fgetc(fp);
	  if(ch == '\n')
		lines++;
	}
	return lines;
}
