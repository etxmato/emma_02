@echo off
REM Build wxWidgets 3.3.3 static release libs for all three targets (guest-side, Emma 02 builder).
REM Uses Hostarm64 toolsets (native host) cross-building x86/x64, plus native arm64.
REM Dynamic CRT (default; do NOT pass RUNTIME_LIBS=static) to match the vc17 project.
setlocal EnableExtensions

set WXROOT=C:\wxWidgets-3.3.3
set VCALL="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"

call :build WIN32 arm64_x86
if errorlevel 1 exit /b 1
call :build X64 arm64_x64
if errorlevel 1 exit /b 1
call :build ARM64 arm64
if errorlevel 1 exit /b 1

echo --- verifying wx libs ---
for %%L in (vc_lib vc_x64_lib vc_arm64_lib) do (
  if exist "%WXROOT%\lib\%%L\wxmsw33u_core.lib" (echo OK %%L) else (echo MISSING %%L)
)
echo WX_BUILD_DONE
exit /b 0

:build
echo ============================================
echo === wx TARGET_CPU=%1  (vcvars %2) ===
echo ============================================
call %VCALL% %2
if errorlevel 1 ( echo VCVARS-FAIL %1 & exit /b 1 )
cd /d "%WXROOT%\build\msw" || ( echo CD-FAIL & exit /b 1 )
nmake /f makefile.vc TARGET_CPU=%1 BUILD=release CFLAGS=/WX- LDFLAGS=/MANIFEST:NO
if errorlevel 1 ( echo NMAKE-FAIL %1 & exit /b 1 )
echo === done %1 ===
exit /b 0
