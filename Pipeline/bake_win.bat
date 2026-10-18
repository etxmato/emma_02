@echo off
REM One-shot bake script for the emma02-win builder VM (run ONCE per fresh VM).
REM Run each section from the matching "Native Tools Command Prompt for VS 2022"
REM (x86 for WIN32, x64 for X64, ARM64 for ARM64) - or run the whole file three
REM times, once per prompt. Requires: VS 2022 Build Tools (x86+x64+ARM64 toolsets),
REM Inno Setup 6, wxWidgets-3.3.3 source, SDL2-2.0.5 + libserialport + curl-win trees
REM as siblings of emma_02 (same layout as Marcel's Windows PC), OpenSSH server.
REM After this, set system env WXWIN/SDL/CURL/SERIALPORT (see bottom) and snapshot.
setlocal

REM ---- 0. where are we? ----
echo WXWIN=%WXWIN%
echo SDL=%SDL%
echo CURL=%CURL%
echo SERIALPORT=%SERIALPORT%
if not defined WXWIN ( echo SET WXWIN first, e.g. C:\wxWidgets-3.3.3 >&2 & exit /b 1 )
if not defined SDL ( echo SET SDL first, e.g. C:\SDL2-2.0.5 >&2 & exit /b 1 )
if not defined CURL ( echo SET CURL first, e.g. C:\curl-win >&2 & exit /b 1 )
if not defined SERIALPORT ( echo SET SERIALPORT first, e.g. C:\libserialport >&2 & exit /b 1 )

REM ---- 1. wxWidgets 3.3.3 (dynamic CRT default - do NOT pass RUNTIME_LIBS=static) ----
REM Pick ONE line matching THIS prompt:
REM   x86 prompt:   nmake /f makefile.vc TARGET_CPU=WIN32 BUILD=release CFLAGS=/WX- LDFLAGS=/MANIFEST:NO
REM   x64 prompt:   nmake /f makefile.vc TARGET_CPU=X64   BUILD=release CFLAGS=/WX- LDFLAGS=/MANIFEST:NO
REM   ARM64 prompt: nmake /f makefile.vc TARGET_CPU=ARM64 BUILD=release CFLAGS=/WX- LDFLAGS=/MANIFEST:NO
cd /d %WXWIN%\build\msw
echo [bake] run the nmake line for THIS prompt, then press any key to verify...
pause >nul
if not exist "%WXWIN%\lib\vc_lib\wxmsw33u_core.lib" echo [warn] vc_lib missing - run WIN32 line
if not exist "%WXWIN%\lib\vc_x64_lib\wxmsw33u_core.lib" echo [warn] vc_x64_lib missing - run X64 line
if not exist "%WXWIN%\lib\vc_arm64_lib\wxmsw33u_core.lib" echo [warn] vc_arm64_lib missing - run ARM64 line
dir "%WXWIN%\lib\vc_*_lib\wxmsw33u_core.lib" "%WXWIN%\lib\vc_lib\wxmsw33u_core.lib" 2>nul

REM ---- 2. libserialport (per-arch build script) ----
REM   x86 prompt:   build_libserialport.bat Win32
REM   x64 prompt:   build_libserialport.bat x64
REM   ARM64 prompt: build_libserialport.bat ARM64
cd /d %SERIALPORT%
echo [bake] run build_libserialport.bat for THIS prompt, then press any key...
pause >nul
dir "%SERIALPORT%\Win32\lib\libserialport.dll" "%SERIALPORT%\Win64\lib\libserialport.dll" "%SERIALPORT%\ARM64\lib\libserialport.dll" 2>nul

REM ---- 3. curl (static lib, nmake from winbuild\) ----
REM   x86 prompt:   nmake /f Makefile.vc MODE=static VC=14 RTLIBCFG=release MACHINE=x86
REM   x64 prompt:   nmake /f Makefile.vc MODE=static VC=14 RTLIBCFG=release MACHINE=x64
REM   ARM64 prompt: nmake /f Makefile.vc MODE=static VC=14 RTLIBCFG=release MACHINE=ARM64
REM then copy_libs_to_build.bat (once, any prompt)
cd /d %CURL%\winbuild
echo [bake] run the curl nmake line for THIS prompt + copy_libs_to_build.bat, then key...
pause >nul
dir "%CURL%\build\Win32\VC14\LIB Release\libcurl.lib" "%CURL%\build\Win64\VC14\LIB Release\libcurl.lib" "%CURL%\build\arm64\VC14\LIB Release\libcurl.lib" 2>nul

REM ---- 4. SDL2: prebuilt - just verify per-arch DLLs exist ----
dir "%SDL%\lib\x86\SDL2.dll" "%SDL%\lib\x64\SDL2.dll" 2>nul
if exist "%SDL%\lib\arm64\SDL2.dll" ( dir "%SDL%\lib\arm64\SDL2.dll" ) else ( dir "%SDL%\lib\ARM64\SDL2.dll" 2>nul || echo [warn] check SDL arm64 DLL dir name )

REM ---- 5. toolchain presence ----
where MSBuild
if exist "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" ( echo ISCC OK ) else ( echo [warn] Inno Setup 6 missing >&2 )

echo [bake] done for this prompt. Repeat from the other two prompts, then:
echo   setx WXWIN /M, SDL, CURL, SERIALPORT as SYSTEM env vars, snapshot the VM.
