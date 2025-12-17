param (
    [switch]$NoPause
)

# Navigate to ..\python relative to where this script is saved
$PythonDir = Join-Path $PSScriptRoot "..\python"
Push-Location $PythonDir

if (Test-Path "Setup.py") {
    Write-Host "Running Python Setup..." -ForegroundColor Cyan
    py Setup.py
}
else {
    Write-Error "Setup.py not found in $PythonDir"
}

Pop-Location

if (-not $NoPause) {
    Write-Host "`nPress any key to continue..." -NoNewline
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}
