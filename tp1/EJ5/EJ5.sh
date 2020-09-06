#! /bin/bash

function information()
{
	echo "
--------------------------------------------------
--------------------------------------------------
Nombre del script: EJ5.sh
Trabajo practico 1 - Ejercicio 5
--------------------------------------------------

Grupo:
MIRANDA SERGIO JAVIER - 35.634.266
CARRIL ARANDA JOSE - 37.120.410
LUNA SANCHEZ MATIAS LEONEL - 37.141.163
RAMIREZ MATIAS GASTON - 39.505.838
MASINI JOAN EZEQUIEL - 39.562.503

--------------------------------------------------

El script se encarga de generar un archivo a partir del ingresado por parametro.
El archivo generado tendra informacion de la cantidad de alumnos agrupados por materia.
La informacion resultante se mostrara respetando el siguiente orden, cada valor separado por comas:

Id de materia, cantidad de alumnos a final, cantidad de recursantes, cantidad de posibles a recuperar
y cantidad de alumnos que abandonaron la materia.


--------------------------------------------------
MODO DE USO
===========

./EJ5.sh <path file>
./EJ5.sh -f <path file>

Recuerde de ingresar la extension del archivo.

Ejemplo 1: ./EJ5.sh inputFIle.txt
Ejemplo 2: ./EJ5.sh -f inputFIle.txt
--------------------------------------------------
--------------------------------------------------

"
}


if [[ $# != 0 && (($1 == "-h") || ($1 == "-?") || ($1 == "-help") || ($1 == "--help") || ($1 == "-HELP") || ($1 == "--HELP")) ]]
then 
	information
else
	
	#----START VALIDACIONES ----#
	if [[ $# -eq 0 ]];
	then
	 echo -e "\nERR1- No se han ingresado parametros\n"
	 exit 0
	elif [[ $# -eq 2 ]];
	then
		if [[ $1 != "-f" ]];
		then
			echo -e "\nERR2- No se han ingresado los parametros correctos\n"
			exit 0
		elif [[ ! -f "${2}" ]];
		then
			echo -e "\nERR3- No se ha encontrado el directorio ingresado\n"
			exit 0
		else
	 		file=$(find $2);
		fi
	elif [[ $# -gt 2 ]];
	then
		echo -e "\nERR4- La cantidad de parametros es excesiva\n"
		exit 0
	elif [[ ! -f "${1}" ]];
	then
		echo -e "\nERR5- No se ha encontrado el directorio ingresado\n"
		exit 0
	else
	 file=$(find $1);
	
	fi


	#----END VALIDACIONES ----#

	echo -e "Ejecutando Ejercicio 5 ...\n------------------------";
	awk  '
	BEGIN {
	FS="|";
	}
	(NR == 1){
		/*print (NR". "$0 "<> este saltea");*/
		next;
	}
	//{ 
		
		materias[$2] = $2;
		/* print (NR". "$0); */
	}
	(NR > 1){
			if($7 == ""){//Si no tiene Final

				if( $6 == "")
				{//Si no tiene recuperatorio
					

					if(($3 == "") || ($4 == ""))
					{//Sin nota en al menos un parcial
						//Abandono
						abandonaron[$2]++;
						/*print (NR". "$0"<-  Este abandono");*/
						next;
					}
					else if(($3 < 4) && ($4 < 4))
					{//En ambos parciales te sacaste menos de 4
						//Recursa
						recursan[$2]++;
						/*print (NR". "$0"<---  Este recursa");*/
						next;
					}
					else if( (($3 < 7) || ($4 < 7)) && (($3 >= 7) || ($4 >= 7)) )
					{//Si tenes alguno menor a 7 y otro mayor a 7
						
						//Puede recuperar
						recuperan[$2]++;
						/*print (NR". "$0"<-----  Este recupera");*/
						next;
					}
					else if( (($3 < 4) || ($4 < 4)) && (($3 >= 4) || ($4 >= 4)) )
					{//Si tenes alguno menor a 4 y otro mayor a 4
						
						//Puede recuperar
						recuperan[$2]++;
						/*print (NR". "$0"<------  Este recupera");*/
						next;
					}
					else if( ($3 < 7) && ($4 < 7) )
					{//Si tenes ambos parciales menos de 7
						
						//Final
						aFinal[$2]++;
						/*print (NR". "$0"<----  Este final");*/
						next;
					}else
					{//En ambos parciales tenes 7 o mas
						//Aprobado		
						aprobados[$2]++;
						/*print (NR". "$0"<--  Este aprobo");*/
						next;			
					}

						

				}else //Cierra si no tiene recuperatorio
				{//Si tiene recuperatorio

					if( $6 < 4)
					{//Si saco menos de 4
						//Recursa
						recursan[$2]++;
						/*print (NR". "$0"<---  Este recursa");*/
						next;
					}
					else if( $6 >= 7)
					{//Si saco 7 o mas en recu
						
						if(($3 >= 7) || ($4 >= 7))
						{//En algun parcial tenes 7 o mas
							//Aprobo
							aprobados[$2]++;
							/*print (NR". "$0"<--  Este aprobo");*/
							next;
						}
						if(($3 >= 4) || ($4 >= 4))
						{//En algun parcial tenes 4 o mas
							//Final
							aFinal[$2]++;
							/*print (NR". "$0"<----  Este final");*/
							next;
						}else
						{//En ningun parcial tenes mas de 4.
							//Recursa	
							recursan[$2]++;	
							/*print (NR". "$0"<---  Este recursa");*/
							next;			
						}
						
					}else //Cierra 7 o mas en recu
					{//Si saco entre 4 y 6 en recu

						if(($3 >= 4) || ($4 >= 4))
						{//En algun parcial te sacaste mas de 4
							//Final
							aFinal[$2]++;
							/*print (NR". "$0"<----  Este final");*/
							next;
						}else
						{//En ningun parcial tenes mas de 4
							//Recursa
							recursan[$2]++;
							/*print (NR". "$0"<---  Este recursa");*/
							next;
						}

					}//Cierra 4y6 recu

				}//Cierra si tiene recuperatorio
			}else{//Si tiene final
				
				if( $7 < 4 )
				{
					//Recursa
					recursan[$2]++;
					/*print (NR". "$0"<---  Este recursa");*/
					next;
				}
				else
				{
					//Aprobo
					aprobados[$2]++;	
					/*print (NR". "$0"<--  Este aprobo");*/
					next;			
				}	
			}//Cierra si tiene final	


	}
	END { 
			print ("\"Materia\"," "\"Final\"," "\"Recursan\"," "\"Recuperan\"," "\"Abandonaron\"");		
			for(materia in materias)
			{	
				if(aFinal[materia] == "")
				{
					aFinal[materia] = 0;	
				}
				
				if(abandonaron[materia] == "")
				{
					abandonaron[materia] = 0;
				}

				if(recursan[materia] == "")
				{
					recursan[materia] = 0;	
				}

				if(recuperan[materia] == "")
				{
					recuperan[materia] = 0;
				}

				print ("\""materias[materia]"\"," "\""aFinal[materia]"\"," "\""recursan[materia]"\"," "\""recuperan[materia]"\"," "\""abandonaron[materia]"\"");
			}
	}' ${file} > output_file.txt;
	
	echo -e "\n El archivo de salida llamado 'output_file.txt'se ha guardado en la locacion actual del script.\n===============\nFin de ejercicio 5\n===============\n"
	
fi




