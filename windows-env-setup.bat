:: SPDX-License-Identifier: zlib-acknowledgement

@echo off
setlocal EnableExtensions EnableDelayedExpansion

for /f %%a in ('echo prompt $E ^| cmd') do set "esc=%%a"
set clear_style=%esc%[0m
set underline=%esc%[4m
set bright_red=%esc%[91m
set bright_green=%esc%[92m
set bright_yellow=%esc%[93m


net session > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo %underline%%bright_red%Require administrator privileges to perform installations.%clear_style%
  exit /b 1
)

:: NOTE(Ryan): Although not checking for distros installed, this is sufficient for my purposes. $
where wsl > NUL > 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo %underline%%bright_red%Please install WSL ^(enable virtualisation in BIOS^) with an Ubuntu distro.%clear_style%
  exit /b 1
)

where choco > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo %underline%%bright_green%INSTALLING CHOCOLATEY%clear_style%
  
  powershell -c "Set-ExecutionPolicy Bypass -Scope Process -Force; !="!^
    "[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; !="!^
    "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"
  
  refreshenv
)


echo %underline%%bright_green%INSTALLING CHOCOLATEY PACKAGES%clear_style%


:: IMPORTANT(Ryan): Consider installing pycharm and configuring it to use python3 WSL interpreter. $
set "general_use=firefox vim vlc gimp bitwarden 7zip veracrypt ffmpeg youtube-dl wsltty"
set "cpp_toolchain=llvm visualstudio2019community visualstudio2019-workload-nativedesktop doxygen.install zeal"
set "cmake_install=cmake --installargs 'ADD_CMAKE_TO_PATH=System'"

choco install %general_use% %cpp_toolchain% -y
choco install %cmake_install% -y

refreshenv

:: TODO(Ryan): Fix vim processing keys incorrectly, e.g. slash goes to 'o', shift-letter goes to delete etc. $
powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/ryan-mcclue/cas/main/wsltty/config -OutFile config"
move config %APPDATA%\wsltty

echo %underline%%bright_yellow%TO FINISH INSTALL OF WSLTTY, RUN "WSL GENERATE SHORTCUTS.BAT"!%clear_style%





echo %underline%%bright_green%INSTALLING GOOGLETEST%clear_style%

powershell -c "Invoke-WebRequest https://github.com/google/googletest/archive/release-1.10.0.zip -OutFile googletest.zip"
mkdir C:\Users\Ryan\prog\libraries\googletest
7z x googletest.zip -oC:\Users\Ryan\prog\libraries\googletest

mkdir C:\Users\Ryan\prog\libraries\googletest\googletest-release-1.10.0\build
pushd C:\Users\Ryan\prog\libraries\googletest\googletest-release-1.10.0\build
:: TODO(Ryan): Avoid having to know the installation directory of llvm.
::             Perhaps query chocolately? $
for /f "tokens=3" %%a in ('clang --version') do (
  set llvm_version=%%a
  goto out
)
:out
set "llvm_dir=C:\Program Files\LLVM\lib\clang\%llvm_version%\"
cmake .. -DCMAKE_INSTALL_PREFIX="%llvm_dir%"
cmake --build . --target install --config Release --parallel %NUMBER_OF_PROCESSORS%

popd
rmdir /s /q C:\Users\Ryan\prog\libraries\googletest\googletest-release-1.10.0\build

echo %underline%%bright_green%CONFIGURING GVIM%clear_style%

powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -OutFile .vimrc"
move .vimrc C:\Users\Ryan\_gvimrc
move .vimrc C:\Users\Ryan\_vimrc

:: TODO(Ryan): Avoid having to know the installation directory of gvim.exe.
::             Perhaps query chocolately? $
for /f "delims=" %%a in ('where /r C:\tools gvim') do set "gvim_loc=%%~dpa"
powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/altercation/vim-colors-solarized/master/colors/solarized.vim -OutFile solarized.vim"
move solarized.vim "%gvim_loc%colors"

refreshenv
