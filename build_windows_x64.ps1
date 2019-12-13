$ErrorActionPreference = Stop

Clear-Host

$BuildDirectory = Join-Path $PSScriptRoot "build"
if (Test-Path -Path $BuildDirectory) {
    Remove-Item -Path $BuildDirectory -Force -Recurse -ErrorAction Stop
}

New-Item -Path $BuildDirectory -Force -ItemType Directory

Push-Location

Set-Location -Path $BuildDirectory
cmake.exe .. -A x64

$SolutionFilePath = (Get-ChildItem -Path $BuildDirectory -Filter *.sln | Select-Object -First 1).FullName
if (Test-Path -Path $SolutionFilePath) {
    Invoke-Item -Path $SolutionFilePath
}
else {
    Write-Error "Unable to find any generated solution in folder $BuildDirectory"
}

Pop-Location
