#! /bin/bash

#########################
###### Information ######
#########################

function information()
{
	echo "
--------------------------------------------------
--------------------------------------------------
Nombre del script: EJ4.sh
Trabajo practico 1 - Ejercicio 4
--------------------------------------------------

Grupo:
MIRANDA SERGIO JAVIER - 35.634.266
CARRIL ARANDA JOSE - 37.120.410
LUNA SANCHEZ MATIAS LEONEL - 37.141.163
RAMIREZ MATIAS GASTON - 39.505.838
MASINI JOAN EZEQUIEL - 39.562.503

--------------------------------------------------

El script se encarga de generar un zip con los log mas antiguos y dejando solo el de la semana mayor.

--------------------------------------------------
MODO DE USO
===========

    ./EJ5.sh -f <path dir Log> -z <path dir zip>  

    ./EJ5.sh -f <path dir Log> -z <path dir zip> -e <business name>

"
        exit 1;
}



#########################
# The command line help #
#########################

display_help() {
    echo "Usage: $0 [option...] " >&2
    echo
    echo "-f \"path\" Directorio en el que se encuentran los archivos de log. Puede ser una ruta relativa o absoluta."
    echo "-z \"path\" Directorio en el que se generarán los archivos comprimidos de los clientes."
    echo "-e \"empresa\" Nombre de la empresa a procesar.(opcional)"
    echo
    exit 1
}

#########################
#### Path validator #####
#########################

path_validator() {
    if [ "$1" == "" ]
    then
        echo "Empty path"
        exit 1;
    fi
    if [ ! -e "$1" ]
    then
        echo "Invalid Path : $1"
        exit 1;
    fi
}


if [ "$#" -eq 0 ];
then
    information
fi

if [[ "$#" != 0 && (("$1" == "-h") || ("$1" == "-?") || ("$1" == "-help") || ("$1" == "--help") || ("$1" == "-HELP") || ("$1" == "--HELP")) ]]
then 
    display_help
fi

#init business
business="w"

while getopts "f:z:e:" option; do
    case "${option}" in
        f ) # path logs
            pathLog=("$OPTARG")
            path_validator "${pathLog}"
            ;;
        z ) # path zip
            pathZip=("$OPTARG")
            path_validator "${pathZip}"
            ;;
        e ) # business
            business=("$OPTARG");;
        \? ) # incorrect option
            echo "Error: Invalid option"
            exit 1
            ;;
    esac
done
shift $((OPTIND -1))

if [ "${pathLog}" == "" ]
then
    echo "command -f is required"
    exit 1;
fi

if [ "${pathZip}" == "" ]
then
    echo "command -z is required"
    exit 1;
fi

regex='\'${business}'+\-[0-9]+\.log'

declare -A map

for var in $(ls "${pathLog}" | egrep "${regex}")

do
    business="${var/'-'*/''}"
    map["${business}"]=""
done

for var in "${!map[@]}"
do
    names=$(ls "${pathLog}" | egrep "\\${var}+\-[0-9]+\.log" | sort -r)
    max="${names/'.log'*/'.log'}"
    params="${names/${max}/''}"

    for arch in $(ls "${pathLog}" | egrep "\\${var}+\-[0-9]+\.log" | sort -r)
    do
        if [ "${arch}" != "${max}" ]
        then
            if [ ! -e "${pathZip}/${var}.zip" ]
            then
                $(zip -q "${pathZip}/${var}.zip" "${pathLog}/${arch}")
            else
                $(zip -r -q "${pathZip}/${var}.zip" "${pathLog}/${arch}")
            fi
            $(rm "${pathLog}/${arch}")
            echo "El archivo : [${arch}] ha sido movido al zip : [${var}.zip]"
        fi
    done
done