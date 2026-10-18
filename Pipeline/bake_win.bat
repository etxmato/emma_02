@echo off
REM One-shot bake script for the emma02-win builder VM (run ONCE per fresh VM).
REM MODERN line: run each section from the matching "Native Tools Command Prompt for VS 2022"
REM (x86 for WIN32, x64 for X64, ARM64 for ARM64) - or run the whole file three
REM times, once per prompt. Requires: VS 2022 Build Tools (x86+x64+ARM64 toolsets),
REM Inno Setup 6, wxWidgets-3.3.3 source, SDL2-2.0.5 + libserialport + curl-win trees
REM as siblings of emma_02 (same layout as Marcel's Windows PC), OpenSSH server.
REM After this, set system env WXWIN/SDL/CURL/SERIALPORT (see bottom) and snapshot.
REM XP line (section 6): VS2008 Express + SP1, SDK 7.1 (x64 compiler), wx 3.2 tree
REM at WX32, VC9-built curl/serialport, Inno 5.6.1 at C:\Inno5, VC9 vcredists.
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

REM ---- 6. XP line (vc9, x86 + x64) - snapshot pre-xp-layer first ----
REM Snapshot: limactl snapshot create emma02-win --tag pre-xp-layer (VM running).
REM a. Install VS2008 Express + SP1, then Windows SDK 7.1 (VC9 Express ships no
REM    x64 compiler - SDK 7.1 adds it; the SDK 7.1 installer needs its Win11
REM    workarounds). Verify: vcbuild /help, and devenv.com under
REM    Microsoft Visual Studio 9.0\Common7\IDE.
REM b. wx 3.2.6 source STAGED 2026-10-18 at C:\wxWidgets-3.2 (zip from GitHub
REM    releases; NOTE the zip extracts flat - files had to be consolidated into
REM    the folder by hand) + WX32=C:\wxWidgets-3.2 set via setx /M.
REM    STILL NEEDED: the vc9 lib build itself (needs VS2008 devenv.com):
REM      devenv.com wx_vc9.sln /build "DLL Release|Win32"
REM      devenv.com wx_vc9.sln /build "DLL Release|x64"
REM    (solution first, then configuration). Starting from a VC9 prompt via
REM    vcbuild also works once the env is set.
REM    Verify: %WX32%\lib\vc_lib\wxmsw32u_core.lib + vc_x64_lib\wxmsw32u_core.lib.
REM    No ARM64 - VC9 predates it.
REM c. VC9-built deps - ALREADY BAKED 2026-10-18, do NOT rebuild:
REM    C:\curl-win\build\Win32\VC9 + Win64\VC9 LIB Release trees exist (vc9.vcproj
REM    references exactly those paths); C:\libserialport\Win32|Win64\lib\release\
REM    holds VC9-built DLL+LIB per arch; SDL2 DLLs are msvcrt-linked - reuse.
REM    STILL NEEDED from Marcel: C:\libserialport\Win32\lib\libserialport.lib +
REM    Win64\lib\libserialport.lib (VC9 import libs - vc9.vcproj links
REM    $(SERIALPORT)\Win32|Win64\lib directly, unlike vc17 which uses \lib\release).
REM d. Inno Setup 5.6.1 (last whose output runs on 2000/XP/2003) side-by-side
REM    at C:\Inno5 (Inno 6 stays for the modern line - build_win.bat picks
REM    C:\Inno5\ISCC.exe for --wx 3.2).
REM e. VC9 vcredists into IsTool\: vcredist9_x86.exe + vcredist9_x64.exe
REM    (VS2008 MSVCR90 redistributables - vc9 Release links /MD, RuntimeLibrary=2;
REM    the _xp.iss files bundle these, not the VC14 ones). Tracked payloads like
REM    the modern vcredists - the tar sync already carries IsTool/*.exe (only
REM    per-arch ISS outputs IsTool/x86|/x64|/arm64/*.exe are excluded). STILL
REM    NEEDED from Marcel.
REM f. setx WX32 /M (new ssh session to pick it up), then:
REM    limactl snapshot create emma02-win --tag xp-layer.
REM g. Build + verify: pipeline.sh --format exe --wx 3.2 --arch x86 (then x64),
REM    verify on a real XP SP3 guest - not Win11.
