# build-all.ps1
# Build MD & MT on Windows for both SHARED/STATIC and Release/Debug
# Also build Win32 (x86) full set.
# Output structure (x64): build\x64\{MD|MT}\{bin,lib}\{Release|Debug}
# Output structure (Win32): build\Win32\{MD|MT}\{bin,lib}\{Release|Debug}
# Usage: run at repo root: powershell -ExecutionPolicy Bypass -File .\build-all.ps1

$ErrorActionPreference = "Stop"

function Invoke-Step([string]$cmd) {
  Write-Host ("==> " + $cmd) -ForegroundColor Cyan
  Invoke-Expression $cmd
}

# Optional: clean build folder (comment out if you want to keep it)
if (Test-Path -LiteralPath "build") {
  Write-Host "Cleaning build directory..." -ForegroundColor Yellow
  Remove-Item -Recurse -Force "build"
}

# Build matrix for both archs
$matrix = @(
  # x64
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MD"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MD"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MD"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MD"; TYPE="STATIC"; CFG="Debug"   },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MT"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MT"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MT"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="x64"; GEN="Visual Studio 17 2022"; AOPT="-A x64";  RT="MT"; TYPE="STATIC"; CFG="Debug"   },
  # Win32 (x86)
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MD"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MD"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MD"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MD"; TYPE="STATIC"; CFG="Debug"   },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MT"; TYPE="SHARED"; CFG="Release" },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MT"; TYPE="SHARED"; CFG="Debug"   },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MT"; TYPE="STATIC"; CFG="Release" },
  @{ ARCH="Win32"; GEN="Visual Studio 17 2022"; AOPT="-A Win32"; RT="MT"; TYPE="STATIC"; CFG="Debug"   }
)

foreach ($m in $matrix) {
  $arch = $m.ARCH
  $gen  = $m.GEN
  $aopt = $m.AOPT
  $rt   = $m.RT
  $type = $m.TYPE
  $cfg  = $m.CFG

  $bdir = "build\$arch"

  Write-Host "==== ARCH=$arch, BUILD_RUNTIME=$rt, NUT_LIBRARY_TYPE=$type, CONFIG=$cfg ====" -ForegroundColor Green

  Invoke-Step "cmake -S . -B `"$bdir`" -G `"$gen`" $aopt -DBUILD_RUNTIME=$rt -DNUT_LIBRARY_TYPE=$type"
  Invoke-Step "cmake --build `"$bdir`" --config $cfg"
}

Write-Host ""
Write-Host "Artifacts:" -ForegroundColor Yellow
$paths = @(
  'build\x64\MD\bin','build\x64\MD\lib','build\x64\MT\bin','build\x64\MT\lib',
  'build\Win32\MD\bin','build\Win32\MD\lib','build\Win32\MT\bin','build\Win32\MT\lib'
)
Get-ChildItem -Recurse -File -Include *.dll,*.lib,*.exp,*.exe -Path $paths | Select-Object FullName | Sort-Object FullName