:: SPDX-License-Identifier: zlib-acknowledgement

@echo off
setlocal EnableExtensions EnableDelayedExpansion

for /f %%a in ('echo prompt $E ^| cmd') do set "esc=%%a"
set clear_style=%esc%[0m
set underline=%esc%[4m
set bright_red=%esc%[91m
set bright_green=%esc%[92m


net session > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo %underline%%bright_red%Require administrator privileges to perform installations.%clear_style%
  exit /b 1
)

goto begin

where choco > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo %underline%%bright_green%INSTALLING CHOCOLATEY%clear_style%
  
  powershell -c "Set-ExecutionPolicy Bypass -Scope Process -Force; !="!^
    "[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; !="!^
    "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"
  
  refreshenv
)




echo %underline%%bright_green%INSTALLING CHOCOLATEY PACKAGES%clear_style%

set "general_use=firefox vim cmdermini vlc gimp bitwarden 7zip veracrypt ffmpeg youtube-dl"
set "cpp_toolchain=llvm visualstudio2019community visualstudio2019-workload-nativedesktop doxygen.install cmake git gpg4win zeal"

choco install %general_use% %cpp_toolchain% -y

:: TODO(Ryan): cmake isn't added to the %PATH%. Investigate why this is happening $

refreshenv




echo %underline%%bright_green%CONFIGURING GIT%clear_style%

if not exist private-key.asc (
  echo %underline%%bright_red%Require private-key.asc to be in %CD%%clear_style%
  exit /b 1
)
 
gpg --import private-key.asc


git config --global user.name "Ryan McClue"
git config --global user.email "re.mcclue@protonmail.com"
:: TODO(Ryan): Use SSH to communicate with git. $
git config --global credential.helper store

set line_num=0
for /f %%a in ('gpg --list-keys') do (
  if !line_num! EQU 3 (
    set gpg_keyid=%%a
  )
  set /a line_num=!line_num!+1
)
git config --global user.signingkey %gpg_keyid%

for /f "delims=" %%a in ('where gpg') do set "gpg_loc=%%~dpa"
:: NOTE(Ryan): Remove the last '\' so it does not escape the final " $
git config --global gpg.program "%gpg_loc:~0,-1%"

git config --global commit.gpgsign true


:begin

echo %underline%%bright_green%INSTALLING GOOGLETEST%clear_style%

mkdir C:\Users\%USERNAME%\prog\libraries

git clone "https://github.com/google/googletest" C:\Users\%USERNAME%\prog\libraries\googletest

mkdir C:\Users\%USERNAME%\prog\libraries\googletest\build
pushd C:\Users\%USERNAME%\prog\libraries\googletest\build
cmake .. && cmake --build . --target install --config Release -- -j %NUMBER_OF_PROCESSORS%
popd
rmdir /s C:\Users\%USERNAME%\prog\libraries\googletest\build

goto eof



echo %underline%%bright_green%CONFIGURING GVIM%clear_style%

powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/ryan-mcclue/cas/main/.gvimrc -OutFile .gvimrc"
powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -OutFile .vimrc"
for /f "delims=" %%a in ('where gvim') do set "gvim_loc=%%~dpa"
move .gvimrc "%gvim_loc%\_gvimrc"
move .vimrc "%gvim_loc%\_vimrc"

powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/altercation/vim-colors-solarized/master/colors/solarized.vim -OutFile solarized.vim"
move solarized.vim "%gvim_loc%\vimfiles\colors"

:eof
