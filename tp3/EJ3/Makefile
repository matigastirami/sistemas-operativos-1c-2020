all: socio pagos

socio: socio.o
	gcc -o socio socio.o -lrt -lpthread
socio.o: socio.c
	gcc -c socio.c -lrt -lpthread

pagos: pagos.o
	gcc -o pagos pagos.o -lrt -lpthread

pagos.o: pagos.c
	gcc -c pagos.c -lrt -lpthread

clean: 
	rm *.o 
