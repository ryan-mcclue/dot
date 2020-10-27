# SPDX-License-Identifier: zlib-acknowledgement

#! /usr/bin/bash

readonly underline='\e[4m'
readonly bright_red='\e[91m'
readonly bright_green='\e[92m'

printf "${underline}${bright_green}CONFIGURING GIT\n"

if [[ ! -f private-key.asc ]]; then
  printf "${underline}${bright_red}Require private-key.asc to be in $(cwd)\n"
  exit 1
fi

gpg --import private-key.asc

git config --global user.name "Ryan McClue"
git config --global user.email "re.mcclue@protonmail.com"
# TODO(Ryan): Use SSH to communicate with git. $
git config --global credential.helper store

readonly gpg_keyid=$(gpg --list-keys | sed -n 3p)
git config --global user.signingkey ${gpg_keyid}

readonly gpg_loc=$(type -a gpg | sed -n 1q)
git config --global gpg.program ${gpg_loc}

git config --global commit.gpgsign true
# TODO(Ryan): Make the git diff less obtrusive $
git config --global diff.tool vimdiff

printf "${underline}${bright_green}CONFIGURING VIM\n"
wget https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -O $HOME/.vimrc
mkdir -p $HOME/.vim/colors
wget https://raw.githubusercontent.com/altercation/vim-colors-solarized/master/colors/solarized.vim \
  -O $HOME/.vim/colors/solarized.vim


printf "${underline}${bright_green}CONFIGURING BASH\n"
echo cd /mnt/c/Users/Ryan >> $HOME/.bashrc
