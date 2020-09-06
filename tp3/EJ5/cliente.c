/*
  Sistemas Operativos
    --------------------
    Trabajo Práctico N°3
    Ejercicio 5   
    --------------------
	 Grupo 5 Integrantes:
		MIRANDA SERGIO JAVIER - 35.634.266
		CARRIL ARANDA JOSE - 37.120.410
		LUNA SANCHEZ MATIAS LEONEL - 37.141.163
		RAMIREZ MATIAS GASTON - 39.505.838
		MASINI JOAN EZEQUIEL - 39.562.503
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#define BUFFER_DATE 800
#define BUFFER_CODE 50
#define MAX_CMD 5000
#define MAX_RES 20000

int validate_ip(char *);
int validate_number(char *);
int validate_user_type(char* result);
int validate_inputdata(char *);
int validate_inputdata_quit(char *);
int validate_inputdata_logout(char *);
int validate_inputdata_option(char *);
int valid_date(int dd, int mm, int yy);
int get_class_code(char* result);
int get_present_percentage(char* result);
int get_present_OK(char* result);
int show_students_by_date(char* result);
int set_class_state_student(char* result);
int starts_with(const char *totalName, const char *startWith);
void replace_newline(char*);
void press_key();
void clean_stdin(void);
int cfileexists(const char * filename);

int main(int argc , char *argv[])
{
    int sock_desc,
		puerto_servidor,
		ip_servidor;
		
    struct sockaddr_in serv_addr;
	
    char client_request_us[MAX_CMD],
		 client_request_pw[MAX_CMD],
		 comando_a_enviar[MAX_CMD],
		 comando_a_enviar_sub[MAX_CMD],
		 resultado[MAX_RES],
		 resultado_clone[MAX_RES],
		 quit_sentence[]="QUIT\0",
		 client_opt[MAX_CMD],
		 client_opt_param[MAX_CMD],
		 client_date[BUFFER_DATE],
		 client_code[BUFFER_CODE];
		 
	int client_response_code = 0;

    argc--;
    int carg=argc;
    
	if (argc==0)
	{// Mal parametros
		printf("Error en los parametros ingresados\n");
		printf("Use como parametro [help] o [-h]para obtener la ayuda.\n");
		return 0;
	}
	
    if (carg != 2)
	{// HELP
		if (strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0)
		{
			printf("----------------------------------------------------------------------------.\n");
			printf("El Servidor debe estar ejecutando antes de iniciar los clientes.\n");
			printf("Para iniciar el servidor, simplemente ejecute ./cliente [IP_SERVER] [PUERTO].\n");
			printf("Siga las indicaciones e ignore los corchetes al cargar datos.\n");
			printf("Si envia QUIT como dato, se descoenctara el cliente del servidor.\n");
			printf("----------------------------------------------------------------------------.\n");
			return 0;
		}
		printf("Error en los parametros ingresados\n");
		printf("Use como parametro [help] o [-h]para obtener la ayuda.\n");
		return 0;
	}
	
	char ip[20];
	strcpy(ip,argv[1]);
	printf("IP del servidor: %s\n",argv[1]);
	if (!validate_ip(ip))
	{// IP Invalida
		printf("La IP ingresada es invalida.\n");
		return 0;
	}
	
	char puertoC[7];
	strcpy(puertoC,argv[2]);
	
	if(validate_number(puertoC))
	{// PUERTO valido
		puerto_servidor = atoi(argv[2]);
		printf("Puerto %d.\n",puerto_servidor);
		if (puerto_servidor<=1 && puerto_servidor>=65000)
		{// Error en puerto
			printf("Error en el puerto ingresado.\n");
			printf("El puerto debe ser entre 1 y 65000.\n");
			return 0;
		}
	}
	else
	{// PUERTO invalido
		printf("Puerto invalido.\n");
		return 0;
	}


    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Fallo al crear el socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    int new_size = 100000 ;
    setsockopt(sock_desc, SOL_SOCKET, SO_RCVBUF , &new_size, sizeof(new_size));

    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //serv_addr.sin_port = htons(3000);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(puerto_servidor);

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) 
	{ // Fallo al conectar al svr
        printf("Fallo al conectar con el servidor\n");
        return -1;
    }
	
	printf("Conexion exitosa.\n\n");
	
	int logged_user = 0;
	int isTeacher = 0;
		
		
    do{
		//fflush(stdin);
		///puts("Limpiando buffers");
		memset(resultado,'\0',MAX_RES);
		memset(resultado_clone,'\0',MAX_RES);
		memset(client_opt,'\0',MAX_CMD);
		memset(client_opt_param,'\0',MAX_CMD);
		memset(comando_a_enviar,'\0',MAX_CMD);
		memset(comando_a_enviar_sub,'\0',MAX_CMD);
		memset(client_date,'\0',BUFFER_DATE);
		///puts("Buffers limpios");
		
		if(logged_user==0)
		{// Si no esta logueado
	
			///puts("Limpiando buffers internos de logeo");
			client_response_code = 0;
			memset(client_code,'\0',BUFFER_CODE);
			memset(client_request_us,'\0',MAX_CMD);
			memset(client_request_pw,'\0',MAX_CMD);
			
			//memset(resultado,'\0',MAX_RES);
			///puts("Buffers limpios internos de logeo");
			system("clear");
			puts("Usted no se encuentra logueado.");
			printf("Ingrese usuario y contraseña para loguearse.\n");
			printf("\nPara salir, ingrese [QUIT] en ambos campos.\nNOTA:SIEMPRE ignorar los corchetes.\n\n");
			
			printf("Ingrese usuario:");
			fgets(client_request_us, MAX_CMD , stdin);
			//replace_newline(client_request_us);
			strcpy(comando_a_enviar,"0|");
			strcat(comando_a_enviar,client_request_us);
			
			int n = strlen(comando_a_enviar);
			comando_a_enviar[n-1] = '|';
			
			printf("Ingrese contraseña:");
			fgets(client_request_pw, MAX_CMD, stdin);
			replace_newline(client_request_pw);
			strcat(comando_a_enviar,client_request_pw);
			
			//n = strlen(comando_a_enviar);
			//comando_a_enviar[n] = '\0';
			
			int i;
			///printf("CLIENTE ENVIA: %s\n", comando_a_enviar);
			
			if(!validate_inputdata(client_request_us) || !validate_inputdata(client_request_pw))
			{// No ingreso data
				printf("No se ingresaron datos, porfavor ingrese una segun el formato establecido.\n");
				printf("Ejemplo: [USUARIO]|[CONTRASEÑA]\n");
				press_key();
			}
			else if(validate_inputdata_quit(client_request_us) || validate_inputdata_quit(client_request_pw))
			{// Ingreso QUIT
				puts("Cerrando conexion...");
				send(sock_desc,quit_sentence,MAX_CMD,0);
				close(sock_desc);
				return 0;
			}
			else	
			{// Datos correctos ingresados, se envian al SVR.
				send(sock_desc,comando_a_enviar,MAX_CMD,0);
				recv(sock_desc,resultado,MAX_RES,MSG_WAITALL);
				//Esto recibe [LOGEADO_OK]|[TIPO_USER]|[COMISION]
				
				///printf("RECIBO DE SERVER:%s \n",resultado);
				
				// Si devuelve 1 es docente, si devuelve 2 es alumno, si devuelve 0 no logea.
				memset(resultado_clone,'\0',MAX_RES);
				strcpy(resultado_clone,resultado);
				isTeacher = validate_user_type(resultado_clone);
				//puts("/nPaso la validacion/n");
				
				if(isTeacher==1)
				{// Es docente
					logged_user = 1;
					isTeacher = 1;
					//puts("Llego docente!");
					memset(resultado_clone,'\0',MAX_RES);
					strcpy(resultado_clone,resultado_clone);
					client_response_code = get_class_code(resultado);
				}
				else if(isTeacher==2)
				{// Es alumno
					isTeacher = 0;
					logged_user = 1;
					//puts("Llego alumno!");
					memset(resultado_clone,'\0',MAX_RES);
					strcpy(resultado_clone,resultado_clone);
					client_response_code = get_class_code(resultado);
				}
				else
				{// No se logea
					//puts("No logea.");
					logged_user = 0;
					isTeacher = 0;
				}
				

				//A esta instancia ya tengo el flag de logeo,el tipo de usuario, el usuario logeado, su pw, y la comision.
				
				press_key();
			}
		}
		else
		{// Si esta logueado.
			system("clear");
			if(isTeacher)
			{ // Si es DOCENTE
				int opt;
				
				fflush(stdin);
				puts("Menu de docente");
				puts("================");
				puts("Opcion 1 - Consultar asistencias para una fecha indicada.");
				puts("\t +Ingrese: 1.");
				puts("Opcion 2 - Cargar asistencias para una fecha indicada.");
				puts("\t +Ingrese: 2.");
				puts("Opcion 3 - Cerrar sesion.");
				puts("\t +Ingrese: 3.");
				printf("\n\nIngrese la opción deseada:");
				fgets(client_opt, MAX_CMD , stdin);
				strcpy(comando_a_enviar,"1|"); //Tipo docente
				// [LOG_DOCENTE]|[XXXXXX]
				
				// Valido lo ingresado por teclado
				if(validate_inputdata_quit(client_opt))
				{// Es QUIT
					puts("Cerrando conexion...");
					send(sock_desc,quit_sentence,MAX_CMD,0);
					close(sock_desc);
					return 0;
				}
				else if(validate_inputdata_logout(client_opt))
				{// Es cerrar sesion
					puts("Se procederá a desloguearse.");
					logged_user = 0;
					isTeacher = 0;
				}
				else if(opt = validate_inputdata_option(client_opt))
				{// Es opcion valida
					replace_newline(client_opt);
					strcat(comando_a_enviar,client_opt);
					strcat(comando_a_enviar,"|");
					// [LOG_DOCENTE]|[OPCION]|[XXXXXX]
					
					int dd=0, mm=0, yy=0,dateIsValid;
					char xtraZero='0';
					
					do{ //Ingresa fecha valida
						printf("\n+Ingrese la fecha respetando el formato yyyy-mm-dd :");
						fflush(stdin);				
						scanf("%d-%d-%d",&yy,&mm,&dd);
						dateIsValid = valid_date(dd,mm,yy);
						
						if(!dateIsValid)
						{
							puts("Ha ingresado una fecha invalida.\nIntente nuevamente...");
							press_key();
						}
						
					}while(dateIsValid==0);
					
					memset(client_date,'\0',BUFFER_DATE);
					sprintf(client_date, "%d", yy);
					strcat(client_opt_param,client_date);

					if(mm > 9)
						sprintf(client_date, "-%d-", mm);
					else
						sprintf(client_date, "-%c%d-",xtraZero, mm);
					strcat(client_opt_param,client_date);
					
					if(dd > 9)
						sprintf(client_date, "%d", dd);
					else
						sprintf(client_date, "%c%d",xtraZero, dd);
					strcat(client_opt_param,client_date);
					
					strcat(comando_a_enviar,client_opt_param);
					strcat(comando_a_enviar,"|");
					// [LOG_DOCENTE]|[OPCION]|[yyyy-mm-dd]|[XXXXXX]
					
					memset(client_code,'\0',BUFFER_CODE);
					
					sprintf(client_code, "%d", client_response_code);
					//printf("Comision del usuario %s\n",client_code);
					strcat(comando_a_enviar,client_code);
					
					// [LOG_DOCENTE]|[OPCION]|[yyyy-mm-dd]|[COMISION]
					
					// ENVIO: [Logeado-DOCENTE]|[OPCION-SELEC]|[FECHA]|[COMISION]
					
					press_key();
					///printf("\nCLIENTE ENVIA:%s\n",comando_a_enviar);
					
					char fileName [100];
					int fileExsits = 0;
					sprintf(fileName, "Asistencia_%s_%s.txt",client_opt_param,client_code);
					
					if((fileExsits=cfileexists(fileName)) && opt == 2)
					{
						system("clear");
						printf("ERROR - Ya existe el archivo de asistencia para la fecha y comision que intenta cargar.\nARCHIVO: [%s]\n\n",fileName);
					}
					else
					{
						send(sock_desc,comando_a_enviar,MAX_CMD,0);
						recv(sock_desc,resultado,MAX_RES,MSG_WAITALL);
					}
					
					if(opt == 1)
					{// Opcion 1 - Consultar asistencias p/fecha
				
						// ACA ENVIO: [Logeado-DOCENTE]|[1]|[FECHA]|[COMISION]
						// Ej: 1|1|dd-MM-aaaa|1
						
						system("clear");
						///printf("\nCLIENTE ENVIA:%s\n",comando_a_enviar);
						
						puts("OP.1 - Consultar asistencias de su comision para una fecha indicada.");
						puts("================");
						printf("Listado de asistencias [%s]\n",client_opt_param);
						puts("----------------------------\n");
						
						//Trato la respuesta para opcion 1.
						///puts("Resultado de respuesta para opcion 1.");
						///printf("RECIBO DE SERVER:%s \n\n",resultado);
						int can_show = show_students_by_date(resultado);
						if(can_show==1)
							puts("\nLista finalizada.");
						else if(can_show==0)
							puts("La lista de asistencias se encuentra vacia.");
						else
							puts("Ocurrio un error. Puede que la fecha ingresada no tenga un archivo asistencias./n");
						
					}
					else if(opt == 2 && !fileExsits)
					{// Opcion 2 - Cargar asistencias p/fecha
						
						// ACA ENVIO: [Logeado-DOCENTE]|[2]|[FECHA]|[COMISION]
						// Ej: 1|2|dd-MM-aaaa|1
						int asist_status=-2;
						system("clear");
						
						///printf("\nCLIENTE ENVIA:%s\n",comando_a_enviar);
						puts("OP.2 - Cargar asistencias para una fecha indicada.");
						puts("===================================================");
						printf("Cargando asistencias para [%s]\n",client_opt_param);
						puts("Ingrese [P]o[A] segun corresponda. (Presente o Ausente)\n");
						puts("--------------------------------------------------------\n");
						
						//Trato la respuesta para opcion 2.
						///puts("Resultado de respuesta para opcion 2.");
						///printf("CLIENTE RECIBE:%s \n",resultado);
						// La idea es recibir [ALUMNO]
						// Creo otro comando a enviar para reutilizar y no tocar el original que contiene algunos parametros utiles.
						// En el original [Logeado-DOCENTE]|[2]|[FECHA]|[COMISION]
						memset(comando_a_enviar_sub,'\0',MAX_CMD);

						asist_status = set_class_state_student(resultado);
						// 1 es presente, 0 ausente.
						// -1 termino lista, -2 error de apertura, -3 existe archivo.
						///printf("El seteo:%d\n",asist_status);
						
						if(asist_status == -1)
							puts("Error, parece que ya existe el archivo de asistencia para la fecha ingresada.");
						else if(asist_status == -2)
							puts("Ocurrio un error al abrir algun archivo");
						else
						{
							while(asist_status!=0)
							{	
								// Agrego a lo enviado anteriormente el alumno ya leido al final y el presente/ausente cargado
								// [Logeado-DOCENTE]|[2]|[FECHA]|[COMISION]

								strcpy(comando_a_enviar_sub,comando_a_enviar);
								strcat(comando_a_enviar_sub,"|");
								strcat(comando_a_enviar_sub,resultado);
								// [Logeado-DOCENTE]|[2]|[FECHA]|[COMISION]|[ALUMNO]
								strcat(comando_a_enviar_sub,"|");
								if(asist_status==1)
									strcat(comando_a_enviar_sub,"P");
								else
									strcat(comando_a_enviar_sub,"A");
								// [Logeado-DOCENTE]|[2]|[FECHA]|[COMISION]|[ALUMNO]|[PoA]
								
								///printf("Comado_enviar_Base: %s\n",comando_a_enviar);
								///printf("Comado_enviar_Sub: %s\n",comando_a_enviar_sub);
								
								
								///printf("\nCLIENTE ENVIA:%s\n",comando_a_enviar_sub);
								send(sock_desc,comando_a_enviar_sub,MAX_CMD,0);
								memset(resultado,'\0',MAX_RES);
								recv(sock_desc,resultado,MAX_RES,MSG_WAITALL);
								//printf("\nRECIBO DE SVR:%s\n",resultado);
								// La idea es recibir otro [ALUMNO], el siguiente. O cero si se acabo.
								asist_status = set_class_state_student(resultado);
								memset(comando_a_enviar_sub,'\0',MAX_CMD);
							}
							
							puts("Fin del listado de alumnos.");
							puts("Se finalizo la carga de asistencia.");	
							//press_key();
							clean_stdin();
						}
				
						
						
						
						
					}
				
				}
				else
				{// Opcion no reconocida
					system("clear");
					puts("---------------------------");
					puts("La opción ingresada es inválida.\nIngrese nuevamente.\n");
				}
				press_key();
			}
			else
			{ // Si es ALUMNO
				int opt;
				fflush(stdin);
				memset(comando_a_enviar,'\0',MAX_CMD);
				memset(client_opt,'\0',MAX_CMD);
				
				puts("Menu de alumno");
				puts("================");
				puts("Opcion 1 - Consultar mi asistencia en una fecha.");
				puts("\t +Ingrese: 1.");
				puts("Opcion 2 - Consultar porcentaje de asistencias / inasitencias.");
				puts("\t +Ingrese: 2.");
				puts("Opcion 3 - Cerrar sesion.");
				puts("\t +Ingrese: 3.");
				printf("\n\nIngrese la opción deseada:");
				fgets(client_opt, MAX_CMD , stdin);
				strcpy(comando_a_enviar,"2|"); //Tipo alumno
				// [LOG_ALUMNO]|[XXXXXX]
				
				// Valido lo ingresado por teclado
				if(validate_inputdata_quit(client_opt))
				{// Es QUIT
					puts("Cerrando conexion...");
					send(sock_desc,quit_sentence,MAX_CMD,0);
					close(sock_desc);
					return 0;
				}
				else if(validate_inputdata_logout(client_opt))
				{// Es cerrar sesion
					puts("Se procederá a desloguearse.");
					logged_user = 0;
					isTeacher = 0;
					press_key();
				}
				else if(opt = validate_inputdata_option(client_opt))
				{// Es opcion valida
					
					replace_newline(client_opt);
					strcat(comando_a_enviar,client_opt);
					strcat(comando_a_enviar,"|");
					// [LOG_ALUMNO]|[OPCION]|[XXXXXX]
					
					
					strcat(comando_a_enviar,client_request_us);
					
					// [LOG_ALUMNO]|[OPCION]|[USUARIO]
					
					if(opt == 1)
					{// Opcion 1 - Consultar mi asistencia p/fecha
				
						memset(comando_a_enviar_sub,'\0',MAX_CMD);
						memset(client_date,'\0',BUFFER_DATE);
						strcpy(comando_a_enviar_sub,comando_a_enviar);
						replace_newline(comando_a_enviar_sub);
						system("clear");
						puts("OP.1 - Consultar mi asistencia para una fecha indicada.");
						puts("========================================================");
						
						int dd=0, mm=0, yy=0, dateIsValid=0,asist=-1;
						char xtraZero='0';
						
						do{// Ingresa fecha valida
							printf("\n+Ingrese la fecha respetando el formato [yyyy-mm-dd]:");
							fflush(stdin);
							//fgets(client_opt_param, MAX_CMD , stdin);
							
							scanf("%d-%d-%d",&yy,&mm,&dd);
							dateIsValid = valid_date(dd,mm,yy);
							
							if(!dateIsValid)
							{
								puts("Ha ingresado una fecha invalida.\nIntente nuevamente...\n");
								//press_key();
							}
							
						}while(dateIsValid==0);
						
						
						sprintf(client_date, "%d",yy);
						strcat(client_opt_param,client_date);

						if(mm > 9)
							sprintf(client_date, "-%d-", mm);
						else
							sprintf(client_date, "-%c%d-",xtraZero, mm);
						
						strcat(client_opt_param,client_date);
						
						if(dd > 9)
							sprintf(client_date, "%d", dd);
						else
							sprintf(client_date, "%c%d",xtraZero, dd);
						
						strcat(client_opt_param,client_date);
						
						strcat(comando_a_enviar_sub,"|");
						strcat(comando_a_enviar_sub,client_opt_param);
						// [LOG_ALUMNO]|[OPCION]|[USUARIO]|[yyyy-mm-dd]
						
						// ENVIA: [Logeado-ALUMNO]|[OPCION-SELEC]|[USUARIO]|[FECHA]
						// Ej: 2|1|[USUARIO]|dd-MM-aaaa 
						// Ver si devolver si 1 o 0 si estuvo o no.
						
						///printf("\nCLIENTE ENVIA:%s\n",comando_a_enviar_sub);
						send(sock_desc,comando_a_enviar_sub,MAX_CMD,0);
						recv(sock_desc,resultado,MAX_RES,MSG_WAITALL);
						///printf("RECIBO DE SERVER:%s \n",resultado);
						asist = get_present_OK(resultado);
						
						puts("=============================");		
						if(asist==1)
							printf("Usted estuvo PRESENTE el dia [%s]\n",client_opt_param);
						else if(asist==0)
							printf("Usted estuvo AUSENTE el dia [%s]\n",client_opt_param);
						else
							puts("No se encontro un archivo de asistencia para la fecha mencionada.");
						puts("=============================");
						
				
						press_key();
						getchar();
						
						memset(comando_a_enviar_sub,'\0',MAX_CMD);
						memset(client_date,'\0',BUFFER_DATE);
					}
					else 
					{// Opcion 2 - Consultar porcentajes de asistencia / inasistencias
						system("clear");
						puts("OP.2 - Consultar porcentaje de asistencias / inasistencias");
						puts("============================================================");
						int per_asist = 0;
						
						// Un alumno no puede estar en mas de una comision.
						// Enviar: [Logeado-ALUMNO]|[OPCION-SELEC]|[USUARIO]
						// Ej: 2|2|[USUARIO] 
						// Esto deberia ser suficiente para que el servidor 
						// interprete que el alumno pide su propio porcentaje de su propia comision.
						
						///printf("\nCLIENTE ENVIA:%s\n",comando_a_enviar);
						send(sock_desc,comando_a_enviar,MAX_CMD,0);
						recv(sock_desc,resultado,MAX_RES,MSG_WAITALL);
						///printf("RECIBO DE SERVER:%s \n",resultado);
						per_asist = get_present_percentage(resultado);
						
						if(per_asist < 0 )
							puts("Ha ocurrido un error al obtener porcentaje de asistencias.");
						else
						{
							puts("==============================================================");
							printf("Su porcentaje es: %d %% asistencias y %d %% inasistencias.\n",per_asist,100-per_asist);
							puts("===============================================================");
						}
						press_key();
					}

				}
				else
				{// Opcion no reconocida
					system("clear");
					puts("--------------------------------------------------------");
					puts("La opción ingresada es inválida.\nIngrese nuevamente.\n");
					fflush(stdin);
					press_key();
				}

			}
		}
		//system("clear");

    }while(1);
}

int validate_number(char *str) 
{// Valida que se haya ingresado un numero
   while (*str) 
   {
      if(!isdigit(*str))
	  { //if the character is not a number, return
         //false
         return 0;
      }
      str++; //point to next character
   }
   return 1;
}

int validate_ip(char *ip) 
{ //Valida la IP ingresada
   int i, num, dots = 0;
   char *ptr;
   
   if (ip == NULL)return 0;
   
   ptr = strtok(ip, "."); //CORTA STRING CON EL DELIM
   
   if (ptr == NULL)return 0;
   
	while (ptr) 
	{
		if (!validate_number(ptr))return 0;

		num = atoi(ptr); //CONVIERTE SUBSTRING A NUM
		
		if (num >= 0 && num <= 255) 
		{
			ptr = strtok(NULL, "."); //CORTA LA SIG PARTE DEL STR
			
			if (ptr != NULL)
			   dots++; //INCREMENTA CONT DE PUNTOS
		   
		} else return 0;
	}
	
    if (dots != 3) //SI LOS PUNTOS NO SON 3, FUE.
       return 0;
	   
    return 1;
}

int validate_inputdata(char* request_data)
{// Valida que haya ingresado data
	char aux[MAX_CMD];
	strcpy(aux, request_data);
	int nm = strlen(aux);
	aux[nm-1]= '\0';
	return nm-1;
}

int validate_inputdata_quit(char* request_data)
{// Valida que la opcion ingresada sea para cerrar socket
	char auxi[MAX_CMD];
	strcpy(auxi, request_data);
	int nm = strlen(auxi);
	auxi[nm-1]= '\0';
	return !strcmp(auxi,"QUIT") || !strcmp(auxi,"quit");
}

int validate_inputdata_logout(char* request_data)
{// Valida que la opcion ingresada sea para cerrar sesion
	char auxi[MAX_CMD];
	strcpy(auxi, request_data);
	int nm = strlen(auxi);
	auxi[nm-1]= '\0';
	return !strcmp(auxi,"3");
}

int validate_inputdata_option(char* request_data)
{// Valida que se hayan ingresado opciones validas, 1 o 2
	char auxi[MAX_CMD];
	strcpy(auxi, request_data);
	int nm = strlen(auxi);
	auxi[nm-1]= '\0';
	
	if(!strcmp(auxi,"1"))
		return 1;
	else if (!strcmp(auxi,"2"))
		return 2;
	else
		return 0;
}

int valid_date(int dd, int mm, int yy)
{// Valida la fecha ingresada
	
    if(yy>=1900 && yy<=9999)
    {
        if(mm>=1 && mm<=12)
        {
            if((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12))
                return 1;
            else if((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11))
                return 1;
            else if((dd>=1 && dd<=28) && (mm==2))
                return 1;
            else if(dd==29 && mm==2 && (yy%400==0 ||(yy%4==0 && yy%100!=0)))
                return 1;
            else
				return 0;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }  
}

void press_key()
{// Funcion para confirmar continuidad

	puts("Presione cualquier tecla para continuar...");
	fflush(stdin);
	getchar();
	fflush(stdin);
}

// Server //
int validate_user_type(char* result)
{// Valida que sea un usuario del tipo DOCENTE desde respuesta de SVR
	// Si devuelve 0 no es profe. Si devuelve 1 es profe.
	char del[] = "|";
	int type=0;
	char *log_ok = strtok(result, del),
	*user_type,
	*class_code;
	
	if(log_ok != NULL)
	{// Desgloza respuesta de servidor.

		if(atoi(log_ok)==0)
			return 0;
		
		user_type = strtok(NULL, del);
		
		if(user_type != NULL)
			class_code = strtok(NULL, del);
	}
	type = atoi(user_type);
	return type;
}

int get_class_code(char* result)
{// Obtiene el CODIGO DE COMISION desde respuesta de SVR
		//[LOG_OK]|[TYPE_USER]|[CODE_COM]
		char del[] = "|";
		int code=0;
		char *log_ok = strtok(result, del),
		*user_type,
		*class_code;

		if(log_ok != NULL)
		{// Desgloza respuesta de servidor.
			user_type = strtok(NULL, del);
			
			if(user_type != NULL)
				class_code = strtok(NULL, del);
		}
		
		code = atoi(class_code);
		
		return code;
}

int get_present_percentage(char* result)
{// Obtiene el PORCENTAJE DE ASISTENCIA desde respuesta de SVR

		if(strcmp(result,"-1")==0)
		{// No se encontraron archivos para presentismo.
			return -1;
		}

		int percentage = -1;
		percentage = atoi(result);
		
		return percentage;
}

int get_present_OK(char* result)
{// Obtiene el presente/ausente desde respuesta de SVR
	
	if(strcmp(result,"-1")==0)
	{// No se encontro alumno o archivo o ocurrio error.
		return -1;
	}
	else if(strcmp(result,"1")==0)
	{// Esta presente.
		return 1;
	}
	else
		return 0; // Esta ausente.
	
}

int show_students_by_date(char* result)
{// Obtiene el LISTADO DE ALUMNOS desde respuesta de SVR
	// Listar alumnos separados por \n.
	// Devolver 1 si se listo correctamente.
	// Devolver 0 si respuesta de svr vino vacia, creo que result viene con -1 si no encuentra el file.
	
	if(strcmp(result,"-1")==0)
	{// No encontro el file.
		return -1;
	}
	
	printf("%s",result);
	return 1;
}

int set_class_state_student(char* result)
{// Mediante la respuesta del servidor que corresponde a un alumno, el docente ingresa P o A segun corresponda.
	// Retorna 1 para presente, 0 termino , -1 existe, -2 error al querer abrir archivos.
	char asistencia[BUFFER_CODE];

	if(starts_with(result,"-1"))
	{// Error, el archivo existe.
		return -1;
	}
	
	if(starts_with(result,"-2"))
	{// Error, al abrir archivos.
		return -2;
	}
	
	if(starts_with(result,"0"))
	{// Se acabaron los estudiantes, basta de leer
		return 0;
	}
	
	do{
		memset(asistencia,'\0',BUFFER_CODE);
		printf("Alumno: %s se encuentra [P]o[A]:", result);
		scanf("%s",asistencia);

	   if(!(starts_with(asistencia,"P")||starts_with(asistencia,"A")))
	   {
			puts("Opcion invalida, intente nuevamente.\nPresione cualquier tecla para continuar...\n");
			fflush(stdin);
			clean_stdin();
			memset(asistencia,'\0',BUFFER_CODE);
	   }
	}while(!(starts_with(asistencia,"P")||starts_with(asistencia,"A")));

	if(starts_with(asistencia,"P"))
		return 1;
	else
		return 2;
}

void replace_newline(char* word)
{// Es necesario porque cuando ingreso valores desde consola, les clava el \n.
	// Sino, los mensajes x el socket se mandan mal.
	char auxi[MAX_CMD];
	strcpy(auxi, word);
	int nm = strlen(auxi);
	word[nm-1]= '\0';
	return;
}

int starts_with(const char *totalName, const char *startWith)
{// Funcion auxiliar para saber si un string comienza con un substring.
   if(strncmp(totalName, startWith, strlen(startWith)) == 0) return 1;
   return 0;
}

void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int cfileexists(const char * filename)
{// Chequea si existe archivo.
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}
