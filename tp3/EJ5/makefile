all: servidor cliente

servidor: servidor.o
	gcc -o servidor servidor.o -lpthread
servidor.o: servidor.c
	gcc -c servidor.c -lpthread

cliente: cliente.o
	gcc -o cliente cliente.o -lpthread

cliente.o: cliente.c
	gcc -c cliente.c -lpthread


clean: 
	rm *.o 
