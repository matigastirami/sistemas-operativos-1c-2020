<#
	.SYNOPSIS

		En base a un archivo CSV
		El script se encarga de generar otro archivo CSV a partir del ingresado por parametro.
		El archivo generado tendra informacion de la cantidad de alumnos agrupados por materia.
		La informacion resultante sera exportada a un archivo CSV respetando el formato.

	.DESCRIPTION

	El script se encarga de generar un archivo a partir del ingresado por parametro.
	El archivo generado tendra informacion de la cantidad de alumnos agrupados por materia.
	La informacion resultante se mostrara respetando el siguiente orden, cada valor separado por comas:

	Id de materia, cantidad de alumnos a final, cantidad de recursantes, cantidad de posibles a recuperar
	y cantidad de alumnos que abandonaron la materia.
	
	El archivo resultante tendra el nombre de "output_file.csv" y se generara en el directorio donde se esta ejecutando el script.

	.EXAMPLE

		.\EJ5.ps1 -Nomina C:\PW\listado.csv

	.EXAMPLE

		./EJ5.ps1 -Nomina /home/matias/listado.csv
		

	.NOTES

	 Sistemas Operativos
	 --------------------
	 Trabajo Practico N2
	 Ejercicio 5
	 Script: EJ5.ps1
	 --------------------
	 Grupo 5 Integrantes:
		MIRANDA SERGIO JAVIER - 35.634.266
		CARRIL ARANDA JOSE - 37.120.410
		LUNA SANCHEZ MATIAS LEONEL - 37.141.163
		RAMIREZ MATIAS GASTON - 39.505.838
		MASINI JOAN EZEQUIEL - 39.562.503

#>

Param (  
	[Parameter(Position = 1, Mandatory = $true)] 
	[ValidateNotNullOrEmpty()]
	[String] $Nomina

)

	
if ([String]::IsNullOrEmpty($Nomina)) { 
	Write-Host "Parametro de ARCHIVO DE ENTRADA inexistente"
	exit 1;
}


if ([String]::IsNullOrWhiteSpace($Nomina)) { 
	Write-Host "Parametro de ARCHIVO DE ENTRADA inexistente"
	exit 1;
}


if (-not (Test-Path $Nomina)) { 
	Write-Host "Parametro de ARCHIVO DE ENTRADA inexistente"
	exit 1;
}

Clear-Host
$IsDirectory = (Get-Item $Nomina)-is [System.IO.DirectoryInfo]
	
if ($IsDirectory -eq $true) 
{
	Write-Host "Parametro de ARCHIVO DE ENTRADA invalido"
	Write-Host "Verificar la existencia de la ruta y el nombre del archivo"
	exit 1;
}

$studentList = Import-Csv -Path $Nomina -Delimiter '|'

Write-Host  "Ejecutando Ejercicio 5 ...`n------------------------";
$i = 0
$firstSignature = $false
$csvFileName = ".\output_file.csv"

