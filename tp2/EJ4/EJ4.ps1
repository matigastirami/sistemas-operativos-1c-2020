# ---------------------------------------------- #
# ---------------------------------------------- #
# Nombre del script: EJ4.ps1
# Trabajo practico N°2
# Ejercicio: 4
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
El script se encarga de mover automaticamente los logs viejos a un zip por cada empresa cada vez que se ejecuta el script.
.Description
El script se encarga de mover automaticamente los logs viejos a un zip cada vez que se ejecuta el script. Evalua todos los archivos o los de la empresa elegida y deja solamente los de la ultima semana y el resto los agrega a un zip por empresa.
Convencion para los nombres de los logs: nombreEmpresa-numeroSemana.log
.Parameter Directorio
Este parametro indica el directorio en el que se encuentran los archivos de log. Puede ser una ruta relativa o absoluta.
.Parameter DirectorioZip
Este parametro indica el directorio en el que se generaran los archivos comprimidos de los clientes.
.Parameter Empresa
Este parametro indica el Nombre de la empresa a procesar.(opcional)
.Example
./EJ4.ps1 -Directorio logs -DirectorioZip zips
.Example
./EJ4.ps1 -Directorio logs -DirectorioZip zips -Empresa empresa1-1.log
#>


Param(
    [Parameter(Mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript( {
            If (Test-Path $_) { $true }else { Throw "Invalid path given: $_" }
        })]
    [string]$Directorio,
    [Parameter(Mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript( {
            If (Test-Path $_) { $true }else { Throw "Invalid path given: $_" }
        })]
    [string]$DirectorioZip,
    [Parameter(Mandatory = $False)]
    [string]$Empresa
)

if (!$Empresa) { $Empresa = '\w' }

$REGEX_FILES = "($Empresa+)\-([0-9]+)\.log$"
#Array de logs
$files = Get-ChildItem -Path $Directorio | Where-Object -FilterScript { $_.Name -match $REGEX_FILES } | ForEach-Object { $_.Name }

$map = @{ }

foreach ($file in $files) {
    $file = $file.Substring( 0 , $file.IndexOf("-"))
    if (!$map.ContainsKey($file)) { $map.Add( $file , "") }
}

foreach ($key in $map.Keys) {
    $names = Get-ChildItem -Path $Directorio | Where-Object -FilterScript { $_.Name -match "($key+)\-([0-9]+)\.log$" } | ForEach-Object { $_.Name } | Sort-Object { [regex]::Replace($_, '\d+', { $args[0].Value.Padleft(20) }) } -Descending
    $array = $names.split(" ")
    $max = $array[0]

    foreach ($arch in $array) {
        if ( $arch -ne $max ) {
            if ( Test-Path "$DirectorioZip/$key.zip" -PathType leaf ) {
                Compress-Archive -Update -DestinationPath "$DirectorioZip/$key.zip" -Path "$Directorio/$arch" -CompressionLevel "Fastest"
            }
            else {
                Compress-Archive -DestinationPath "$DirectorioZip/$key.zip" -Path "$Directorio/$arch" -CompressionLevel "Fastest"
            }
            Remove-Item "$Directorio/$arch"
            Write-Host "El archivo : [$arch] ha sido movido al zip : [$key.zip]"
        }
    }
}
