#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
void funcion(int num,int vec[],int vec2[],int num2);
void ayuda();
int main(int argc, char*argv[])
{
 if(argc<=1){
    printf("ERROR, no ha ingresado un numero\n");
    exit(0);
 }
 if(argc>2){
    printf("ERROR, ha ingresado mas de un parametro\n");
    exit(0);
 }
 if(strcmp(argv[1],"-h")==0||strcmp(argv[1],"-H")==0||strcmp(argv[1],"-help")==0||strcmp(argv[1],"-HELP")==0||strcmp(argv[1],"-?")==0){
    ayuda();
    exit(0);
 }
 if(atoi(argv[1])<0){
    printf("ERROR, ha ingresado un numero negativo\n");
    exit(0);
 }
 if(!isdigit(*argv[1])){
    printf("Error no ha ingresado un numero como parametro\n");
    exit(0);
 }
 int valor=atoi(argv[1]);
 printf("el numero ingresado es:%d\n",valor);
 if(valor<0){
    printf("ERROR, ha ingresado un numero negativo\n");
    exit(0);
 }
 pid_t pid0,pid1,pid2,pid3,pid4;
 int num=valor;
 int num2=1;
 int num3=4;
 int vec[4+num];
 int vec2[4+num];
 for(int i=0;i<(4+num);i++){
    if(i==0){
        vec2[i]=0;
    }else{
        if(i==1){
            vec2[i]=2;
        }else{
            if(i==2){
                vec2[i]=4;
            }else{
                if(i==3){
                    vec2[i]=7;
                }else{
                    vec2[i]=7+num2;
                    num2++;
                }
            }
        }
    }
 }
 printf("0(%d)\n",getpid());
 pid0=getpid();
 for(int i=1;i<=3;i++){
    if(!fork()){
        pid1=getpid();
        printf("%d(%d)-0(%d)\n",i,pid1,pid0);
        if(i==2){
        for(int j=4;j<=5;j++){
            if(!fork()){
                pid2=getpid();
                printf("%d(%d)-%d(%d)-0(%d)\n",j,pid2,i,pid1,pid0);
                if(j==4){
                    if(!fork()){
                       pid4=getpid();
                        printf("7(%d)-%d(%d)-%d(%d)-0(%d)\n",pid4,j,pid2,i,pid1,pid0);
                        vec[0]=pid0;
                        vec[1]=pid1;
                        vec[2]=pid2;
                        vec[3]=pid4;
                        funcion(num,vec,vec2,num3);
                        exit(0);
                    }
                }
                exit(0);
            }
        }
        }
        if(i==3){
            if(!fork()){
            pid3=getpid();
                printf("6(%d)-%d(%d)-0(%d)\n",pid3,i,pid1,pid0);
                exit(0);
            }
        }
        exit(0);
    }
 }

 return 0;
}
void funcion(int num,int vec[],int vec2[],int num3){
  pid_t pid1;
    if(num==0){
        return;
    }
    else{
        if(!fork()){
            pid1=getpid();
            vec[num3]=pid1;
            for(int i=num3;i>=0;i--){
                if(i==0){
                    printf("%d(%d)",vec2[i],vec[i]);
                }else{
                    printf("%d(%d)-",vec2[i],vec[i]);
                }

            }
            printf("\n");
            num3++;
            funcion(num-1,vec,vec2,num3);
            exit(0);
        }
    }
}
void ayuda(){
    printf("Ingrese un numero N para crear N jerarquias de procesos a partir del subproceso 7\n");
    printf("Ejemplo de ejecucion:\n");
    printf("1- Make ej1 \n");
    printf("2- ./ej1 5 \n");
}
