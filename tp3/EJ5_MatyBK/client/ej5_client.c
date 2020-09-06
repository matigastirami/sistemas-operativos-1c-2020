#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 2000
#define PORT 4200
#define SA struct sockaddr
void func(int sockfd)
{
    printf("/** Sistema universidad **/\n");
    char message[MAX] = "", server_reply[MAX];
    int n, usuario_logueado = 0;
    /* Recibir mensaje inicial del servidor */
    if (recv(sockfd, server_reply, MAX, 0) < 0)
    {
        puts("recv failed");
        return;
    }
    else
    {
        puts(server_reply);
        while (1)
        {
            if(!usuario_logueado){
                printf("Ingrese una opción y presione enter: ");
                scanf("%s", message);
            }
            

            //Send some data
            if (send(sockfd, message, strlen(message), 0) < 0)
            {
                puts("Send failed");
                return 1;
            }

            //Receive a reply from the server
            if (recv(sockfd, server_reply, MAX, 0) < 0)
            {
                puts("recv failed");
                break;
            }

            puts("Server reply :");
            puts(server_reply);
        }
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Fallo en la creación del socket...\n");
        exit(0);
    }
    /*else
        printf("Socket creado exitosamente..\n");*/
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Error al conectar con el servidor...\n");
        exit(0);
    }
    /*else
        printf("Conectado al servidor..\n");*/

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}
