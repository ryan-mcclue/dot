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


where choco > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo %underline%%bright_green%INSTALLING CHOCOLATEY%clear_style%
  
  powershell -c "Set-ExecutionPolicy Bypass -Scope Process -Force; !="!^
    "[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; !="!^
    "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"
  
  refreshenv
)




echo %underline%%bright_green%INSTALLING CHOCOLATEY PACKAGES%clear_style%

:: install WSL as it gives a better terminal --> follow msdn instructions. enable virtualisation in bios. install from windows store.
:: where mintty gives this directory
:: C:\ProgramData\chocolatey\lib\wsltty\tools\wslttyinstall\install.bat
:: launch from WSL Terminal shortcut
:: 
:: CursorType=block
:: solarized colour information
:: %APPDATA%/wsltty/config
:: 
:: files in windows accessed via mnt\c\....
:: add visual studio solarized also?
:: 
:: .vim/colors
:: 
:: git and gpg inside WSL
:: 
:: setup dircolors
:: remove block cursor


set "general_use=firefox vim vlc gimp bitwarden 7zip veracrypt ffmpeg youtube-dl python mintty"
set "cpp_toolchain=llvm visualstudio2019community visualstudio2019-workload-nativedesktop doxygen.install zeal"
set "cmake_install=cmake --installargs 'ADD_CMAKE_TO_PATH=System'"

choco install %general_use% %cpp_toolchain% -y
choco install %cmake_install% -y

refreshenv




echo %underline%%bright_green%CONFIGURING GIT%clear_style%

:: NOTE(Ryan): We only have to check for private key, as gpg will store public key information within it. 
::             This is because gpg uses single keyring for both public and private keys. $
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

for /f "delims=" %%a in ('where gpg') do set "gpg_loc=%%a"
git config --global gpg.program "%gpg_loc%"

git config --global commit.gpgsign true
:: TODO(Ryan): Make the git diff less obtrusive $
git config --global diff.tool vimdiff



echo %underline%%bright_green%INSTALLING GOOGLETEST%clear_style%

mkdir C:\Users\%USERNAME%\prog\libraries

git clone "https://github.com/google/googletest" C:\Users\%USERNAME%\prog\libraries\googletest

mkdir C:\Users\%USERNAME%\prog\libraries\googletest\build
pushd C:\Users\%USERNAME%\prog\libraries\googletest\build
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
rmdir /s /q C:\Users\%USERNAME%\prog\libraries\googletest\build

echo %underline%%bright_green%CONFIGURING GVIM%clear_style%

powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -OutFile .vimrc"
move .vimrc C:\Users\%USERNAME%\_gvimrc
move .vimrc C:\Users\%USERNAME%\_vimrc

:: TODO(Ryan): Avoid having to know the installation directory of gvim.exe.
::             Perhaps query chocolately? $
for /f "delims=" %%a in ('where /r C:\tools gvim') do set "gvim_loc=%%~dpa"
powershell -c "Invoke-WebRequest https://raw.githubusercontent.com/altercation/vim-colors-solarized/master/colors/solarized.vim -OutFile solarized.vim"
move solarized.vim "%gvim_loc%colors"

refreshenv
