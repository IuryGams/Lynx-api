param (
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug"
)

Write-Host "==> Configurando projeto com preset crow_api..."
cmake --preset crow_api

Write-Host "==> Buildando ($Config)..."
if ($Config -eq "Debug") {
    cmake --build --preset debug
} else {
    cmake --build --preset release
}

$exePath = Join-Path -Path ".\build\$Config" -ChildPath "crow_api.exe"
Write-Host "==> Rodando API..."
& $exePath