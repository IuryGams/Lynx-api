param (
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug"
)

Write-Host "==> Configurando projeto..."
cmake --preset crow_api

Write-Host "==> Buildando ($Config)..."
cmake --build build --config $Config

Write-Host "==> Rodando API..."
.\build\$Config\crow_api.exe