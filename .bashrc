# SPDX-License-Identifier: zlib-acknowledgement

[[ $- == *i* ]] || return

eval $(dircolors ~/.dir_colors/dircolors) 
alias ls='ls --color=auto'
alias grep='grep --color=auto'

export HISTTIMEFORMAT="%d/%m/%y %T "

source ~/.bash/git-prompt.sh
export GIT_PS1_SHOWDIRTYSTATE=1
export GIT_PS1_SHOWCOLORHINTS=true

dark_green='\[\e[01;32m\]'
dark_blue='\[\e[01;34m\]'
colour_reset='\[\e[m\]'

venv_name () { 
  v_name="${VIRTUAL_ENV##*/}"
  if [[ ! -z "${v_name}" ]]; then
    printf "($v_name)"
  fi
}

export PROMPT_COMMAND='venv_name;__git_ps1 "${dark_green}\u@\h${colour_reset}${dark_blue}:\w" "\\\$ ${colour_reset}"'

[[ ~ == "/root" ]] || cd ~/prog/personal

# NOTE(Ryan): This is to handle particular python scripts
export PATH="$PATH:~/.local/bin"
