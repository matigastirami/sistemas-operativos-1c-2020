#!/bin/bash 

ErrorS()
{
echo "Error. La sintaxis del script es la siguiente:"
echo "Para saber numeros de lineas del archivo: $0 nombre_archivo L" # COMPLETAR
echo "Para saber el numero de caracteres del archivo: $0 nombre_archivo C" # COMPLETAR
echo "Para saber la longitud de la linea mas larga del archivo: $0 nombre_archivo M" # COMPLETAR
}
ErrorP()
{
echo "Error. nombre_archivo no existe o no se puede acceder para lectura" # COMPLETAR
}
if test $# -lt 2; then #compara si la cantidad de parametros es menor que 2
ErrorS #llama a esta funcion
fi

if  test ! -r $1; then # valida de forma negada si el primer parametro tiene permisos de lectura
ErrorP #llama a esta funcion
elif test -f "$1" && (test "$2" = "L" || test "$2" = "C" || test "$2" = "M") ;then #verifica si el primer parametro es un fichero que existe y si el segundo parametro es "L" o "C" o "M". 
if test $2 = "L"; then #pregunta si el segundo parametro es igual a "L"
res=`wc -l $1` #calcula la cantidad de lineas del primer parametro y lo asigna en la variable res.
echo "numero de lineas del archivo: $res" # COMPLETAR
elif test $2 = "C"; then #pregunta si el segundo parametro ingresado es igual a "C"
res=`wc -m $1` #calcula la cantidad de caracteres del primer parametro y lo asigna en la variable res.
echo "numero de caracteres del archivo: $res" # COMPLETAR
elif test $2 = "M"; then #pregunta si el segundo parametro ingresado es igual a "M".
res=`wc -L $1` #calcula la longitud de la linea mas larga del primer parametro y lo asigna en la variable res.
echo "longitud de la linea mas larga: $res" # COMPLETAR
fi
else
ErrorS #llama a esta funcion
fi

# a- cual es el objetivo de este script?
# el objetivo de este script es primero validar que la cantidad de parametros ingresados no sea menor que dos, luego que el primer parametro ingresado sea un archivo con permisos de lectura y no un directorio, luego validar que el segundo parametro sea ("L" o "C" o "M") para luego mostrar por pantalla el numero de lineas o el numero de caracteres o la longitud mas larga del archivo dependiendo de que letra se ingreso como segundo parametro.  
# b- que parametros recibe?
# recibe dos parametros: el primero es la direccion del archivo y segundo una letra (L,C,M) que sirve para mostrar una caracteristica del archivo.
# c- comentar el codigo segun la funcionalidad
# la logica del codigo seria que si al principio no ingresamos 2 parametros nos muestra un mensaje, si ingresamos 2 parametros primero verifica que el primer parametro sea un archivo y si lo es verifica cual letra es el segundo parametro. Si la letra coincide con (L,C,M) realizara una de las siguientes funciones: mostrar numeros de lineas, mostrar longitud de la linea mas larga o mostrar el numero de palabras.
# d- completar los echo con los mensajes correspondientes
# e- ¿Qué información brinda la variable “$#”? ¿Qué otras variables similares conocen?
# $# : es el numeros de argumentos que le pasa al script al ejecutarlo
# $? : la salida del ultimo proceso que se ha ejecutado
# $$ : el id del proceso del script
# $0 : representa el nombre del script
# f- Explique las diferencias entre los distintos tipos de comillas que se pueden utilizar en Shell scripts.
# " ": Lo que esté dentro de las comillas dobles se toma literalmente a excepción de los caracteres $ (sustitución de variable), ` (sustitución de comando) y \ (caracter de escape).
# ' ': Las comillas simples (') también se usan para encerrar frases sin embargo todo lo que esta dentro de las comillas simples es tomado literalmente.
# ` `: Las comillas invertidas (`) son interpretadas como sustitución de comandos, es decir, los comandos que estén dentro serán pasados al comando que lo preceden.
