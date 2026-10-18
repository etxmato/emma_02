@echo off
REM Build Emma 02 Windows installer - wx 3.3.3, all three arches (guest-side, runs on the Windows builder).
REM Usage: build_win.bat --arch arm/x64/x86 [--wx 3.3] [--src REPO_ROOT] [--out DIR] [--no-build]
REM   --arch arm : MSBuild Platform=ARM64, IsTool\Emma 02 arm.iss, gives emma_02_arm_windows.exe
REM   --arch x64 : MSBuild Platform=x64,   IsTool\Emma 02 x64.iss, gives emma_02_x64_windows.exe
REM   --arch x86 : MSBuild Platform=Win32,  IsTool\Emma 02 x86.iss, gives emma_02_x86_windows.exe
REM   --wx 3.3   : only wx 3.3 supported by this script (vc17 line); the 3.2/vc9 legacy line follows.
REM   --src      : repo root (default: parent of this script's dir)
REM   --out      : where to place the installer (default: %SRCROOT%)
REM   --no-build : skip MSBuild, package the exe already present
REM Env expected (baked into the VM image, system-wide): WXWIN, SDL, CURL, SERIALPORT.
REM Deps expected next to the tree per arch (WXLIBDIR/SDLLIBDIR/SPORTDIR in the table below):
REM   wx %WXWIN%\lib\vc_arm64_lib, vc_x64_lib, vc_lib\wxmsw33u_core.lib,
REM   ..\SDL2-2.0.5\lib\arm64, x64, x86\SDL2.dll,
REM   ..\libserialport\ARM64, Win64, Win32\lib\libserialport.dll.
setlocal EnableDelayedExpansion

set ARCH=
set WXVER=3.3
set SRCROOT=
set OUTDIR=
set BUILD=1

:parse
if "%~1"=="" goto doneparse
if "%~1"=="--arch" ( set "ARCH=%~2" & shift & shift & goto parse )
if "%~1"=="--wx" ( set "WXVER=%~2" & shift & shift & goto parse )
if "%~1"=="--src" ( set "SRCROOT=%~2" & shift & shift & goto parse )
if "%~1"=="--out" ( set "OUTDIR=%~2" & shift & shift & goto parse )
if "%~1"=="--no-build" ( set "BUILD=0" & shift & goto parse )
echo unknown arg: %~1 >&2
exit /b 1
:doneparse

if "%ARCH%"=="arm" goto arch_arm
if "%ARCH%"=="x64" goto arch_x64
if "%ARCH%"=="x86" goto arch_x86
echo --arch arm^|x64^|x86 required >&2
exit /b 1

:arch_arm
set PLATFORM=ARM64
set SLNPLATFORM=ARM64
set ISSFILE=IsTool\Emma 02 arm.iss
set EXEFILE=ARM64\Release\Emma 02.exe
set ISSOUT=IsTool\arm64\emma_02_arm_windows.exe
set ARTIFACT=emma_02_arm_windows.exe
set WXLIBDIR=vc_arm64_lib
set SDLLIBDIR=arm64
set SPORTDIR=ARM64
set VCREDIST=vc_redist.arm64.exe
goto arch_done

:arch_x64
set PLATFORM=x64
set SLNPLATFORM=x64
set ISSFILE=IsTool\Emma 02 x64.iss
set EXEFILE=x64\Release\Emma 02.exe
set ISSOUT=IsTool\x64\emma_02_x64_windows.exe
set ARTIFACT=emma_02_x64_windows.exe
set WXLIBDIR=vc_x64_lib
set SDLLIBDIR=x64
set SPORTDIR=Win64
set VCREDIST=vcredist_x64.exe
goto arch_done

:arch_x86
set PLATFORM=Win32
set SLNPLATFORM=x86
set ISSFILE=IsTool\Emma 02 x86.iss
set EXEFILE=Win32\Release\Emma 02.exe
set ISSOUT=IsTool\x86\emma_02_x86_windows.exe
set ARTIFACT=emma_02_x86_windows.exe
set WXLIBDIR=vc_lib
set SDLLIBDIR=x86
set SPORTDIR=Win32
set VCREDIST=vcredist_x86.exe
goto arch_done

:arch_done
if not "%WXVER%"=="3.3" ( echo --wx 3.3 required ^(vc17 line only^) >&2 & exit /b 1 )
if "%SRCROOT%"=="" set SRCROOT=%~dp0..
if "%OUTDIR%"=="" set OUTDIR=%SRCROOT%
cd /d "%SRCROOT%" || ( echo cannot cd to SRCROOT: %SRCROOT% >&2 & exit /b 1 )

REM 1. toolchain + baked deps present?
where MSBuild >nul 2>&1
if not errorlevel 1 set "MSBUILD=MSBuild"
if not defined MSBUILD (
  set "VSWHERE=!ProgramFiles(x86)!\Microsoft Visual Studio\Installer\vswhere.exe"
  if exist "!VSWHERE!" (
    for /f "delims=" %%M in ('"!VSWHERE!" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2^>nul') do set "MSBUILD=%%M"
  )
)
if not defined MSBUILD ( echo MSBuild not found - install VS 2022 Build Tools with x86+x64+ARM64 toolsets >&2 & exit /b 1 )
if not defined WXWIN ( echo WXWIN not set ^(want C:\wxWidgets-3.3.3^) >&2 & exit /b 1 )
if not exist "%WXWIN%\lib\%WXLIBDIR%\wxmsw33u_core.lib" ( echo missing wx 3.3.3 %PLATFORM% lib: %WXWIN%\lib\%WXLIBDIR%\wxmsw33u_core.lib >&2 & exit /b 1 )
if not exist "..\SDL2-2.0.5\lib\%SDLLIBDIR%\SDL2.dll" ( echo missing dep: ..\SDL2-2.0.5\lib\%SDLLIBDIR%\SDL2.dll >&2 & exit /b 1 )
if not exist "..\libserialport\%SPORTDIR%\lib\libserialport.dll" ( echo missing dep: ..\libserialport\%SPORTDIR%\lib\libserialport.dll >&2 & exit /b 1 )
if not exist "IsTool\%VCREDIST%" ( echo missing payload: IsTool\%VCREDIST% >&2 & exit /b 1 )
set "ISCC_EXE=!ProgramFiles(x86)!\Inno Setup 6\ISCC.exe"
if exist "!ISCC_EXE!" ( set "ISCC=!ISCC_EXE!" ) else ( set "ISCC=ISCC.exe" )
echo wx: 3.3.3  arch-target: %ARCH% ^(%PLATFORM%^)  src: %CD%

REM 2. compile (vc17 sln, Release|%SLNPLATFORM% - one ARM64 VM cross-builds all three;
REM    note: solution platform x86 maps to the Win32 project platform for the 32-bit target)
if "%BUILD%"=="1" (
  "%MSBUILD%" "Emma 02 vc17.sln" /p:Configuration=Release /p:Platform=%SLNPLATFORM% /m /v:m
  if errorlevel 1 ( echo MSBuild failed >&2 & exit /b 1 )
)
if not exist "%EXEFILE%" ( echo missing binary: %EXEFILE% ^(build first or drop --no-build^) >&2 & exit /b 1 )

REM 3. stamp AppVersion/AppVerName in the .iss from emma02_package.conf (transient, guest-only)
for /f "tokens=2 delims==" %%V in ('findstr "^VERSION=" "Pipeline\emma02_package.conf"') do set VER=%%V
if "%VER%"=="" ( echo cannot read VERSION from Pipeline\emma02_package.conf >&2 & exit /b 1 )
powershell -NoProfile -Command "(Get-Content '%ISSFILE%') -replace '^AppVerName=.*', 'AppVerName=Emma 02 - V%VER%' -replace '^AppVersion=.*', 'AppVersion=%VER%' | Set-Content '%ISSFILE%'"
if errorlevel 1 ( echo version stamp failed >&2 & exit /b 1 )
echo version: %VER% -^> %ISSFILE%

REM 4. package via Inno Setup 6
"%ISCC%" "%ISSFILE%"
if errorlevel 1 ( echo ISCC failed >&2 & exit /b 1 )
if not exist "%ISSOUT%" ( echo ISCC produced no installer: %ISSOUT% >&2 & exit /b 1 )

REM 5. publish version-free artifact (version lives in v^<version^> folder + AppVersion field)
copy /Y "%ISSOUT%" "%OUTDIR%\%ARTIFACT%" >nul
if errorlevel 1 ( echo copy to OUTDIR failed >&2 & exit /b 1 )
echo built: %OUTDIR%\%ARTIFACT%
dir "%OUTDIR%\%ARTIFACT%"
