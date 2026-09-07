# build-all.ps1
# Build MD & MT on Windows for both SHARED/STATIC and Release/Debug.
# Usage: powershell -ExecutionPolicy Bypass -File .\build-all.ps1

$ErrorActionPreference = "Stop"
$rootDir = (Resolve-Path -LiteralPath $PSScriptRoot).Path

function Find-VsDevCmd {
  $candidates = @(
    "D:\Visual Studio 2026\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
  )

  foreach ($candidate in $candidates) {
    if ($candidate -and (Test-Path -LiteralPath $candidate)) {
      return $candidate
    }
  }

  throw "Visual Studio VsDevCmd.bat was not found."
}

function Import-VsEnvironment([string]$vsDevCmd, [string]$arch) {
  Write-Host "Loading Visual Studio environment: $arch" -ForegroundColor DarkCyan
  $lines = & cmd.exe /d /c "call `"$vsDevCmd`" -arch=$arch -host_arch=x64 >nul && set"
  if ($LASTEXITCODE -ne 0) {
    throw "Failed to load Visual Studio environment for $arch."
  }

  foreach ($line in $lines) {
    $separator = $line.IndexOf('=')
    if ($separator -gt 0) {
      Set-Item -Path "Env:$($line.Substring(0, $separator))" `
        -Value $line.Substring($separator + 1)
    }
  }
}

function Invoke-Step([string]$description, [scriptblock]$command) {
  Write-Host ("==> " + $description) -ForegroundColor Cyan
  & $command
  if ($LASTEXITCODE -ne 0) {
    throw "Step failed: $description"
  }
}

$vsDevCmd = Find-VsDevCmd
$ninjaPath = "D:\Visual Studio 2026\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
if (-not (Get-Command ninja.exe -ErrorAction SilentlyContinue)) {
  if (Test-Path -LiteralPath $ninjaPath) {
    $env:PATH = (Split-Path -Parent $ninjaPath) + ";" + $env:PATH
  }
  else {
    throw "ninja.exe was not found."
  }
}

$buildRoot = Join-Path $rootDir "build"
if (Test-Path -LiteralPath $buildRoot) {
  Write-Host "Cleaning build directory..." -ForegroundColor Yellow
  $resolvedBuildRoot = (Resolve-Path -LiteralPath $buildRoot).Path
  if ($resolvedBuildRoot -ne $buildRoot) {
    throw "Refusing to clean unexpected build path: $resolvedBuildRoot"
  }
  Remove-Item -LiteralPath $resolvedBuildRoot -Recurse -Force
}

$matrix = @(
  @{ ARCH="x64";  VSARCH="x64"; RT="MD"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="x64";  VSARCH="x64"; RT="MD"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="x64";  VSARCH="x64"; RT="MD"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="x64";  VSARCH="x64"; RT="MD"; TYPE="STATIC"; CFG="Debug"   },
  @{ ARCH="x64";  VSARCH="x64"; RT="MT"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="x64";  VSARCH="x64"; RT="MT"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="x64";  VSARCH="x64"; RT="MT"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="x64";  VSARCH="x64"; RT="MT"; TYPE="STATIC"; CFG="Debug"   },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MD"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MD"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MD"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MD"; TYPE="STATIC"; CFG="Debug"   },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MT"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MT"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MT"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="Win32"; VSARCH="x86"; RT="MT"; TYPE="STATIC"; CFG="Debug"   }
)

$loadedArch = ""
foreach ($m in $matrix) {
  if ($loadedArch -ne $m.VSARCH) {
    Import-VsEnvironment $vsDevCmd $m.VSARCH
    $loadedArch = $m.VSARCH
  }

  $buildDir = Join-Path $buildRoot "$($m.ARCH)\$($m.RT)-$($m.TYPE)-$($m.CFG)"
  Write-Host "==== ARCH=$($m.ARCH), BUILD_RUNTIME=$($m.RT), NUT_LIBRARY_TYPE=$($m.TYPE), CONFIG=$($m.CFG) ====" -ForegroundColor Green

  Invoke-Step "Configure $buildDir" {
    & cmake -S $rootDir -B $buildDir -G Ninja `
      "-DCMAKE_BUILD_TYPE=$($m.CFG)" `
      "-DBUILD_RUNTIME=$($m.RT)" `
      "-DNUT_LIBRARY_TYPE=$($m.TYPE)"
  }
  Invoke-Step "Build $buildDir" {
    & cmake --build $buildDir --parallel
  }
}

Write-Host ""
Write-Host "All configurations built successfully." -ForegroundColor Green
Get-ChildItem -LiteralPath $buildRoot -Recurse -File -Include *.dll,*.lib,*.exp,*.exe |
  Select-Object -ExpandProperty FullName |
  Sort-Object
