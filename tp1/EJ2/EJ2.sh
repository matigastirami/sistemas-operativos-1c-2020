#! /bin/bash

Ayuda(){
echo "Objetivo del script: leer todos los archivos de un directorio para realizar y mostrar por pantalla estadisticas de la mismas."
echo "parametros: -f y direccion del directorio"
echo "ejecucion: ./EJ2.sh -f llamadas"
}

if [ $# -eq 0 ] || [ $# -gt 2 ];then
 echo "no ingreso ningun parametro o ingreso mas parametros de lo esperado"
 exit 1
fi

if [ $# -eq 1 -a \( "$1" = "-h" -o "$1" = "-help" -o "1" = "-?" \) ];then
Ayuda
exit 1
else
if [ $# -eq 1 ];then
echo "ingreso solo un parametro"
exit 1
fi	
fi

if [ $1 = "-f" ] && [ -d "$2" ];then
for line in $(find $2 -name "*.log")
do
echo "////////////////////////////////////"
filename="${line##*/}"
echo "archivo:$filename"
echo " "
sort -k1,1 -k3 $line > ordenados.log
awk '{ print $1}' $line |sort -u   > fechas.log
awk '{ print $4}' $line |sort -u   > usuarios.log
band=0
llamadas=0

while IFS= read -r linea
do
if [ $band = "0" ]; then
  fecha1=${linea:0:10}
  tiempo1=${linea:11:8}
  nombre1=${linea:22}
  hora1=${linea:11:2}
  minutos1=${linea:14:2}
  segundos1=${linea:17:2}
  total1=$((hora1*3600+minutos1*60+segundos1)) 
  band=1
  else
  fecha2=${linea:0:10}
  tiempo2=${linea:11:8}
  nombre2=${linea:22}
  hora2=${linea:11:2}
  minutos2=${linea:14:2}
  segundos2=${linea:17:2}
  total2=$((hora2*3600+minutos2*60+segundos2))
  totales=$((total2-total1))
  hms=`date -d "1970-01-01 $totales sec" +"%H:%M:%S"`
  echo "$fecha2 $hms _ $nombre2"  >> totales.log
  band=0 
fi
done < ordenados.log
band2=0
sumprom=0
cantidad=1

echo " "
echo "Promedio de tiempo de las llamadas realizadas en un dia en segundos:"
while IFS= read -r fecha
do
fecha1=${fecha:0}
segundos1=`date --date "$fecha1" +%s`
sumprom=0
cantidad=0
while IFS= read -r linea
do
fecha2=${linea:0:10}
tiempo2=${linea:11:8}
hora2=${linea:11:2}
minutos2=${linea:14:2}
segundos2=${linea:17:2}
total2=$((hora2*3600+minutos2*60+segundos2))
segundos2=`date --date "$fecha2" +%s`
if [ "$segundos1" = "$segundos2" ]; then
sumprom=$((sumprom+total2))
cantidad=$((cantidad+1))
fi
done < totales.log
promedio=$((sumprom/cantidad))
echo "$fecha1 ---> $promedio en segundos"
echo "$fecha1 $promedio" >> promedios.log
done < fechas.log


echo "  "
echo "Promedio de tiempo y cantidad por usuario por dia:"
while IFS= read -r fecha
do
date=${fecha:0}
dates=`date --date "$fecha" +%s`
while IFS= read -r nombre
do
nombre1=${nombre:0}
sumprom=0
cantidad=0
while IFS= read -r linea
do
fecha2=${linea:0:10}
nombre2=${linea:22}
tiempo2=${linea:11:8}
hora2=${linea:11:2}
minutos2=${linea:14:2}
segundos2=${linea:17:2}
total2=$((hora2*3600+minutos2*60+segundos2))
segundos2=`date --date "$fecha2" +%s`
if [ "$dates" = "$segundos2" ] && [ "$nombre1" = "$nombre2" ]; then
sumprom=$((sumprom+total2))
cantidad=$((cantidad+1))
fi
done < totales.log
if [ "$cantidad" -ge "1" ];then
promedio=$((sumprom/cantidad))
echo "En la fecha $date $nombre llamo $cantidad veces con un promedio de $promedio segundos"
fi
done < usuarios.log
done < fechas.log

echo " "
echo "3 usuarios con mas llamadas a la semana"
while IFS= read -r nombre
do
nombre1=${nombre:0}
cantidad=0
while IFS= read -r linea
do
nombre2=${linea:22}
if [ "$nombre1" = "$nombre2" ];then
cantidad=$((cantidad+1))
fi 
done < totales.log
echo "$nombre1 $cantidad" >> maximo.log
done < usuarios.log

sort -r -k 2 maximo.log > maximo2.log

band3=0
while IFS= read -r maximo
do
if [ "$band3" -lt "3" ]; then
echo "$maximo"
band3=$((band3+1))
fi
done < maximo2.log


echo " "
echo "Llamadas que no superan la media por dia "
while IFS= read -r linea
do
fecha=${linea:0:10}
promedio=${linea:11}
segundos=`date --date "$fecha" +%s`
band=0
while IFS= read -r linea2
do
fecha2=${linea2:0:10}
nombre2=${linea2:22}
tiempo2=${linea2:11:8}
hora2=${linea2:11:2}
minutos2=${linea2:14:2}
segundos2=${linea2:17:2}
total2=$((hora2*3600+minutos2*60+segundos2))
segundos22=`date --date "$fecha2" +%s`
if [ "$segundos" = "$segundos22" ] && [ "$total2" -lt "$promedio" ];then
band=$((band+1))
fi
done <totales.log
echo "$fecha $band"
done < promedios.log

echo "Usuario que tiene mas llamadas por debajo de la media en la semana"
while IFS= read -r nombre
do
band=0
while IFS= read -r linea
do
fecha=${linea:0:10}
promedio=${linea:11}
segundos=`date --date "$fecha" +%s`
while IFS= read -r linea2
do
fecha2=${linea2:0:10}
nombre2=${linea2:22}
tiempo2=${linea2:11:8}
hora2=${linea2:11:2}
minutos2=${linea2:14:2}
segundos2=${linea2:17:2}
total2=$((hora2*3600+minutos2*60+segundos2))
segundos22=`date --date "$fecha2" +%s`
if [ "$nombre" = "$nombre2" ] && [ "$segundos" = "$segundos22" ] && [ "$total2" -lt "$promedio" ];then
band=$((band+1))
fi
done < totales.log
done < promedios.log
echo "$nombre $band" >> maximo3.log
done < usuarios.log

sort -r -k 2 maximo3.log > maximo4.log

band4=0
while IFS= read -r maximo
#cantidad=${maximo##* }
do
if [ "$band4" -lt "1" ] && [ ${maximo##* } -gt "0" ]; then
echo "$maximo"
band4=$((band4+1))
fi
done < maximo4.log
if [ "$band4" -eq "0" ];then
echo "no hay usuarios que no superan la media"
fi
rm fechas.log
rm maximo.log
rm maximo2.log
rm maximo3.log
rm maximo4.log
rm ordenados.log
rm promedios.log
rm totales.log
rm usuarios.log
done
else
echo "no ingreso como primer parametro -f o el directorio no existe"
fi
