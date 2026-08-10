# Navigate to ..\python relative to where this script is saved
$PythonDir = Join-Path $PSScriptRoot ".\python"
Push-Location $PythonDir

if (Test-Path "Setup.py")
{
    python Setup.py @args
}
else
{
    Write-Error "Setup.py not found in $PythonDir"
}

Pop-Location
