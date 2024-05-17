:: SPDX-License-Identifier: zlib-acknowledgement
@echo off

if not exist build mkdir build

call cl /O2 /DTEST_BUILD=0 /DBUILD_DEBUG=0 /DBUILD_RELEASE=1 /Icode\ /nologo /FC /Z7 /link /MANIFEST:EMBED /INCREMENTAL:NO code\desktop.cpp /out:build\desktop

:: cl gdi32.lib msvcrt.lib raylib.lib winmm.lib filename.c -Ic:\path\to\raylib\include /link /libpath:c:\path\to\raylib\lib /NODEFAULTLIB:libcmt 

:: git clone https://github.com/Microsoft/vcpkg.git
:: cd vcpkg
:: bootstrap-vcpkg.bat
:: vcpkg integrate install
:: vcpkg install raylib:x64-windows
