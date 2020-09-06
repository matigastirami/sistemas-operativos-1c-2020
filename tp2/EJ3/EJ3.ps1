# ---------------------------------------------- #
# ---------------------------------------------- #
# Nombre del script: EJ3.ps1
# Trabajo practico N°2
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

<#
.Synopsis
El script se encarga de eliminar automáticamente los logs viejos cada vez que se crea un archivo nuevo.
.Description
El script se encarga de eliminar automáticamente los logs viejos cada vez que se crea un archivo nuevo. Evalúa todos los archivos del directorio y deja solamente los de la última semana de cada empresa.
Convención para los nombres de los logs: nombreEmpresa-númeroSemana.log
.Parameter Directorio
Este parámetro indicará el directorio a monitorear.
.Example
./EJ3.ps1 -Directorio logs
.Example
./EJ3.ps1 logs
#>

Param(
    [Parameter(Mandatory=$True)] 
    [ValidateNotNullOrEmpty()]
    [ValidateScript({
        if(-Not ($_ | Test-Path)){
            throw "El directorio no existe."
        }
        if(-Not ($_ | Test-Path -PathType Container)){
            throw "El argumento Directorio debe ser una carpeta. Las rutas de archivos no están permitidas."
        }
        return $true
    })]
    [String]$Directorio
)

$global:Directorio = $Directorio

Get-EventSubscriber -SourceIdentifier FileCreated -ErrorAction SilentlyContinue | Unregister-Event

$watcher = New-Object IO.FileSystemWatcher
$watcher.Path = $Directorio
$watcher.IncludeSubdirectories = $false
$watcher.EnableRaisingEvents = $true

#Evento de creacion de nuevo archivo
Register-ObjectEvent $watcher -EventName Created -SourceIdentifier FileCreated -Action {
    
    $REGEX_FILES = '(\w+)\-([0-9]+)\.log$'

    #Array de logs
    $files = Get-ChildItem -Path $global:Directorio | Where-Object -FilterScript {$_.Name -match $REGEX_FILES} | ForEach-Object {$_.Name} | Sort-Object -Descending
    
    #Cantidad de logs
    $cantidadLogs = $files.Length

    #Asignacion del tamaño de las arrays
    $empresas = @(0) * $cantidadLogs
    $semanasString = @(0) * $cantidadLogs

    #Carga de las arrays de empresas-semanas
    for($i = 0; $i -lt $cantidadLogs; $i++){
        $empresas[$i],$semanasString[$i] = $files[$i].replace('.log','').Split('-')
    }

    #Conversion de las semanas a enteros
    $semanas = $semanasString.foreach({[int]$_.trim()})

    #Inicializacion del array de logs a eliminar
    $antiguosLogs = @()

    $maxSem = $semanas[0]
    $empresaActual = $empresas[0]
    $pos = 0

    # Se guardan los logs con el numero de semana menor para luego ser eliminados
    for($i = 1; $i -lt $cantidadLogs; $i++){
        if ( $empresaActual.Equals($empresas[$i])){
            if($maxSem -gt $semanas[$i]){
                $antiguosLogs += $files[$i]
            }else{
                $antiguosLogs += $files[$pos]
                $maxSem = $semanas[$i]
                $pos = $i
            }
        }else{
            $empresaActual = $empresas[$i]
            $maxSem = $semanas[$i]
            $pos = $i
        }
    }

    foreach($log in $antiguosLogs){
        Remove-Item "$global:Directorio/$log"
    }
}