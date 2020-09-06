#!/bin/bash
# --------------------------------------------------
# --------------------------------------------------
# Nombre del script: EJ6.sh
# Trabajo practico: 1
# Ejercicio: 6
# Numero de entrega: 1 (entrega)
# Grupo:
#   MIRANDA SERGIO JAVIER - 35.634.266
#   CARRIL ARANDA JOSE - 37.120.410
#   LUNA SANCHEZ MATIAS LEONEL - 37.141.163
#   RAMIREZ MATIAS GASTON - 39.505.838
#   MASINI JOAN EZEQUIEL - 39.562.503

# Funciones generales a los 2 approach
gcd() {
	until test 0 -eq "$2"; do
		set -- "$2" "`expr "$1" % "$2"`"
	done

	test 0 -gt "$1" && set -- "`expr 0 - "$1"`"
	echo "$1"
}
 
lcm() {
	set -- "$1" "$2" "`gcd "$1" "$2"`"
	set -- "`expr "$1" \* "$2" / "$3"`"
	test 0 -gt "$1" && set -- "`expr 0 - "$1"`"
	echo "$1"
}

strindex() { 
  x="${1%%$2*}"
  [[ "$x" = "$1" ]] && echo -1 || echo "${#x}"
}

sumaFracciones(){
    f1=$1
    f2=$2
    entero=$3
    if [ $entero -lt 0 ]; then
        f2="-$f2"
    fi
    IFS='\/' read -a split_f1 <<< $f1
    IFS='\/' read -a split_f2 <<< $f2

    mcm=`lcm ${split_f1[1]} ${split_f2[1]}`
    echo $(($mcm / ${split_f1[1]} * ${split_f1[0]} + $mcm / ${split_f2[1]} * ${split_f2[0]} + $entero * $mcm))/$(($mcm))
}

simplificarFraccion(){
    fraccion=$1
    IFS='\/' read -a split_fraccion <<< $fraccion
    mcd=`gcd ${split_fraccion[0]} ${split_fraccion[1]}`
    while [[ "$mcd" -gt 1 ]]
    do
        fraccion=$((${split_fraccion[0]} / $mcd))/$((${split_fraccion[1]} / $mcd))
        IFS='\/' read -a split_fraccion <<< $fraccion
        mcd=`gcd ${split_fraccion[0]} ${split_fraccion[1]}`
    done
    if [[ "${split_fraccion[1]}" -eq 1 ]]
    then
        echo ${split_fraccion[0]}
    else
        echo $fraccion
    fi
}

procesarArchivo(){
    IFS='' read -r line < $1
    resultado_parcial="0/1"
    if [[ $line =~ ^(\-{0,}([0-9]{0,}:{0,}[0-9]{1,}\/[0-9]{1,}|[0-9]+)\,{0,}){1,}$ ]];
    then
        
        
        IFS=',' read -r -a fractions <<< "$line"
        for element in "${fractions[@]}"
        do
        
            IFS='\/' read -a fraction <<< $element
            
            if [ ${#fraction[@]} -eq 1 ]; then
                resultado_parcial=`sumaFracciones $resultado_parcial "0/1" $element`
            else
                if [[ $element == *":"* ]]; then
                    IFS=':' read -a entero_split <<< $element
                    resultado_parcial=`sumaFracciones $resultado_parcial ${entero_split[1]} ${entero_split[0]}`
                else
                    resultado_parcial=`sumaFracciones $resultado_parcial $element "0"`
                fi
            fi
        done
        simplificada=`simplificarFraccion $resultado_parcial`
        echo $simplificada
        echo $simplificada > "./output/salida.out"
        return 0
    else
        echo "Archivo inválido"
        return -1
    fi
}

main(){
    case $1 in
        "-f")
            if [ ! -f "$2" ]; then
                echo "Resultado: El archivo $2 no existe"
            else
                resultado_operacion=`procesarArchivo $2`
                echo "Resultado: "$resultado_operacion
            fi
        ;;
        "-h" | "-?" | "-help")
            printf "Ayuda de sumar fracciones:
            \t-f path_archivo\t\tEjecuta la suma de las fracciones de un archivo indicado en path_archivo
            \t-h -? --help\t\t Mostrar ayuda del sumador de fracciones\n"
        ;;
        *)
            echo "Comando desconocido. Use -h para obtener ayuda"
        ;;
    esac
}

# Fin de funcionalidad

main $1 $2