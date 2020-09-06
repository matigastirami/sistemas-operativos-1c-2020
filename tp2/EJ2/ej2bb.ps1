<#
.SYNOPSIS
Este script permite obtener informacion de un log de llamadas en la semana.
.DESCRIPTION
La informacion que podemos obtener:
Promedio de tiempo de las llamadas realizadas por dia
Promedio de tiempo y cantidad por usuario
Los 3 usuarios con mas llamadas en la semana
Cuantas llamadas no superan la media de tiempo por dia y el usuario con mas llamadas por debajo de la media
.PARAMETER path
Ruta del archivo
.EXAMPLE
/ej2.ps1 -path semana1.log	
#>

Param([Parameter(Mandatory=$true,Position=1)][ValidateNotNullOrEmpty()][string]$path)
$existe=test-path $path
if ($existe -eq $true){
$contenido= gc -path $path

foreach ($line in $contenido){
    $fechaynombre=$line.substring(0,$line.INdexOf("_")-1)|get-date -Format "yyyy-MM-dd HH:mm:sss"
    $nombre=$line.substring($line.INdexOf("_")+2)
    write-output "$fechaynombre _ $nombre" >> nuevo.log
}

#obtengo fechas
$fechas= new-object system.collections.generic.list[system.object]
foreach($linea in gc .\nuevo.log) {
	$fecha=$linea.Substring(0,10)
    if(!$fechas.contains($fecha)){
		$fechas.add($fecha)
    }      
}
#$fechas

#obtengo nombres
$nombres= new-object system.collections.generic.list[system.object]
foreach($linea in gc .\nuevo.log) {
	$nombre=$linea.Substring(22)
    if(!$nombres.contains($nombre)){
		$nombres.add($nombre)
    }
}

#$nombres| format-list -property fechas

#ordeno llamadas
for ($i=0;$i -lt $fechas.count;$i++){
	for($j=0;$j -lt $nombres.count;$j++){
	#write-host "$fecha ---------------"
		foreach($linea in gc .\nuevo.log){
			$fecha=$linea.Substring(0,10)
			$hora=$linea.Substring(11,8)
			$nombre=$linea.Substring(22)
			$fecha1= $fechas[$i]|get-date -format dd/MM/yyyy
			$fecha2= $fecha|get-date -format dd/MM/yyyy
			if ( $fecha1 -eq $fecha2 -and $nombres[$j] -eq $nombre){
				write-output "$fecha $hora $nombre" >> ordenados.log 
			}
		}
	}
}
write-host "LLAMADAS ORDENADAS-----------"
foreach($linea in gc .\ordenados.log){
write-host $linea
}

#calculo el tiempo de la llamada
$f= new-object system.collections.generic.list[system.object]
$n= new-object system.collections.generic.list[system.object]
$s= new-object system.collections.generic.list[system.object]
$band=0
foreach($linea in gc .\ordenados.log){
	if($band -eq 0){
		$fecha=$linea.Substring(0,10)
		$hora=$linea.Substring(11,8)
		$nombre=$linea.Substring(20)
		$hh=$linea.Substring(11,2)
		$mm=$linea.Substring(14,2)
		$ss=$linea.Substring(17,2)
		$segundos=([int]$hh*3600+[int]$mm*60+[int]$ss)
		#write-host "$nombre"
		$band=1	
	}else{
		$fecha2=$linea.Substring(0,10)
		$hora2=$linea.Substring(11,8)
		$nombre2=$linea.Substring(20)
		$hh1=$linea.Substring(11,2)
		$mm1=$linea.Substring(14,2)
		$ss1=$linea.Substring(17,2)
		$segundos1=([int]$hh1*3600+[int]$mm1*60+[int]$ss1)
		$segundostotales= $segundos1 - $segundos
		$f.add($fecha2)
		$n.add($nombre2)
		$s.add($segundostotales)
		$band=0
	}
}

write-host "TIEMPO LLAMADA---------------"
$band2=1
for($i=0;$i -lt $f.count;$i++){
	write-host "llamada $band2 :"$f[$i] $n[$i] $s[$i]"segundos"
	$band2++ 
}

write-host "------------------------------------------------"
write-host "1)CONSIGNA:PROMEDIO DE TIEMPO DE LLAMADAS POR DIA"
$lista=@()
$datoslista=@{"Fecha" ="";"Promedio" =""}
for($i=0; $i -lt $fechas.count;$i++){
	$sum=0
	$cont=0
	for($j=0; $j -lt $f.count;$j++){
		$fecha1= $fechas[$i]|get-date -format dd/MM/yyyy
		$fecha2= $f[$j]|get-date -format dd/MM/yyyy
		if($fecha1 -eq $fecha2){
			$sum=$sum+$s[$j]
			$cont++
		}
	}
	$promedio= $sum / $cont
	$datos= New-Object -TypeName psobject -Property $datoslista
	$datos.Fecha=$fecha1
	$datos.Promedio=$promedio
	$lista += $datos
	#write-host "$promedio"
}
$lista | Format-list Fecha,Promedio  
write-host "   "
write-host "2)CONSIGNA:PROMEDIO DE TIEMPO Y CANTIDAD POR USUARIO POR DIA"
$lista2=@()
$datoslista2=@{"Nombre" ="";"Fecha" ="";"Cantidad"="";"Promedio"=""}
for($i=0;$i -lt $nombres.count;$i++){
	for($j=0;$j -lt $fechas.count;$j++){
		$count=0
		$sum=0
		for($k=0;$k -lt $f.count;$k++){
			$fecha1= $fechas[$j]|get-date -format dd/MM/yyyy
			$fecha2= $f[$k]|get-date -format dd/MM/yyyy
			$nombre1= [string]$nombres[$i]
			$nombre2= [string]$n[$k]
			#write-host $nombre1$nombre2 
			if($nombre1.equals($nombre2) -and $fecha1 -eq $fecha2){
				#Write-host "entro"
				$sum= $sum + $s[$k] 
				$count++
			}
		}
		if($count -ne 0){
			$promedio= $sum/$count
			#write-host $nombres[$i] $fecha1 $count $promedio
			$datos= New-Object -TypeName psobject -Property $datoslista2
			$datos.Nombre=$nombres[$i]
			$datos.Fecha=$fecha1
			$datos.Cantidad=$count
			$datos.Promedio=$promedio
			$lista2+=$datos
		}
	}
}
$lista2|Format-list Nombre,Fecha,Cantidad,Promedio
write-host " "
write-host "3)CONSIGNA:LOS 3 USUARIOS CON MAS LLAMADAS A LA SEMANA "
$cantidad=@{}
$lista3=@()
$datoslista3=@{"Nombre" ="";"cantLlamadas" =""}
for($i=0;$i -lt $nombres.count;$i++){
	$cont=0
	for($j=0;$j -lt $n.count;$j++){
		if($nombres[$i] -eq $n[$j]){
			$cont++
		}
	}
	$cantidad.add($nombres[$i],$cont)
	$datos= New-Object -TypeName psobject -Property $datoslista3
	$datos.Nombre=$nombres[$i]
	$datos.cantLlamadas=$cont
	$lista3+=$datos
}
$lista3|sort-object -Property cantLlamadas -Descending|select-object -index 0, 1, 2|format-list -property cantLlamadas,Nombre
write-host " "
write-host "4)CONSIGNA:CANT DE LLAMADAS QUE NO SUPERAN LA MEDIA POR DIA Y USUARIO QUE TIENE MAS LLAMADAS POR DIA"
$lista4=@()
$datoslista4=@{"Fecha" ="";"cantLlamadas" =""}
$contadorLlamadas=0
foreach($linea in $lista){
	$cont=0
	$fecha1= $linea.Fecha|get-date -format dd/MM/yyyy
	for($i=0;$i -lt $f.count;$i++){
		$fecha2= $f[$i]|get-date -format dd/MM/yyyy
		if($fecha1 -eq $fecha2 -and $s[$i] -lt $linea.Promedio ){
			$cont++
			$contadorLlamadas++
		}
	}
	$datos= New-Object -TypeName psobject -Property $datoslista4
	$datos.Fecha=$fecha1
	$datos.cantLlamadas=$cont
	$lista4+=$datos
}
$lista4|format-list -property Fecha,cantLlamadas
write-host "total de llamadas que no superan la media: $contadorLlamadas"
$lista5=@()
$datoslista5=@{"Nombre" ="";"cantLlamadas" =""}
for($i=0;$i -lt $nombres.count;$i++){
	$cont=0
	for($j=0;$j -lt $f.count;$j++){
		$fecha1= $f[$j]|get-date -format dd/MM/yyyy
		foreach($linea in $lista){
			$fecha2= $linea.Fecha|get-date -format dd/MM/yyyy
			if($nombres[$i] -eq $n[$j] -and $fecha1 -eq $fecha2 -and $s[$j] -lt $linea.Promedio){
				$cont++
			}
		}
	}
	$datos= New-Object -TypeName psobject -Property $datoslista5
	$datos.Nombre=$nombres[$i]
	$datos.cantLlamadas=$cont
	$lista5+=$datos
}
write-host " "
write-host "usuario que tiene mas llamadas por debajo de la media:"
$band3=0
foreach($line in $lista5){
	if($line.cantLlamadas -gt 0){
		$band3++
	}
}
if($band3 -eq 0){
	write-host "no hay usuarios que tengan llamadas por debajo de la media"
}else{
	$lista5|sort-object -Property cantLlamadas -Descending|select-object -index 0|format-list -property Nombre,cantLlamadas
}

rm ordenados.log 
rm nuevo.log
}else{
write-host "el archivo no existe..."
}

