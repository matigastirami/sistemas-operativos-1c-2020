all: consumidor productor_pagos productor_asistencias

consumidor: consumidor.o
	gcc -o consumidor consumidor.o -lrt -lpthread

consumidor.o: consumidor.c
	gcc -c consumidor.c -lrt -lpthread

productor_pagos: productor_pagos.o
	gcc -o productor_pagos productor_pagos.o -lrt -lpthread

productor_pagos.o: productor_pagos.c
	gcc -c productor_pagos.c -lrt -lpthread

productor_asistencias: productor_asistencias.o
	gcc -o productor_asistencias productor_asistencias.o -lrt -lpthread

productor_asistencias.o: productor_asistencias.c
	gcc -c productor_asistencias.c -lrt -lpthread

clean: 
	rm *.o 
