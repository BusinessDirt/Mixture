<#
.SYNOPSIS
    Runs premake5 to generate project files.
.EXAMPLE
    .\Win-GenProjects.ps1 -Target vs2019
    .\Win-GenProjects.ps1 -Target vs2022 -NoPause
#>
param (
    [string]$Target = "vs2026", # Default to vs2026 if not specified
    [switch]$NoPause            # Use this switch to skip the pause at the end
)

# Set the location to the script's directory, then move up two levels
# Resolve-Path ensures we get the absolute path
$RootPath = Resolve-Path "$PSScriptRoot\..\..\"
Push-Location $RootPath

$PremakePath = "vendor\premake\bin\premake5.exe"

if (Test-Path $PremakePath) {
    Write-Host "Generating project files for target: $Target" -ForegroundColor Cyan

    # The '&' operator executes the command string
    & $PremakePath $Target
}
else {
    Write-Error "Could not find premake5.exe at $PremakePath"
}

# Return to the original directory
Pop-Location

# Pause logic equivalent
if (-not $NoPause) {
    Write-Host "`nPress any key to continue..." -NoNewline
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}
