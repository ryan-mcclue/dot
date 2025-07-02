Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

Set-PSReadlineOption -EditMode vi
Set-PSReadLineOption -ViModeIndicator Cursor
Set-PSReadlineOption -BellStyle None

Import-Module posh-git

function InteractiveShell {
    # Test each Arg for match of abbreviated '-NonInteractive' command.
    $NonInteractive = [Environment]::GetCommandLineArgs() | Where-Object{ $_ -like '-NonI*' }

    if ([Environment]::UserInteractive -and -not $NonInteractive) {
        return $true
    } else {
    	return $false
    }
}

if (InteractiveShell) { cd "C:\Users\ryan\prog" }
