<#
.Synopsis
Sumar fracciones a partir de un archivo de entrada
.Description
El script recibe un archivo con una línea de fracciones separadas por comas (Ejemplo: 16/8,8/9,1:5/8,10:9/2,-7/5) y retorna el resultado expresado como fracción por pantalla, y guarda ese resultado en un archivo del directorio ./output
.Parameter -Path
Este parámetro indicará el directorio en donde se encuentra el archivo.
.Example
./EJ6.ps1 -Path ./test_cases/test_cases.txt
.Notes
Nombre del script: EJ6.ps1
Trabajo practico: 2
Ejercicio: 6
Numero de entrega: 1 (entrega)
Grupo:
  MIRANDA SERGIO JAVIER - 35.634.266
  CARRIL ARANDA JOSE - 37.120.410
  LUNA SANCHEZ MATIAS LEONEL - 37.141.163
  RAMIREZ MATIAS GASTON - 39.505.838
  MASINI JOAN EZEQUIEL - 39.562.503
#>

Param (  
	[Parameter(Position = 1, Mandatory = $true)] 
	[ValidateNotNullOrEmpty()]
	[String] $Path

)

function Get-GCD ([int]$x, [int]$y)
{
  if ($x -eq $y) { return $y }
  if ($x -gt $y) {
    $a = $x
    $b = $y
  }
  else {
    $a = $y
    $b = $x
  }
  while ($a % $b -ne 0) {
    $tmp = $a % $b
    $a = $b
    $b = $tmp
  }
  return $b
}

function Get-LCM([int]$a, [int]$b) {
    if ($a -eq 0 -or $b -eq 0) {
        return 0
    }

    return ($a * $b) / (Get-GCD $a $b)
}

function Write-HelpText
{
    Write-Output("Ayuda de sumar fracciones:`n-f path_archivo : Ejecuta la suma de las fracciones de un archivo indicado en path_archivo`n-h -? --help : Mostrar ayuda del sumador de fracciones")
}

function Sum-Fractions ([string]$f1, [string]$f2, [int]$integer)
{
    $split_f1 = ($f1 -split "/")
    $split_f2 = ($f2 -split "/")
    if($integer -lt 0) {
        $split_f2[0] = -$split_f2[0]
    }
    $lcm = (Get-LCM $split_f1[1] $split_f2[1])
    return "$($lcm / $split_f1[1] * $split_f1[0] + $lcm / $split_f2[1] * $split_f2[0] + $lcm * $integer)/$lcm"
}

function Simplify-Fraction([string] $fraction)
{
    $split_fraction = ($fraction -split "/")
    $gcd = (Get-GCD $split_fraction[0] $split_fraction[1])
    # Iterate until GCD is 1
    while($gcd -gt 1)
    {
        $fraction = "$($split_fraction[0]/$gcd)/$($split_fraction[1]/$gcd)"
        $split_fraction = ($fraction -split "/")
        $gcd = (Get-GCD $split_fraction[0] $split_fraction[1])
    }
    # If denominator is 1, return only numerator, example: if result = 2/1, return 2.
    if($split_fraction[1] -eq 1)
    {
        return $split_fraction[0]
    }

    return $fraction
}

function Process-File([string] $file_name)
{
    $result="0/1"
    $line = (Get-Content $file_name)
    # Verify validity
    if($line.Trim() -match '^(\-{0,}([0-9]{0,}:{0,}[0-9]{1,}\/[0-9]{1,}|\d{1,})\,{0,}){1,}$')
    {
        # Get fractions array
        $fractions = ($line -split ",")
        foreach ($item in $fractions)
        {
            # split into  denominator and numerator
            $fraction = ($item -split "/")
            if($fraction.count -eq 1){ #If it's an integer
                $result = (Sum-Fractions $result "0/1" $item)
            }
            else {
                $numerator = $fraction[0]
                $denominator = $fraction[1]
                # Verify is it has integer part
                if($numerator -Match ":")
                {
                    # Split into fraction and integer
                    $integer_split = ($item -split ":")
                    $result = (Sum-Fractions $result $integer_split[1] $integer_split[0])
                }
                else 
                {
                    $result = (Sum-Fractions $result $item 0)
                }
            }
            
        }
    }
    else
    {
        return "Archivo inválido"
    }

    return (Simplify-Fraction $result)
}

if ([String]::IsNullOrEmpty($Path)) { 
	Write-Host "1 Debe indicar un archivo válido en el parámetro -Path"
	exit 1;
}

if (-not (Test-Path $Path -PathType leaf)) { 
	Write-Host "3 Debe indicar un archivo válido en el parámetro -Path"
	exit 1;
}

$result = (Process-File $Path)
Out-File -FilePath salida.out -InputObject $result
Write-Output "Resultado: $result"

#Fin de funcionalidad