[array]$materias = @()
$materiaList=@{}
$abandonaron=@{}
$recursan=@{}
$recuperan=@{}
$aFinal=@{}
$aprobados=@{}
Write-Host "Leyendo archivo..."
Write-Host "`n-------------------------------------------`n"
ForEach ($item in $studentList){ 

	Try
	{

		$i = $i + 1

		 if(! $materiaList.ContainsKey($item.idmateria))
		 {
			$materiaList[$item.idmateria]=1
		 }else{
			$materiaList[$item.idmateria]++
		 }
	
	
		if($item.final -eq  ""){# Si no tiene Final

			if( $item.recunota -eq  "")
			{# Si no tiene recuperatorio
				

				if(($item.primerparcial -eq "") -or ($item.segundoparcial -eq ""))
				{# Sin nota en al menos un parcial
					# Abandono
					if(! $abandonaron.ContainsKey($item.idmateria)){$abandonaron[$item.idmateria]=1}else{$abandonaron[$item.idmateria]++}
					Write-Host $item" <-  Este abandono"
					continue
				}
				elseif(([int]$item.primerparcial -lt 4) -and ([int]$item.segundoparcial -lt 4))
				{# En ambos parciales te sacaste menos de 4
					# Recursa
					if(! $recursan.ContainsKey($item.idmateria)){$recursan[$item.idmateria]=1}else{$recursan[$item.idmateria]++}
					Write-Host $item" <---  Este recursa"
					continue
				}
				elseif( (([int]$item.primerparcial -lt 7) -or ([int]$item.segundoparcial -lt 7)) -and (([int]$item.primerparcial -ge 7) -or ([int]$item.segundoparcial -ge 7)) )
				{# Si tenes alguno menor a 7 y otro mayor a 7
					
					# Puede recuperar
					if(! $recuperan.ContainsKey($item.idmateria)){$recuperan[$item.idmateria]=1}else{$recuperan[$item.idmateria]++}
					Write-Host $item" <-----  Este recupera"
					continue
				}
				elseif( (([int]$item.primerparcial -lt 4) -or ([int]$item.segundoparcial -lt 4)) -and (([int]$item.primerparcial -ge 4) -or ([int]$item.segundoparcial -ge 4)) )
				{# Si tenes alguno menor a 4 y otro mayor a 4
					
					# Puede recuperar
					if(! $recuperan.ContainsKey($item.idmateria)){$recuperan[$item.idmateria]=1}else{$recuperan[$item.idmateria]++}
					Write-Host $item" <-------  Este recupera"
					continue
				}
				elseif( (([int]$item.primerparcial -lt 7) -or ([int]$item.segundoparcial -lt 7)) -and (([int]$item.primerparcial -ge 7) -or ([int]$item.segundoparcial -ge 7)) )
				{# Si tenes alguno menor a 7 y otro mayor igual a 7
					
					# Puede recuperar
					if(! $recuperan.ContainsKey($item.idmateria)){$recuperan[$item.idmateria]=1}else{$recuperan[$item.idmateria]++}
					Write-Host $item" <-----  Este recupera"
					continue
				}
				elseif( ([int]$item.primerparcial -lt 7) -and ([int]$item.segundoparcial -lt 7) )
				{# Si tenes ambos parciales menos de 7
					
					# Final
					if(! $aFinal.ContainsKey($item.idmateria)){$aFinal[$item.idmateria]=1}else{$aFinal[$item.idmateria]++}
					Write-Host $item" <-----  Este final"
					continue
				}
				else
				{# En ambos parciales tenes 7 o mas
					# Aprobado		
					if(! $aprobados.ContainsKey($item.idmateria)){$aprobados[$item.idmateria]=1}else{$aprobados[$item.idmateria]++}
					Write-Host $item" <----  Este aprobo"
					continue			
				}

					

			}else # Cierra si no tiene recuperatorio
			{# Si tiene recuperatorio

				if( [int]$item.recunota -lt 4)
				{# Si saco menos de 4
					# Recursa
					if(! $recursan.ContainsKey($item.idmateria)){$recursan[$item.idmateria]=1}else{$recursan[$item.idmateria]++}
					Write-Host $item" <----  Este recursa"
					continue
				}
				elseif( [int]$item.recunota -ge 7)
				{# Si saco 7 o mas en recu
					
					if(([int]$item.primerparcial -ge 7) -or ([int]$item.segundoparcial -ge 7))
					{# En algun parcial tenes 7 o mas
						# Aprobo
						if(! $aprobados.ContainsKey($item.idmateria)){$aprobados[$item.idmateria]=1}else{$aprobados[$item.idmateria]++}
						Write-Host $item" <----  Este aprobo"
						continue
					}
					if(([int]$item.primerparcial -ge 4) -or ([int]$item.segundoparcial -ge 4))
					{# En algun parcial tenes 4 o mas
						# Final
						if(! $aFinal.ContainsKey($item.idmateria)){$aFinal[$item.idmateria]=1}else{$aFinal[$item.idmateria]++}
						Write-Host $item" <-----  Este final"
						continue
					}else
					{# En ningun parcial tenes mas de 4.
						# Recursa	
						if(! $recursan.ContainsKey($item.idmateria)){$recursan[$item.idmateria]=1}else{$recursan[$item.idmateria]++}
						Write-Host $item" <-----  Este recursa"
						continue			
					}
					
				}else # Cierra 7 o mas en recu
				{# Si saco entre 4 y 6 en recu

					if(([int]$item.primerparcial -ge 4) -or ([int]$item.segundoparcial -ge 4))
					{# En algun parcial te sacaste mas de 4
						# Final
						if(! $aFinal.ContainsKey($item.idmateria)){$aFinal[$item.idmateria]=1}else{$aFinal[$item.idmateria]++}
						Write-Host $item" <-----  Este final"
						continue
					}else
					{# En ningun parcial tenes mas de 4
						# Recursa
						if(! $recursan.ContainsKey($item.idmateria)){$recursan[$item.idmateria]=1}else{$recursan[$item.idmateria]++}
						Write-Host $item" <-----  Este recursa"
						continue
					}

				}# Cierra 4y6 recu

			}# Cierra si tiene recuperatorio
		}else{# Si tiene final
			
			if( [int]$item.final -lt 4 )
			{
				# Recursa
				if(! $recursan.ContainsKey($item.idmateria)){$recursan[$item.idmateria]=1}else{$recursan[$item.idmateria]++}
				Write-Host $item" <----  Este recursa"
				continue
			}
			else
			{
				# Aprobo
				if(! $aprobados.ContainsKey($item.idmateria)){$aprobados[$item.idmateria]=1}else{$aprobados[$item.idmateria]++}
				Write-Host $item" <----  Este aprobo"
				continue			
			}	
		}# Cierra si tiene final	
	}
	Catch
	{
		Write-Host  "Ha ocurrido un error`r`n";
		Write-Host  "Verifique la existencia de los mismos`r`nPresione cualquier tecla para continuar";
		Break
	}

}
Write-Host "`n-------------------------------------------`n"

