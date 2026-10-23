@echo off
REM Build Emma 02 Windows installer - wx 3.3.3 modern + wx 3.2 XP + wx 2.8 w2k lines (guest-side).
REM Usage: build_win.bat --arch arm/x64/x86 [--wx 3.3|3.2|2.8] [--src REPO_ROOT] [--out DIR] [--no-build]
REM   --arch arm : MSBuild Platform=ARM64, IsTool\Emma 02 arm.iss, gives emma_02_arm_windows.exe (3.3 only)
REM   --arch x64 : 3.3 -> x64.iss/emma_02_x64_windows.exe | 3.2 -> x64_xp.iss/emma_02_x64_windows_xp.exe
REM   --arch x86 : 3.3 -> x86.iss/emma_02_x86_windows.exe | 3.2 -> x86_xp.iss/emma_02_x86_windows_xp.exe
REM            : 2.8 -> w2k.iss/emma_02_x86_windows_w2k.exe (x86 only, Win2000)
REM   --wx 3.3   : vc17 line (VS2022 MSBuild, wx 3.3.3, WXWIN) - arm/x64/x86
REM   --wx 3.2   : vc9 XP line (VS2008 vcbuild/devenv, wx 3.2, WX32) - x64/x86 only, packaged with Inno 5
REM   --wx 2.8   : vc9 w2k line (VS2008, wx 2.8.12, WX28, SDL 1.2, stripped manifest) - x86 only, Inno 5
REM   --src      : repo root (default: parent of this script's dir)
REM   --out      : where to place the installer (default: %SRCROOT%)
REM   --no-build : skip compile, package the exe already present
REM Env expected (baked into the VM image, system-wide): WXWIN, WX32, WX28, SDL, CURL, SERIALPORT.
REM Deps expected next to the tree per arch (WXLIBDIR/SDLLIBDIR/SPORTDIR in the table below):
REM   wx 3.3: %WXWIN%\lib\vc_arm64_lib, vc_x64_lib, vc_lib\wxmsw33u_core.lib,
REM   wx 3.2: %WX32%\lib\vc_x64_lib, vc_lib\wxmsw32u_core.lib (built via wx_vc9.sln,
REM     WXWIN is redirected to WX32 for the vc9 build since vc9.vcproj reads $(WXWIN)),
REM   ..\SDL2-2.0.5\lib\arm64, x64, x86\SDL2.dll (compiler-agnostic, shared),
REM   VC9 link lib: ..\libserialport\Win32-VS2008|Win64-VS2008\lib\release\libserialport.lib (vc9.vcproj
REM     links those dirs; plain Win32|Win64 trees belong to the vc17/VS2022 line),
REM   XP payload DLL: ..\libserialport\Win32-VS2008|Win64-VS2008\lib\release\libserialport.dll (VC9-built).
setlocal EnableDelayedExpansion
set "PF86=%ProgramFiles(x86)%"

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
if "%WXVER%"=="3.2" (
  set ISSFILE=IsTool\Emma 02 x64_xp.iss
  set ISSOUT=IsTool\x64\emma_02_x64_windows_xp.exe
  set ARTIFACT=emma_02_x64_windows_xp.exe
  set VCREDIST=vcredist9_x64.exe
) else (
  set ISSFILE=IsTool\Emma 02 x64.iss
  set ISSOUT=IsTool\x64\emma_02_x64_windows.exe
  set ARTIFACT=emma_02_x64_windows.exe
  set VCREDIST=vcredist_x64.exe
)
set EXEFILE=x64\Release\Emma 02.exe
set WXLIBDIR=vc_x64_lib
set SDLLIBDIR=x64
set SPORTDIR=Win64
goto arch_done

:arch_x86
set PLATFORM=Win32
set SLNPLATFORM=x86
if "%WXVER%"=="2.8" (
  set ISSFILE=IsTool\Emma 02 w2k.iss
  set ISSOUT=IsTool\x86\emma_02_x86_windows_w2k.exe
  set ARTIFACT=emma_02_x86_windows_w2k.exe
  set VCREDIST=vcredist9_x86.exe
) else if "%WXVER%"=="3.2" (
  set ISSFILE=IsTool\Emma 02 x86_xp.iss
  set ISSOUT=IsTool\x86\emma_02_x86_windows_xp.exe
  set ARTIFACT=emma_02_x86_windows_xp.exe
  set VCREDIST=vccrt.msi
) else (
  set ISSFILE=IsTool\Emma 02 x86.iss
  set ISSOUT=IsTool\x86\emma_02_x86_windows.exe
  set ARTIFACT=emma_02_x86_windows.exe
  set VCREDIST=vcredist_x86.exe
)
set EXEFILE=Win32\Release\Emma 02.exe
set WXLIBDIR=vc_lib
set SDLLIBDIR=x86
set SPORTDIR=Win32
goto arch_done

:arch_done
if "%WXVER%"=="3.3" goto line_vc17
if "%WXVER%"=="3.2" goto line_vc9
if "%WXVER%"=="2.8" goto line_vc9_w2k
echo --wx 3.3 ^(vc17 modern^), 3.2 ^(vc9 XP^) or 2.8 ^(vc9 w2k^) required >&2 & exit /b 1

:line_vc9_w2k
set "EXEFILE=%PLATFORM%-VC9\Release\Emma 02.exe"
if not "%ARCH%"=="x86" ( echo --wx 2.8 w2k line is x86 only >&2 & exit /b 1 )
if "%SRCROOT%"=="" set SRCROOT=%~dp0..
if "%OUTDIR%"=="" set OUTDIR=%SRCROOT%
cd /d "%SRCROOT%" || ( echo cannot cd to SRCROOT: %SRCROOT% >&2 & exit /b 1 )

REM 1. toolchain + baked deps present? (same VS2008 as the XP line, plus the
REM    wx2.8 tree, SDL 1.2 tree and stripped manifest)
REM    MSBuild 17 cannot build .vcproj - the w2k line needs VS2008 vcbuild/devenv.
where vcbuild >nul 2>&1
if not errorlevel 1 set "VCBUILD=vcbuild"
if not defined VCBUILD (
  set "DEVENV=!PF86!\Microsoft Visual Studio 9.0\Common7\IDE\devenv.com"
  if exist "!DEVENV!" set "VCBUILD=!DEVENV! /build"
)
if not defined VCBUILD (
  set "DEVENV2=!ProgramFiles!\Microsoft Visual Studio 9.0\Common7\IDE\devenv.com"
  if exist "!DEVENV2!" set "VCBUILD=!DEVENV2! /build"
)
if not defined VCBUILD (
  set "VCBUILD_EXE=!PF86!\Microsoft Visual Studio 9.0\VC\vcpackages\vcbuild.exe"
  if exist "!VCBUILD_EXE!" set "VCBUILD=!VCBUILD_EXE!"
)
if not defined VCBUILD ( echo VS2008 vcbuild/devenv not found - bake the XP layer first >&2 & exit /b 1 )
if not defined WX28 ( echo WX28 not set ^(want C:\wxWidgets-2.8^) >&2 & exit /b 1 )
if not exist "%WX28%\lib\%WXLIBDIR%\wxmsw28u_core.lib" ( echo missing wx 2.8 %PLATFORM% lib: %WX28%\lib\%WXLIBDIR%\wxmsw28u_core.lib >&2 & exit /b 1 )
if not exist "..\SDL-1.2.15\lib\%SDLLIBDIR%\SDL.dll" ( echo missing dep: ..\SDL-1.2.15\lib\%SDLLIBDIR%\SDL.dll >&2 & exit /b 1 )
if not exist "..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.lib" ( echo missing dep: ..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.lib ^(VC9 link lib^) >&2 & exit /b 1 )
if not exist "..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.dll" ( echo missing dep: ..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.dll ^(VC9 payload DLL^) >&2 & exit /b 1 )
if not exist "IsTool\%VCREDIST%" ( echo missing payload: IsTool\%VCREDIST% ^(VC9 vcredist, shared with XP line^) >&2 & exit /b 1 )
if not exist "Emma02_w2k.manifest" ( echo missing Emma02_w2k.manifest ^(stripped manifest^) >&2 & exit /b 1 )
set "ISCC_EXE=C:\Inno5\ISCC.exe"
if exist "!ISCC_EXE!" ( set "ISCC=!ISCC_EXE!" ) else ( set "ISCC=ISCC.exe" )
echo wx: 2.8-w2k ^(vc9^)  arch-target: %ARCH% ^(%PLATFORM%^)  src: %CD%

REM 2. compile (same vc9 sln, WXWIN redirected to the wx2.8 tree; the .rc embeds
REM    Emma02.manifest so swap in the stripped w2k manifest for this build only,
REM    restore afterwards. Build legacy before modern: output path collides.)
if "%BUILD%"=="1" (
  set "WXWIN_SAVED=%WXWIN%"
  set "WXWIN=%WX28%"
  copy /Y Emma02.manifest Emma02.manifest.saved >nul
  copy /Y Emma02_w2k.manifest Emma02.manifest >nul
  "!VCBUILD!" "Emma 02 vc9.sln" "Release|%PLATFORM%"
  set "VC9RC=!errorlevel!"
  copy /Y Emma02.manifest.saved Emma02.manifest >nul
  del Emma02.manifest.saved
  set "WXWIN=!WXWIN_SAVED!"
  set "WXWIN_SAVED="
  if not "!VC9RC!"=="0" ( echo vc9-w2k build failed >&2 & exit /b 1 )
)
if not exist "%EXEFILE%" ( echo missing binary: %EXEFILE% ^(build first or drop --no-build^) >&2 & exit /b 1 )
goto stamp

:line_vc9
set "EXEFILE=%PLATFORM%-VC9\Release\Emma 02.exe"
if "%ARCH%"=="arm" ( echo --wx 3.2 XP line is x86/x64 only ^(no ARM64 in VC9^) >&2 & exit /b 1 )
if "%SRCROOT%"=="" set SRCROOT=%~dp0..
if "%OUTDIR%"=="" set OUTDIR=%SRCROOT%
cd /d "%SRCROOT%" || ( echo cannot cd to SRCROOT: %SRCROOT% >&2 & exit /b 1 )

REM 1. toolchain + baked deps present?
REM    MSBuild 17 cannot build .vcproj - the vc9 line needs VS2008 vcbuild/devenv.
where vcbuild >nul 2>&1
if not errorlevel 1 set "VCBUILD=vcbuild"
if not defined VCBUILD (
  set "DEVENV=!PF86!\Microsoft Visual Studio 9.0\Common7\IDE\devenv.com"
  if exist "!DEVENV!" set "VCBUILD=!DEVENV! /build"
)
if not defined VCBUILD (
  set "DEVENV2=!ProgramFiles!\Microsoft Visual Studio 9.0\Common7\IDE\devenv.com"
  if exist "!DEVENV2!" set "VCBUILD=!DEVENV2! /build"
)
if not defined VCBUILD (
  set "VCBUILD_EXE=!PF86!\Microsoft Visual Studio 9.0\VC\vcpackages\vcbuild.exe"
  if exist "!VCBUILD_EXE!" set "VCBUILD=!VCBUILD_EXE!"
)
if not defined VCBUILD ( echo VS2008 vcbuild/devenv not found - bake the XP layer first >&2 & exit /b 1 )
if not defined WX32 ( echo WX32 not set ^(want C:\wxWidgets-3.2^) >&2 & exit /b 1 )
if not exist "%WX32%\lib\%WXLIBDIR%\wxmsw32u_core.lib" ( echo missing wx 3.2 %PLATFORM% lib: %WX32%\lib\%WXLIBDIR%\wxmsw32u_core.lib >&2 & exit /b 1 )
if not exist "..\SDL2-2.0.5\lib\%SDLLIBDIR%\SDL2.dll" ( echo missing dep: ..\SDL2-2.0.5\lib\%SDLLIBDIR%\SDL2.dll >&2 & exit /b 1 )
if not exist "..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.lib" ( echo missing dep: ..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.lib ^(VC9 link lib^) >&2 & exit /b 1 )
if not exist "..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.dll" ( echo missing dep: ..\libserialport\%SPORTDIR%-VS2008\lib\release\libserialport.dll ^(VC9 payload DLL^) >&2 & exit /b 1 )
if not exist "IsTool\%VCREDIST%" ( echo missing payload: IsTool\%VCREDIST% ^(VC9 vcredist for the XP line^) >&2 & exit /b 1 )
set "ISCC_EXE=C:\Inno5\ISCC.exe"
if exist "!ISCC_EXE!" ( set "ISCC=!ISCC_EXE!" ) else ( set "ISCC=ISCC.exe" )
echo wx: 3.2-XP ^(vc9^)  arch-target: %ARCH% ^(%PLATFORM%^)  src: %CD%

REM 2. compile (vc9 sln via vcbuild/devenv - XP-compatible CRT; build legacy before modern:
REM    vc9 writes Win32-VC9|x64-VC9 (own OutDir + IntDir) so it never shares objects with vc17 Win32|x64;
REM    note: vcbuild/devenv take the solution FIRST, then the configuration.
REM    vc9.vcproj reads $(WXWIN) - point it at the wx3.2 tree for this build only,
REM    restore afterwards so the modern line is unaffected.)
if "%BUILD%"=="1" (
  set "WXWIN_SAVED=%WXWIN%"
  set "WXWIN=%WX32%"
  "!VCBUILD!" "Emma 02 vc9.sln" "Release|%PLATFORM%"
  set "VC9RC=!errorlevel!"
  set "WXWIN=!WXWIN_SAVED!"
  set "WXWIN_SAVED="
  if not "!VC9RC!"=="0" ( echo vc9 build failed >&2 & exit /b 1 )
)
if not exist "%EXEFILE%" ( echo missing binary: %EXEFILE% ^(build first or drop --no-build^) >&2 & exit /b 1 )
goto stamp

:line_vc17
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

:stamp
REM 2b. --no-build freshness: the packaged exe must not be older than any source/project/resource file.
REM     Existence alone let a stale binary through (tonight's bug).
if "%BUILD%"=="0" (
  powershell -NoProfile -Command "$e=(Get-Item '%EXEFILE%').LastWriteTime; $n=Get-ChildItem -Path '%SRCROOT%' -Recurse -File -Include *.cpp,*.c,*.h,*.rc,*.manifest,*.vcproj,*.sln | Sort-Object LastWriteTime -Descending | Select-Object -First 1; if ($n -and $n.LastWriteTime -gt $e) { Write-Output ('stale binary %EXEFILE%: older than ' + $n.FullName + ' - rebuild without --no-build'); exit 1 }"
  if errorlevel 1 ( echo stale binary: %EXEFILE% ^(rebuild without --no-build^) >&2 & exit /b 1 )
)
REM 3. stamp AppVersion/AppVerName in the .iss from emma02_package.conf (transient, guest-only)
for /f "tokens=2 delims==" %%V in ('findstr "^VERSION=" "Pipeline\emma02_package.conf"') do set VER=%%V
if "%VER%"=="" ( echo cannot read VERSION from Pipeline\emma02_package.conf >&2 & exit /b 1 )
powershell -NoProfile -Command "(Get-Content '%ISSFILE%') -replace '^AppVerName=.*', 'AppVerName=Emma 02 - V%VER%' -replace '^AppVersion=.*', 'AppVersion=%VER%' | Set-Content '%ISSFILE%'"
if errorlevel 1 ( echo version stamp failed >&2 & exit /b 1 )
echo version: %VER% -^> %ISSFILE%

REM 4. package via Inno Setup (6 for the modern line, 5.6.1 for the XP line -
REM    Inno 6 output does not run on XP)
"%ISCC%" "%ISSFILE%"
if errorlevel 1 ( echo ISCC failed >&2 & exit /b 1 )
if not exist "%ISSOUT%" ( echo ISCC produced no installer: %ISSOUT% >&2 & exit /b 1 )

REM 5. publish version-free artifact (version lives in v^<version^> folder + AppVersion field)
copy /Y "%ISSOUT%" "%OUTDIR%\%ARTIFACT%" >nul
if errorlevel 1 ( echo copy to OUTDIR failed >&2 & exit /b 1 )
echo built: %OUTDIR%\%ARTIFACT%
dir "%OUTDIR%\%ARTIFACT%"
