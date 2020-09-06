
/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define PORT 4200

//the thread function
void *connection_handler(void *);

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Error en bind");
        return 1;
    }

    //Listen
    listen(socket_desc , 3);

    /*//Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);*/


    //Accept and incoming connection
    char str[5];
    sprintf(str,"%d",PORT);
    puts("Servidor en http://localhost:4200");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Conexion aceptada");

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("Error al crear el thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Gestor de conexion asignado");
    }

    if (client_sock < 0)
    {
        perror("Falla al aceptar la conexión");
        return 1;
    }

    close(client_sock);

    return 0;
}

char * imprimir_opciones(int logueado){
    char * opciones = "Seleccione una opción:\n1 - Ingresar al sistema\n";
    return opciones;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int usuario_logueado = 0;
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client
    /*message = "\nIngrese sus credenciales por favor\nUsuario: ";*/
    write(sock , imprimir_opciones(0) , strlen(client_message));

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	{
		//Send the message back to client
		//write(sock , imprimir_opciones(0) , strlen(client_message));


        if(!usuario_logueado && strcmp(client_message,"1") != 0){
            printf("Opción incorrecta");
        }
	}
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}

    close(sock);

    return 0;
}