ForEach ($itemMateria in $materiaList.GetEnumerator())
{ 

	if(! $aprobados.ContainsKey($itemMateria.Key)){$aprobados[$itemMateria.Key]=0}

	if(! $abandonaron.ContainsKey($itemMateria.Key)){$abandonaron[$itemMateria.Key]=0}

	if(! $recursan.ContainsKey($itemMateria.Key)){$recursan[$itemMateria.Key]=0}

	if(! $aFinal.ContainsKey($itemMateria.Key)){$aFinal[$itemMateria.Key]=0}

	if(! $recuperan.ContainsKey($itemMateria.Key)){$recuperan[$itemMateria.Key]=0}

	Write-Host  "`r`n"
	Write-Host  "Materia" $itemMateria.Key  
	Write-Host  "A Final" $aFinal[$itemMateria.Key] 
	Write-Host  "Recursan" $recursan[$itemMateria.Key] 
	Write-Host  "Recuperan" $recuperan[$itemMateria.Key] 
	Write-Host  "Abandonaron" $abandonaron[$itemMateria.Key]
	Write-Host  "Aprobados" $aprobados[$itemMateria.Key] 
	
	if(! $firstSignature){
		$firstSignature = $true
		New-Object -TypeName PSCustomObject -Property @{
		Materia = $itemMateria.Key  
		Final = $aFinal[$itemMateria.Key] 
		Recursan = $recursan[$itemMateria.Key] 
		Recuperan = $recuperan[$itemMateria.Key] 
		Abandonaron = $abandonaron[$itemMateria.Key] 
		} | Select-Object "Materia", "Final", "Recursan", "Recuperan", "Abandonaron" | Export-Csv -Path $csvFileName -NoTypeInformation
	}else{
		New-Object -TypeName PSCustomObject -Property @{
		Materia = $itemMateria.Key  
		Final = $aFinal[$itemMateria.Key] 
		Recursan = $recursan[$itemMateria.Key] 
		Recuperan = $recuperan[$itemMateria.Key] 
		Abandonaron = $abandonaron[$itemMateria.Key] 
		} | Select-Object "Materia", "Final", "Recursan", "Recuperan", "Abandonaron"  | Export-Csv -Path $csvFileName -NoTypeInformation -Append
	}

}

Write-Host "`n-------------------------------------------`n"
Write-Host "Total de alumnos: $i"

Write-Host  "`r`nEl archivo de salida llamado 'output_file.csv' se ha guardado en la locacion actual del script.`n=============== Fin de ejercicio 5 ===============`n"


	 