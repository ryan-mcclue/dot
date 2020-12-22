# SPDX-License-Identifier: zlib-acknowledgement

#! /usr/bin/bash

readonly red='\033[31m'
readonly green='\033[32m'
readonly reset='\033[0m'

printf "${green}CONFIGURING GIT\n${reset}"

if [[ ! -f private-key.asc ]]; then
  printf "${red}Require private-key.asc to be in $(cwd)\n${reset}"
  exit 1
fi


# TODO(Ryan): Setup gpg-agent to use password caching and to use as SSH keys. $
gpg --import private-key.asc

git config --global user.name "Ryan McClue"
git config --global user.email "re.mcclue@protonmail.com"
# TODO(Ryan): Use SSH to communicate with git. $
git config --global credential.helper store

readonly gpg_keyid=$(gpg --list-keys | sed -n 4p)
git config --global user.signingkey ${gpg_keyid}

readonly gpg_loc=$(which gpg)
git config --global gpg.program ${gpg_loc}

git config --global commit.gpgsign true
git config --global diff.tool vimdiff
# NOTE(Ryan): This reorders the default vimdiff windows, i.e. the staged file is on the left. $
git config --global difftool.vimdiff.cmd 'vim -f -d -c "wincmd h" -c '\''cd "$GIT_PREFIX"'\'' "$REMOTE" "$LOCAL"'
# NOTE(Ryan): This allows the use of :cquit to exit the entire diff process. $
git config --global difftool.trustExitCode true
git config --global difftool.prompt false

mkdir $HOME/.bash
wget -O $HOME/.bash/git-prompt.sh https://raw.githubusercontent.com/git/git/master/contrib/completion/git-prompt.sh 
echo source $HOME/.bash/git-prompt.sh >> $HOME/.bashrc
echo export GIT_PS1_SHOWDIRTYSTATE=1 >> $HOME/.bashrc
echo export GIT_PS1_SHOWCOLORHINTS=true >> $HOME/.bashrc
echo export PROMPT_COMMAND='__git_ps1 "\u@\h:\w" "\\\$ "' >> $HOME/.bashrc

printf "${green}CONFIGURING VIM\n${reset}"
wget https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -O $HOME/.vimrc
mkdir -p $HOME/.vim/colors
wget https://raw.githubusercontent.com/altercation/vim-colors-solarized/master/colors/solarized.vim \
  -O $HOME/.vim/colors/solarized.vim


printf "${green}CONFIGURING BASH\n${reset}"
wget -O $HOME/.dircolors https://raw.githubusercontent.com/seebi/dircolors-solarized/master/dircolors.ansi-light 
# NOTE(Ryan): This is necessary to direct gpg to ask for password confirmation of key in the terminal. $
echo export GPG_TTY="$(tty)" >> $HOME/.bashrc
echo cd /mnt/c/Users/Ryan >> $HOME/.bashrc
echo export DISPLAY="$(sed -n 's/nameserver //p' /etc/resolv.conf):0" >> $HOME/.bashrc

# sudo apt install nasm libx11-dev gdb gcc universal-ctags
