Param (
[Parameter(Position = 1, Mandatory = $false)]
[String] $pathsalida = ".\procesos.txt ",
[int] $cantidad = 3
)
$existe = Test-Path $pathsalida
if ($existe -eq $true) {
$listaproceso = Get-Process
foreach ($proceso in $listaproceso) {
$proceso | Format-List -Property Id,Name >> $pathsalida
}
for ($i = 0; $i -lt $cantidad ; $i++) {
Write-Host $listaproceso[$i].Name - $listaproceso[$i].Id
}
} else {
Write-Host "El path no existe"
}

<#
Responder:
1. ¿Cuál es el objetivo del script?
	El objetivo del script es mostrar por pantallas el name y el id de cierta cantidad de procesos activos  que previamente fueron grabados en un archivo y que la cantidad mostrada depende del parametro $cantidad. 
2. ¿Agregaría alguna otra validación a los parámetros?
	validar que la $cantidad no sea igual que cero o menor que cero.
3. ¿Qué sucede si se ejecuta el script sin ningún parámetro?
	Si se ejecuta el script sin parametros mostraria un error de que el path no existe.
#>
