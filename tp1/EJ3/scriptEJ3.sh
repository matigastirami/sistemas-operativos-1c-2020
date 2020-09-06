#!/bin/bash

# ---------------------------------------------- #
# ---------------------------------------------- #
# Nombre del script: scriptEJ3.sh
# Trabajo practico N°1
# Ejercicio: 3
# Numero de entrega: 1 (entrega)
# Grupo:
#	MIRANDA SERGIO JAVIER - 35.634.266
#	CARRIL ARANDA JOSE - 37.120.410
#	LUNA SANCHEZ MATIAS LEONEL - 37.141.163
#	RAMIREZ MATIAS GASTON - 39.505.838
#	MASINI JOAN EZEQUIEL - 39.562.503

# ---------------------------------------------- #
# ---------------------------------------------- #

# El script se encarga de eliminar automáticamente los logs viejos cada vez que se crea un archivo nuevo. Evalúa todos los archivos del directorio y deja solamente los de la última semana de cada empresa.
# Convención para los nombres de los logs: nombreEmpresa-númeroSemana.log

# ---------------------------------------------- #
# ---------------------------------------------- #

# Validaciones

FOLDER=""
TIME=0

if (( $# < 1 || $# > 4 ))
then
	echo "Parámetros incorrectos. Use -h, -help para obtener ayuda."
	exit 1;
fi

while getopts "?hhelp:f:t:" option; do
	case "${option}" in
		h) 	#Help
			printf "Parámetros:\n\t-f \tPath de la carpeta (relativa o absoluta).\n\t-t \tTiempo (en segundos) que desea monitorear la carpeta del primer parámetro.\n"
			exit ;;
		f) 	# Path
			FOLDER=("$OPTARG");;
		t) 	# Tiempo
			TIME=("$OPTARG");;
		*) 	# Incorret
			echo "Parámetros incorrectos. Use -h, -help para obtener ayuda."
			exit ;;
	esac
done

shift $((OPTIND -1))

if ! test -r "$FOLDER"
then
	echo "Path inválido."
	exit 1;
fi

if (( "$TIME" <= 0 ))
then
	echo "El tiempo debe ser un número y mayor a 0."
	exit 1;
fi

deleteOldLogs() {

	# Array de los archivos a eliminar.
	declare -a filesToDelete

	# Regex validador de archivos de log.
	REGEX_FILES='(\w+)\-([0-9]+)\.log'

	# Cantidad actual de archivos.
	CURRENT_COUNT_FILES=$(ls "$FOLDER" | wc -l)

	while true ; do
		
		# Verificar si crearon un archivo nuevo.
		if [[ "$CURRENT_COUNT_FILES" < $(ls "$FOLDER" | wc -l) ]]
		then
			# Listar  los archivos ordenados.
			files=($(ls "$FOLDER" | egrep "$REGEX_FILES" | sort -k1,1 -t - -k2nr))
			
			# Se resta uno para que el for termine una posicion antes.
			MAX=$((${#files[@]} - 1))
			
			for i in $(seq 0 "$MAX")
			do
				name="$(cut -d'-' -f1 <<< ${files[i]})"
				nextName="$(cut -d'-' -f1 <<< ${files[$(($i+1))]})"
		
				# Se compara el nombre actual con el siguiente.
				# Si son iguales se guarda (para eliminar) el siguiente archivo ya que es de una semana anterior.
				if [[ $name == $nextName ]]
				then
					filesToDelete+=(${files[i+1]})
				fi
	
			done
	
			# Se eliminan las semanas anteriores.
			for files in ${filesToDelete[@]}
			do
				rm -f "$FOLDER/$files"
			done
			
			# Se actualiza la cantidad actual de archivos.
			CURRENT_COUNT_FILES=$(ls "$FOLDER" | wc -l)
		fi
		#Monitorea cada "t" segundos
		sleep "$TIME"
	done
}

#Demonio
deleteOldLogs &

echo PID $!

kill $$
