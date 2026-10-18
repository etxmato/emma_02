# Verify the Windows builder toolchain for Emma 02 (guest-side).
$ErrorActionPreference = 'SilentlyContinue'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools'

Write-Output '--- MSVC toolsets ---'
Get-ChildItem "$vs\VC\Tools\MSVC" | ForEach-Object { Write-Output $_.Name }

Write-Output '--- cl.exe targets ---'
Get-ChildItem "$vs\VC\Tools\MSVC" -Recurse -Filter cl.exe |
  ForEach-Object { Write-Output $_.FullName }

Write-Output '--- Windows SDK versions ---'
Get-ChildItem 'C:\Program Files (x86)\Windows Kits\10\Include' |
  ForEach-Object { Write-Output $_.Name }

Write-Output '--- MSBuild ---'
Write-Output ("MSBuild: " + (Test-Path "$vs\MSBuild\Current\Bin\MSBuild.exe"))

Write-Output '--- Inno Setup ---'
Write-Output ("ISCC: " + (Test-Path 'C:\Program Files (x86)\Inno Setup 6\ISCC.exe'))

Write-Output '--- Deps on C: ---'
foreach ($p in @('C:\wxWidgets-3.3.3\build\msw\makefile.vc','C:\SDL2-2.0.5\lib\arm64\SDL2.dll','C:\libserialport\ARM64\lib\libserialport.dll','C:\curl-win\include\curl\curl.h')) {
  Write-Output ((Test-Path $p).ToString().PadRight(6) + $p)
}
