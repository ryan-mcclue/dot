Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

Set-PSReadlineOption -EditMode vi
Set-PSReadLineOption -ViModeIndicator Cursor
Set-PSReadlineOption -BellStyle None

Import-Module posh-git

cd "C:\Users\ryan\prog"