[CmdletBinding()]
param(
    [string]$MtascPath = "mtasc",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$root = $PSScriptRoot
$output = Join-Path $root "bin\DurabilityMenu.swf"
$template = Join-Path $root "assets\DurabilityMenu.template.swf"

if ($Clean -and (Test-Path -LiteralPath $output)) {
    Remove-Item -LiteralPath $output
}

if (-not (Test-Path -LiteralPath $template)) {
    throw "Missing asset template: $template"
}

New-Item -ItemType Directory -Force -Path (Split-Path -Parent $output) | Out-Null
Copy-Item -LiteralPath $template -Destination $output -Force

Push-Location (Join-Path $root "src")
try {
    & $MtascPath `
        -cp "." `
        -version 8 `
        -swf $output `
        "Main.as"

    if ($LASTEXITCODE -ne 0) {
        throw "MTASC failed with exit code $LASTEXITCODE."
    }
} finally {
    Pop-Location
}

Write-Host "Built $output"
